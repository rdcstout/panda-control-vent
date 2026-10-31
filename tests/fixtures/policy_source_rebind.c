#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "dv_policy.h"
#include "dv_bambu_state.h"

// Host adapters for ESP-IDF scheduling and the printer-client boundaries.
typedef void *SemaphoreHandle_t;
typedef void *TaskHandle_t;
#define BED_SEAL_C_DEFAULT 85.0f
#define BED_OPEN_C_DEFAULT 45.0f
#define BED_CLOSE_C_DEFAULT 35.0f
#define TICK_MS 1000
#define portMAX_DELAY 0
#define pdPASS 1
#define pdMS_TO_TICKS(n) (n)
#define ESP_LOGI(...) ((void)0)
#define dc_evlog_add(...) ((void)snprintf(log_buffer, sizeof(log_buffer), __VA_ARGS__))
static char log_buffer[256];
#define xSemaphoreTake(...) ((void)0)
#define xSemaphoreGive(...) ((void)0)
#define xSemaphoreCreateMutex() ((void *)1)
static jmp_buf tick_end;
static void vTaskDelay(int ms) { assert(ms == TICK_MS); longjmp(tick_end, 1); }
static int xTaskCreate(void (*task)(void *), const char *name, int stack, void *arg, int priority, TaskHandle_t *out) {
    (void)task; (void)name; (void)stack; (void)arg; (void)priority; *out = (void *)1; return pdPASS;
}
typedef enum { DC_SRC_KLIPPER, DC_SRC_BAMBU, DC_SRC_HA, DC_SRC_NONE } dc_ctl_source_t;
typedef enum { DC_MK_DISABLED, DC_MK_DISCONNECTED, DC_MK_CONNECTING, DC_MK_CONNECTED, DC_MK_SUBSCRIBED } dc_moonraker_state_t;
typedef enum { DC_PRINTER_UNKNOWN, DC_PRINTER_IDLE, DC_PRINTER_PREPARING, DC_PRINTER_PRINTING, DC_PRINTER_PAUSED, DC_PRINTER_COMPLETE, DC_PRINTER_ERROR } dc_printer_state_t;
typedef struct {
    dc_moonraker_state_t state;
    dc_printer_state_t printer;
    float bed_temp, bed_target;
    char material[16];
    bool db_present, db_connected, db_fault, db_inhibited;
    float db_target;
    char db_mode[12];
} dc_moonraker_status_t;
static dc_ctl_source_t selected = DC_SRC_KLIPPER;
static dc_moonraker_status_t moon;
static dc_bambu_status_t bambu;
static int source_reads, moon_reads, bambu_reads;
static esp_err_t moon_result = ESP_OK, bambu_result = ESP_OK;
static dc_ctl_source_t dc_source_get(void) { source_reads++; return selected; }
static const char *dc_source_str(dc_ctl_source_t s) { (void)s; return "none"; }
static const char *dc_printer_state_str(dc_printer_state_t s) { (void)s; return "klipper"; }
esp_err_t dc_moonraker_get_status(dc_moonraker_status_t *out) { moon_reads++; *out=moon; return moon_result; }
esp_err_t dc_bambu_get_status(dc_bambu_status_t *out) { bambu_reads++; *out=bambu; return bambu_result; }
static int motor_writes;
static dv_motor_target_t motors[2] = {DV_MOTOR_TARGET_CLOSED, DV_MOTOR_TARGET_CLOSED};
int dv_motor_active_groups(void) { return 2; }
esp_err_t dv_motor_set_target(int group, dv_motor_target_t target) { assert(group >= 0 && group < 2); motors[group]=target; motor_writes++; return ESP_OK; }
/* INJECT:globals */
/* INJECT:read_auto_input */
/* INJECT:apply_target */
/* INJECT:decide_auto_target */
/* INJECT:policy_task */
static void load_persisted(void) {
    memcpy(s_rules, DEFAULT_FILAMENT_RULES, sizeof(DEFAULT_FILAMENT_RULES));
    s_rule_count = sizeof(DEFAULT_FILAMENT_RULES) / sizeof(DEFAULT_FILAMENT_RULES[0]);
}
/* INJECT:dv_policy_start */
static void tick(dv_motor_target_t expected) {
    source_reads=moon_reads=bambu_reads=0;
    if (setjmp(tick_end)==0) policy_task(NULL);
    assert(s_current_target==expected);
    assert(motors[0]==expected && motors[1]==expected);
    assert(source_reads==1);
    assert(moon_reads==(selected==DC_SRC_KLIPPER));
    assert(bambu_reads==(selected==DC_SRC_BAMBU));
}
int main(void) {
    assert(dv_policy_start()==ESP_OK); // boot unconfigured on Klipper
    assert(dv_policy_start()==ESP_ERR_INVALID_STATE);
    // Match the observed field failure: bind a live PETG Bambu without reboot.
    selected=DC_SRC_BAMBU;
    bambu=(dc_bambu_status_t){.state=DC_BAMBU_SUBSCRIBED,.connected=true,.printing=true,.bed_temp=70,.bed_target=70,.print_state=DC_BAMBU_PRINT_PRINTING};
    strcpy(bambu.filament,"PETG");
    tick(DV_MOTOR_TARGET_OPEN);
    // Seal threshold is inclusive; material does not override SIMPLE.
    bambu.bed_target=85; tick(DV_MOTOR_TARGET_CLOSED);
    bambu.bed_target=84.9f; tick(DV_MOTOR_TARGET_OPEN);
    bambu.bed_target=85.1f; tick(DV_MOTOR_TARGET_CLOSED);
    bambu.state=DC_BAMBU_DISCONNECTED; bambu.bed_target=70; tick(DV_MOTOR_TARGET_CLOSED);
    bambu.state=DC_BAMBU_SUBSCRIBED; tick(DV_MOTOR_TARGET_OPEN);
    bambu_result=1; bambu.bed_target=100; tick(DV_MOTOR_TARGET_OPEN); bambu_result=ESP_OK;
    // Rebind back to Klipper while stale Bambu data requests the opposite target.
    selected=DC_SRC_KLIPPER;
    moon=(dc_moonraker_status_t){.state=DC_MK_SUBSCRIBED,.printer=DC_PRINTER_PRINTING,.bed_temp=100,.bed_target=100};
    tick(DV_MOTOR_TARGET_CLOSED);
    moon.bed_target=70; tick(DV_MOTOR_TARGET_OPEN);
    moon.db_present=moon.db_connected=true; moon.db_target=50; strcpy(moon.db_mode,"auto"); tick(DV_MOTOR_TARGET_CLOSED);
    moon.db_inhibited=true; tick(DV_MOTOR_TARGET_OPEN);
    moon.db_inhibited=false; moon.db_fault=true; tick(DV_MOTOR_TARGET_OPEN);
    moon.db_fault=false; strcpy(moon.db_mode,"off"); tick(DV_MOTOR_TARGET_OPEN);
    moon.db_present=false; moon.printer=DC_PRINTER_ERROR; moon.bed_target=100; tick(DV_MOTOR_TARGET_OPEN);
    moon.printer=DC_PRINTER_PRINTING; moon.state=DC_MK_DISCONNECTED; tick(DV_MOTOR_TARGET_OPEN);
    moon.state=DC_MK_SUBSCRIBED; moon_result=1; tick(DV_MOTOR_TARGET_OPEN); moon_result=ESP_OK;
    // NONE and unsupported external controllers must hold, never reuse clients.
    selected=DC_SRC_NONE; tick(DV_MOTOR_TARGET_OPEN);
    selected=DC_SRC_HA; tick(DV_MOTOR_TARGET_OPEN);
    selected=DC_SRC_BAMBU; bambu.state=DC_BAMBU_CONNECTING; tick(DV_MOTOR_TARGET_OPEN);
    bambu.state=DC_BAMBU_SUBSCRIBED; tick(DV_MOTOR_TARGET_CLOSED);
    // Manual targets survive source changes; execute the real task's mode branch.
    s_mode=DV_POLICY_MODE_MANUAL; s_manual_target=DV_MOTOR_TARGET_OPEN; tick(DV_MOTOR_TARGET_OPEN);
    selected=DC_SRC_NONE; s_manual_target=DV_MOTOR_TARGET_CLOSED; tick(DV_MOTOR_TARGET_CLOSED);
    selected=DC_SRC_KLIPPER; tick(DV_MOTOR_TARGET_CLOSED);
    s_mode=DV_POLICY_MODE_AUTO;
    selected=DC_SRC_BAMBU; bambu.printing=false; bambu.bed_target=0;
    bambu.bed_temp=45; tick(DV_MOTOR_TARGET_CLOSED);
    bambu.bed_temp=45.1f; tick(DV_MOTOR_TARGET_OPEN);
    bambu.bed_temp=35; tick(DV_MOTOR_TARGET_OPEN);
    bambu.bed_temp=34.9f; tick(DV_MOTOR_TARGET_CLOSED);
    bambu.bed_temp=40; tick(DV_MOTOR_TARGET_CLOSED);
    // Advanced material behavior remains intact after rebinding.
    s_automation_mode=DV_AUTOMATION_ADVANCED; bambu.printing=true; bambu.bed_target=100; tick(DV_MOTOR_TARGET_OPEN);
    strcpy(bambu.filament,"ABS"); bambu.bed_target=70; tick(DV_MOTOR_TARGET_CLOSED);
    strcpy(bambu.filament,"unknown"); tick(DV_MOTOR_TARGET_OPEN);
    bambu.bed_target=0; tick(DV_MOTOR_TARGET_OPEN);
    assert(motor_writes>0);
    puts("policy source rebinding, thresholds, heater interlock, disconnect hold, manual and cooldown passed");
    return 0;
}
