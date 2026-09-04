// SPDX-License-Identifier: MIT
#pragma once
#include "dc_bambu.h"

// Cached Bambu phases are only live while subscribed. Keep this presentation
// rule shared by lighting, the API, and diagnostic logs; do not alter the
// client's cached job history or the vent's connection-loss hold policy.
static inline dc_bambu_print_state_t dv_bambu_live_phase(const dc_bambu_status_t *status)
{
    if (!status->connected) return DC_BAMBU_PRINT_UNKNOWN;
    switch (status->print_state) {
    case DC_BAMBU_PRINT_IDLE:
    case DC_BAMBU_PRINT_DOWNLOADING:
    case DC_BAMBU_PRINT_PREPARING:
    case DC_BAMBU_PRINT_PRINTING:
    case DC_BAMBU_PRINT_PAUSED:
    case DC_BAMBU_PRINT_COMPLETE:
    case DC_BAMBU_PRINT_ERROR:
        return status->print_state;
    default:
        return status->error ? DC_BAMBU_PRINT_ERROR
             : status->printing ? DC_BAMBU_PRINT_PRINTING : DC_BAMBU_PRINT_IDLE;
    }
}

static inline const char *dv_bambu_live_state_str(const dc_bambu_status_t *status)
{
    switch (dv_bambu_live_phase(status)) {
    case DC_BAMBU_PRINT_IDLE:        return "idle";
    case DC_BAMBU_PRINT_DOWNLOADING: return "downloading";
    case DC_BAMBU_PRINT_PREPARING:   return "preparing";
    case DC_BAMBU_PRINT_PRINTING:    return "printing";
    case DC_BAMBU_PRINT_PAUSED:      return "paused";
    case DC_BAMBU_PRINT_COMPLETE:    return "complete";
    case DC_BAMBU_PRINT_ERROR:       return "error";
    default:                        return "unknown";
    }
}
