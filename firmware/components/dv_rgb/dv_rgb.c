#include "dv_rgb.h"

#include "dc_lighting.h"
#include "dv_board.h"
#include "dv_motor.h"

#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "nvs.h"

#include <math.h>
#include <string.h>

static const char *TAG = "dv_rgb";

/* Physical output topology belongs to DragonVent; the shared dc_lighting engine
 * owns transport, timing, brightness, direction, and effects. The strips use
 * SPI+DMA: the classic ESP32 RMT refill ISR can be starved and corrupt a WS2812
 * frame, so DragonVent declares SPI+DMA transport for its two outputs. */
#define LEDS_PER_STRIP 30
#define VENT_FIRST_PIXEL 0
#define VENT_PIXEL_COUNT 11
#define CHAMBER_FIRST_PIXEL 11
#define CHAMBER_PIXEL_COUNT 5
#define MAX_STRIPS 2
#define CFG_NVS_NS "app_nvs"
#define CFG_NVS_KEY "lighting"
#define IDLE_DIM_DELAY_US 3000000LL

static const gpio_num_t STRIP_GPIO[MAX_STRIPS] = { DV_PIN_RGB_STRIP_0, DV_PIN_RGB_STRIP_1 };
static int s_count;
static SemaphoreHandle_t s_lock;

static dv_lighting_t s_cfg = {
    .enabled = true, .brightness = 255,
    .open = {0, 0, 255}, .closed = {255, 0, 0}, .printing = {0, 255, 0},
    .use_printing = false, .use_temp = false, .temp_min_c = 25, .temp_max_c = 60,
    .effect = DV_FX_SOLID, .speed = 128,
    .error = {255, 0, 0}, .use_error = false,
    .mode = DV_LIGHT_MODE_VENT,
    .idle = {255, 255, 255}, .prep = {248, 163, 35}, .paused = {255, 255, 255}, .complete = {0, 255, 42},
    .chamber_independent = false,
    .chamber = {
        .enabled = true, .brightness = 255,
        .open = {255, 255, 255}, .closed = {255, 255, 255}, .printing = {255, 255, 255},
        .use_printing = false, .use_temp = false, .temp_min_c = 25, .temp_max_c = 60,
        .effect = DV_FX_SOLID, .speed = 128,
        .error = {255, 0, 0}, .use_error = false,
        .mode = DV_LIGHT_MODE_VENT,
        .idle = {255, 255, 255}, .prep = {248, 163, 35},
        .paused = {255, 255, 255}, .complete = {0, 255, 42},
        .dim_idle = false,
        .follow_printer_light = false,
    },
};

static int s_target = DV_MOTOR_TARGET_CLOSED;
static int s_pstatus = DV_PS_NONE;
static bool s_error;
static bool s_printer_light_known, s_printer_light_on;
static float s_bed = NAN;
static int64_t s_idle_dim_after_us;

static const uint8_t *printer_status_color(void)
{
    switch (s_pstatus) {
    case DV_PS_PREPARING: return s_cfg.prep;
    case DV_PS_PRINTING: return s_cfg.printing;
    case DV_PS_PAUSED: return s_cfg.paused;
    case DV_PS_COMPLETE: return s_cfg.complete;
    case DV_PS_ERROR: return s_cfg.error;
    default: return s_cfg.idle;
    }
}

static const uint8_t *profile_status_color(const dv_lighting_profile_t *cfg)
{
    switch (s_pstatus) {
    case DV_PS_PREPARING: return cfg->prep;
    case DV_PS_PRINTING: return cfg->printing;
    case DV_PS_PAUSED: return cfg->paused;
    case DV_PS_COMPLETE: return cfg->complete;
    case DV_PS_ERROR: return cfg->error;
    default: return cfg->idle;
    }
}

/* Resolve the state base color (no brightness). PRINTER mode follows printer
 * status; VENT mode follows the vent (open/closed, printing override, temp). */
static dc_rgb_t base_color(void)
{
    const uint8_t *base;
    uint8_t gradient[3];
    if (s_cfg.mode == DV_LIGHT_MODE_PRINTER) {
        base = printer_status_color();
    } else if (s_cfg.use_temp && !isnan(s_bed)) {
        int lo = s_cfg.temp_min_c, hi = s_cfg.temp_max_c;
        float factor = hi > lo ? (s_bed - lo) / (float)(hi - lo) : 0;
        if (factor < 0) factor = 0;
        if (factor > 1) factor = 1;
        for (int i = 0; i < 3; ++i)
            gradient[i] = (uint8_t)(s_cfg.open[i] + (int)((s_cfg.closed[i] - s_cfg.open[i]) * factor));
        base = gradient;
    } else {
        base = s_target == DV_MOTOR_TARGET_OPEN ? s_cfg.open : s_cfg.closed;
    }
    return (dc_rgb_t){base[0], base[1], base[2]};
}

