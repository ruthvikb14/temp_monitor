/**
 * @file app.h
 * @brief Top-level application: boot sequence and main-loop step.
 */
#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stdint.h>
#include "common/temp_types.h"
#include "drivers/eeprom_config.h"

typedef enum {
    APP_OK = 0,
    APP_ERR_CONFIG,     /* EEPROM unreadable / invalid -> fault indication */
    APP_ERR_SENSOR      /* no sensor driver for this revision */
} app_status_t;

typedef struct {
    bool            fault;
    config_status_t config_status;
    device_config_t config;
    const char     *sensor_name;
    temp_dC_t       last_temp_dC;
    uint16_t        last_raw;
    temp_level_t    level;
    uint32_t        samples_processed;
    uint32_t        samples_missed;
} app_info_t;

/** Load config, select sensor, start sampling. Call once after reset. */
app_status_t app_init(void);

/** One main-loop iteration: process the newest sample if there is one. */
void app_run_once(void);

/** Snapshot of the application state (diagnostics / demo output). */
void app_get_info(app_info_t *out);

#endif /* APP_H */
