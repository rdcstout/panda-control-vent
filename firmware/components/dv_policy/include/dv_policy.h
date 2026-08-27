#pragma once

// Vent policy: consumes Moonraker status, commands the motor driver. Owns the
// AUTO/MANUAL mode toggle and the temperature hysteresis for auto decisions.
//
// AUTO mode defaults to a Panda Breath-style commanded-bed rule: below the
// seal threshold opens the vents, at/above it closes them. Advanced mode can
// apply reported-material rules first. Idle cooldown uses actual-bed hysteresis.
//
// MANUAL mode:
//   target = whatever dv_policy_set_manual_target set most recently

#include <stdbool.h>
#include "esp_err.h"
#include "dv_motor.h"

typedef enum {
    DV_POLICY_MODE_AUTO,
    DV_POLICY_MODE_MANUAL,
} dv_policy_mode_t;

typedef enum {
    DV_AUTOMATION_SIMPLE,
    DV_AUTOMATION_ADVANCED,
} dv_automation_mode_t;

esp_err_t dv_policy_start(void);

esp_err_t dv_policy_set_mode(dv_policy_mode_t mode);   // persisted to NVS
dv_policy_mode_t dv_policy_get_mode(void);

esp_err_t dv_policy_set_manual_target(dv_motor_target_t t);   // persisted
dv_motor_target_t dv_policy_get_target(void);          // whatever we're commanding

// SIMPLE follows the printer's commanded bed temperature. ADVANCED applies
// material rules when material metadata is available, then falls back to the
// same commanded-bed threshold. The default is SIMPLE.
esp_err_t dv_policy_set_automation_mode(dv_automation_mode_t mode);  // persisted
dv_automation_mode_t dv_policy_get_automation_mode(void);
esp_err_t dv_policy_set_seal_threshold(float bed_target_c);          // persisted
float dv_policy_get_seal_threshold(void);

// Bed-temperature hysteresis for AUTO mode (idle/complete only — during a
// print the material rule wins). Default 45 / 35 °C. OPEN must be strictly
// greater than CLOSE.
esp_err_t dv_policy_get_thresholds(float *bed_open_c, float *bed_close_c);
esp_err_t dv_policy_set_thresholds(float bed_open_c, float bed_close_c);

// Filament rules for AUTO mode. During a print, if the detected filament name
// begins with a rule's name, the vent follows that rule (seal = closed, else
// vent = open); an unmatched filament vents (safe default for PLA-family).
// Names are matched case-insensitively as prefixes ("PLA" matches "PLA+").
#define DV_FILAMENT_MAX 20
typedef struct {
    char name[12];   // filament prefix, e.g. "PLA", "ABS"
    bool seal;       // true = seal chamber (closed); false = vent (open)
} dv_filament_rule_t;

int       dv_policy_filament_rules(dv_filament_rule_t *out, int max);   // returns count
esp_err_t dv_policy_set_filament_rules(const dv_filament_rule_t *rules, int count);  // persisted

esp_err_t dv_policy_clear(void);   // wipe all persisted policy state