static dc_rgb_t profile_base_color(const dv_lighting_profile_t *cfg)
{
    const uint8_t *base;
    uint8_t gradient[3];
    if (cfg->mode == DV_LIGHT_MODE_PRINTER) {
        base = profile_status_color(cfg);
    } else if (cfg->use_temp && !isnan(s_bed)) {
        int lo = cfg->temp_min_c, hi = cfg->temp_max_c;
        float factor = hi > lo ? (s_bed - lo) / (float)(hi - lo) : 0;
        if (factor < 0) factor = 0;
        if (factor > 1) factor = 1;
        for (int i = 0; i < 3; ++i)
            gradient[i] = (uint8_t)(cfg->open[i] + (int)((cfg->closed[i] - cfg->open[i]) * factor));
        base = gradient;
    } else {
        base = s_target == DV_MOTOR_TARGET_OPEN ? cfg->open : cfg->closed;
    }
    return (dc_rgb_t){base[0], base[1], base[2]};
}

static uint8_t profile_brightness(const dv_lighting_profile_t *cfg)
{
    bool idle = s_pstatus == DV_PS_NONE || s_pstatus == DV_PS_IDLE;
    bool delay_elapsed = s_idle_dim_after_us <= 0 ||
                         esp_timer_get_time() >= s_idle_dim_after_us;
    if (cfg->dim_idle && cfg->mode == DV_LIGHT_MODE_PRINTER && idle && delay_elapsed)
        return (uint8_t)((cfg->brightness + 2u) / 5u);
    return cfg->brightness;
}

/* Map DragonVent's effect ids (stable 0..7 for the SPA dropdown and saved NVS
 * configs) onto the shared engine's enum, which numbers effects differently. */
static dc_lighting_effect_t core_effect(uint8_t effect)
{
    switch (effect) {
    case DV_FX_CYCLE: return DC_LIGHTING_CYCLE;
    case DV_FX_RAINBOW: return DC_LIGHTING_RAINBOW;
    case DV_FX_BREATHE: return DC_LIGHTING_BREATHE;
    case DV_FX_STROBE: return DC_LIGHTING_STROBE;
    case DV_FX_WAVE: return DC_LIGHTING_WAVE;
    case DV_FX_MARQUEE: return DC_LIGHTING_MARQUEE;
    case DV_FX_CYLON: return DC_LIGHTING_CYLON;
    default: return DC_LIGHTING_SOLID;
    }
}

/* Caller holds s_lock. Product policy resolves color + error precedence; the
 * shared renderer paints the selected color/effect over the declared layout. */
static void apply_locked(void)
{
    if (s_cfg.chamber_independent) {
        dc_lighting_zone_t zones[2] = {
            {
                .first_pixel = VENT_FIRST_PIXEL,
                .pixel_count = VENT_PIXEL_COUNT,
                .enabled = s_cfg.enabled,
                .brightness = s_cfg.brightness,
                .color = base_color(),
                .effect = core_effect(s_cfg.effect),
                .speed = s_cfg.speed,
            },
            {
                .first_pixel = CHAMBER_FIRST_PIXEL,
                .pixel_count = CHAMBER_PIXEL_COUNT,
                .enabled = s_cfg.chamber.enabled &&
                           (!s_cfg.chamber.follow_printer_light ||
                            !s_printer_light_known || s_printer_light_on),
                .brightness = profile_brightness(&s_cfg.chamber),
                .color = profile_base_color(&s_cfg.chamber),
                .effect = core_effect(s_cfg.chamber.effect),
                .speed = s_cfg.chamber.speed,
            },
        };
        if (s_cfg.use_error && s_error) {
            zones[0].color = (dc_rgb_t){s_cfg.error[0], s_cfg.error[1], s_cfg.error[2]};
            zones[0].effect = DC_LIGHTING_STROBE;
            zones[0].speed = 64;
        }
        if (s_cfg.chamber.use_error && s_error) {
            zones[1].color = (dc_rgb_t){s_cfg.chamber.error[0], s_cfg.chamber.error[1], s_cfg.chamber.error[2]};
            zones[1].effect = DC_LIGHTING_STROBE;
            zones[1].speed = 64;
        }
        (void)dc_lighting_set_brightness(255);
        (void)dc_lighting_set_zones(zones, 2);
        return;
    }

    (void)dc_lighting_clear_zones();
    (void)dc_lighting_set_brightness(s_cfg.brightness);
    if (!s_cfg.enabled) { (void)dc_lighting_off(); return; }
    /* A print error takes top precedence and flashes to demand attention. */
    if (s_cfg.use_error && s_error) {
        (void)dc_lighting_set((dc_rgb_t){s_cfg.error[0], s_cfg.error[1], s_cfg.error[2]}, DC_LIGHTING_STROBE, 64);
        return;
    }
    /* Cylon uses the resolved state color, like the other effects. */
    (void)dc_lighting_set(base_color(), core_effect(s_cfg.effect), s_cfg.speed);
}

void dv_rgb_get_config(dv_lighting_t *out)
{
    if (!out) return;
    if (s_lock) xSemaphoreTake(s_lock, portMAX_DELAY);
    *out = s_cfg;
    if (s_lock) xSemaphoreGive(s_lock);
}

