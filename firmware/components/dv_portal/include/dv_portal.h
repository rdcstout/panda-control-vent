#pragma once

// DragonVent API/product-settings adapter for the shared dc_portal service.
// Call after dc_wifi_start() and product policy initialization.

#include "esp_err.h"

esp_err_t dv_portal_start(void);
esp_err_t dv_portal_stop(void);

// Clear printer/policy setup and retained stock provisioning data. Wi-Fi
// credentials are cleared by the reset caller after this succeeds.
esp_err_t dv_portal_reset_settings(void);
