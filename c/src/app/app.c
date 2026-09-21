#include "app/app.h"

#include <string.h>
#include "app/temp_monitor.h"
#include "drivers/led_indicator.h"
#include "drivers/temp_acquisition.h"
#include "drivers/temp_sensor.h"
#include "hal/hal_i2c.h"

#ifndef APP_HYSTERESIS_DC
#define APP_HYSTERESIS_DC  10   /* 1.0 degC */
#endif

static struct {
    app_info_t           info;
    const temp_sensor_t *sensor;
    temp_monitor_t       monitor;
    temp_level_t         shown_level;   /* what the LEDs currently show */
    bool                 led_valid;     /* false until first update     */
} s_app;

static void enter_fault(void) {
    s_app.info.fault = true;
    temp_acq_stop();
    led_indicator_show_fault();
}

app_status_t app_init(void) {
    memset(&s_app, 0, sizeof s_app);

    /* LEDs first: defined, all-off state as early as possible. */
    led_indicator_init();

    (void)hal_i2c_init();
    s_app.info.config_status = eeprom_config_load(&s_app.info.config);
    if (s_app.info.config_status != CONFIG_OK) {
        enter_fault();
        return APP_ERR_CONFIG;
    }

    s_app.sensor = temp_sensor_select(s_app.info.config.revision);
    if (s_app.sensor == NULL) {
        enter_fault();
        return APP_ERR_SENSOR;
    }
    s_app.info.sensor_name = s_app.sensor->name;

    temp_monitor_init(&s_app.monitor, APP_HYSTERESIS_DC);

    /* Sampling starts last, when everything that consumes samples is ready. */
    temp_acq_init();
    temp_acq_start();
    return APP_OK;
}

void app_run_once(void) {
    temp_sample_t sample;
    temp_level_t level;

    if (s_app.info.fault) {
        return;
    }
    if (!temp_acq_get_latest(&sample)) {
        return;
    }

    s_app.info.last_raw = sample.raw;
    s_app.info.last_temp_dC = s_app.sensor->to_deci_celsius(sample.raw);
    s_app.info.samples_processed++;
    s_app.info.samples_missed += sample.missed;

    level = temp_monitor_update(&s_app.monitor, s_app.info.last_temp_dC);
    s_app.info.level = level;

    /* Touch the GPIOs only when the level changes. */
    if (!s_app.led_valid || (level != s_app.shown_level)) {
        led_indicator_show(level);
        s_app.shown_level = level;
        s_app.led_valid = true;
    }
}

void app_get_info(app_info_t *out) {
    if (out != NULL) {
        *out = s_app.info;
    }
}