esp_err_t dv_rgb_set_config(const dv_lighting_t *cfg)
{
    if (!cfg || !s_lock) return ESP_ERR_INVALID_ARG;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_cfg = *cfg;
    /* Legacy DragonVent printing overrides are intentionally retired. Vent
     * mode follows vent state; printer mode owns the printing color. */
    s_cfg.use_printing = false;
    s_cfg.chamber.use_printing = false;
    if (s_cfg.effect > DV_FX_CYLON) s_cfg.effect = DV_FX_SOLID;
    if (s_cfg.chamber.effect > DV_FX_CYLON) s_cfg.chamber.effect = DV_FX_SOLID;
    for (int i = 0; i < s_count && i < MAX_STRIPS; ++i)
        (void)dc_lighting_set_output_reverse(i, s_cfg.rev_strip[i]);
    apply_locked();
    nvs_handle_t nvs;
    esp_err_t err = nvs_open(CFG_NVS_NS, NVS_READWRITE, &nvs);
    if (err == ESP_OK) {
        err = nvs_set_blob(nvs, CFG_NVS_KEY, &s_cfg, sizeof(s_cfg));
        if (err == ESP_OK) err = nvs_commit(nvs);
        nvs_close(nvs);
    }
    xSemaphoreGive(s_lock);
    return err;
}

static void cfg_load(void)
{
    nvs_handle_t nvs;
    if (nvs_open(CFG_NVS_NS, NVS_READONLY, &nvs) != ESP_OK) return;
    size_t size = 0;
    if (nvs_get_blob(nvs, CFG_NVS_KEY, NULL, &size) == ESP_OK && size > 0 && size <= sizeof(s_cfg)) {
        uint8_t saved[sizeof(s_cfg)];
        size_t capacity = sizeof(saved);
        if (nvs_get_blob(nvs, CFG_NVS_KEY, saved, &capacity) == ESP_OK) {
            memcpy(&s_cfg, saved, capacity);
            s_cfg.use_printing = false;
            s_cfg.chamber.use_printing = false;
            /* Migrate a legacy global reverse into both per-strip reverse flags. */
            if (s_cfg.reverse) { s_cfg.rev_strip[0] = 1; s_cfg.rev_strip[1] = 1; s_cfg.reverse = false; }
            ESP_LOGI(TAG, "loaded lighting config (%u B) from NVS", (unsigned)capacity);
        }
    }
    nvs_close(nvs);
}

int dv_rgb_strip_count(void) { return s_count; }

esp_err_t dv_rgb_start(void)
{
    if (s_lock) return ESP_ERR_INVALID_STATE;
    int groups = dv_motor_active_groups();
    int strips = groups >= 2 ? 2 : 0;
    if (strips == 0) {
        ESP_LOGI(TAG, "no RGB strips (config-detect reports %d motor groups)", groups);
        return ESP_OK;
    }
    s_lock = xSemaphoreCreateMutex();
    if (!s_lock) return ESP_ERR_NO_MEM;
    cfg_load();

    dc_lighting_output_t outputs[MAX_STRIPS] = {0};
    for (int i = 0; i < strips; ++i) {
        outputs[i] = (dc_lighting_output_t){
            .gpio = STRIP_GPIO[i], .pixels = LEDS_PER_STRIP, .reverse = s_cfg.rev_strip[i],
            .transport = DC_LIGHTING_TRANSPORT_SPI_DMA,
            .spi_host = i == 0 ? SPI2_HOST : SPI3_HOST,
        };
    }
    esp_err_t err = dc_lighting_start(&(dc_lighting_config_t){
        .outputs = outputs, .output_count = strips, .brightness = s_cfg.brightness,
        .fps = 33, .layout = DC_LIGHTING_LAYOUT_PER_OUTPUT,
    });
    if (err != ESP_OK) return err;
    s_count = strips;
    xSemaphoreTake(s_lock, portMAX_DELAY); apply_locked(); xSemaphoreGive(s_lock);
    ESP_LOGI(TAG, "started %d SPI+DMA WS2812 output(s), %d LEDs each", s_count, LEDS_PER_STRIP);
    return ESP_OK;
}

void dv_rgb_update(int target, int status, float bed_temp_c,
                   bool printer_light_known, bool printer_light_on)
{
    if (!s_lock) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_target = target;
    if (status != s_pstatus) {
        // Make every real job-ending transition visible: show the Idle color at
        // full brightness for three seconds before Dim while idle takes effect.
        // Startup already in Idle (NONE -> IDLE) dims immediately, while pause
        // and resume never enter Idle and therefore do not start this delay.
        if (status == DV_PS_IDLE && s_pstatus != DV_PS_NONE && s_pstatus != DV_PS_IDLE)
            s_idle_dim_after_us = esp_timer_get_time() + IDLE_DIM_DELAY_US;
        else if (status != DV_PS_IDLE)
            s_idle_dim_after_us = 0;
    }
    s_pstatus = status;
    s_error = status == DV_PS_ERROR;
    s_bed = bed_temp_c;
    s_printer_light_known = printer_light_known;
    s_printer_light_on = printer_light_on;
    apply_locked();
    xSemaphoreGive(s_lock);
}
