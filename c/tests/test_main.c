/**
 * @file test_main.c
 * @brief Unit + integration tests on the PC mock HAL. No framework needed.
 */
#include <stdio.h>
#include <string.h>
#include "app/app.h"
#include "app/temp_monitor.h"
#include "drivers/eeprom_config.h"
#include "drivers/temp_acquisition.h"
#include "drivers/temp_sensor.h"
#include "mock_hal.h"

static int s_run, s_failed;

#define CHECK(cond) do {                                                  \
        s_run++;                                                          \
        if (!(cond)) {                                                    \
            s_failed++;                                                   \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                 \
    } while (0)

static bool leds_are(bool r, bool y, bool g) {
    return mock_gpio_get(BOARD_PIN_LED_RED) == r &&
           mock_gpio_get(BOARD_PIN_LED_YELLOW) == y &&
           mock_gpio_get(BOARD_PIN_LED_GREEN) == g;
}

static void test_sensor_conversion(void) {
    const temp_sensor_t *a = temp_sensor_select(HW_REV_A);
    const temp_sensor_t *b = temp_sensor_select(HW_REV_B);

    CHECK(a != NULL && b != NULL);
    CHECK(a->to_deci_celsius(10) == 100);     /* spec example: 10 -> 10 C  */
    CHECK(b->to_deci_celsius(100) == 100);    /* spec example: 100 -> 10 C */
    CHECK(a->to_deci_celsius(105) == 1050);
    CHECK(b->to_deci_celsius(1050) == 1050);
    CHECK(temp_sensor_select((hw_revision_t)5) == NULL);
}

static void test_classification_boundaries(void) {
    CHECK(temp_monitor_classify(0)    == TEMP_LEVEL_CRITICAL);
    CHECK(temp_monitor_classify(49)   == TEMP_LEVEL_CRITICAL);  /*  4.9 */
    CHECK(temp_monitor_classify(50)   == TEMP_LEVEL_NORMAL);    /*  5.0 */
    CHECK(temp_monitor_classify(849)  == TEMP_LEVEL_NORMAL);    /* 84.9 */
    CHECK(temp_monitor_classify(850)  == TEMP_LEVEL_WARNING);   /* 85.0 */
    CHECK(temp_monitor_classify(1049) == TEMP_LEVEL_WARNING);   /*104.9 */
    CHECK(temp_monitor_classify(1050) == TEMP_LEVEL_CRITICAL);  /*105.0 */
    CHECK(temp_monitor_classify(-100) == TEMP_LEVEL_CRITICAL);
}

static void test_hysteresis(void) {
    temp_monitor_t m;
    temp_monitor_init(&m, 10);

    CHECK(temp_monitor_update(&m, 849)  == TEMP_LEVEL_NORMAL);
    CHECK(temp_monitor_update(&m, 850)  == TEMP_LEVEL_WARNING);  /* up: immediate */
    CHECK(temp_monitor_update(&m, 845)  == TEMP_LEVEL_WARNING);  /* in band: hold */
    CHECK(temp_monitor_update(&m, 840)  == TEMP_LEVEL_WARNING);
    CHECK(temp_monitor_update(&m, 839)  == TEMP_LEVEL_NORMAL);   /* left band */
    CHECK(temp_monitor_update(&m, 1100) == TEMP_LEVEL_CRITICAL); /* jump 2 levels */
    CHECK(temp_monitor_update(&m, 1040) == TEMP_LEVEL_CRITICAL);
    CHECK(temp_monitor_update(&m, 700)  == TEMP_LEVEL_NORMAL);   /* big drop: 2 levels */
    CHECK(temp_monitor_update(&m, 49)   == TEMP_LEVEL_CRITICAL); /* cold */
    CHECK(temp_monitor_update(&m, 59)   == TEMP_LEVEL_CRITICAL);
    CHECK(temp_monitor_update(&m, 60)   == TEMP_LEVEL_NORMAL);

    temp_monitor_init(&m, 0);                                    /* disabled */
    CHECK(temp_monitor_update(&m, 850)  == TEMP_LEVEL_WARNING);
    CHECK(temp_monitor_update(&m, 849)  == TEMP_LEVEL_NORMAL);
}

static void test_eeprom_config(void) {
    device_config_t cfg;
    uint8_t bad_serial[2] = { 'A', 0x01 };

    mock_hal_reset();
    mock_eeprom_set_config(1u, "ABC1234");
    CHECK(eeprom_config_load(&cfg) == CONFIG_OK);
    CHECK(cfg.revision == HW_REV_B);
    CHECK(strcmp(cfg.serial, "ABC1234") == 0);

    mock_hal_reset();                                  /* blank = 0xFF */
    CHECK(eeprom_config_load(&cfg) == CONFIG_ERR_REVISION);

    mock_hal_reset();
    mock_eeprom_set_config(2u, "ABC1234");
    CHECK(eeprom_config_load(&cfg) == CONFIG_ERR_REVISION);

    mock_hal_reset();
    mock_eeprom_set_config(0u, "");
    CHECK(eeprom_config_load(&cfg) == CONFIG_ERR_SERIAL);

    mock_hal_reset();
    mock_eeprom_set_config(0u, "X");
    mock_eeprom_write(0x0001u, bad_serial, sizeof bad_serial);
    CHECK(eeprom_config_load(&cfg) == CONFIG_ERR_SERIAL);

    mock_hal_reset();
    mock_eeprom_set_config(0u, "ABC1234");
    mock_i2c_set_fail(true);
    CHECK(eeprom_config_load(&cfg) == CONFIG_ERR_I2C);

    mock_hal_reset();                                  /* 16 chars, no NUL */
    mock_eeprom_set_config(0u, "0123456789ABCDEF");
    CHECK(eeprom_config_load(&cfg) == CONFIG_OK);
    CHECK(strlen(cfg.serial) == 16u);
    CHECK(eeprom_config_load(NULL) == CONFIG_ERR_PARAM);
}

static void test_acquisition_handoff(void) {
    temp_sample_t s;

    mock_hal_reset();
    temp_acq_init();
    CHECK(!temp_acq_get_latest(&s));          /* nothing yet */

    temp_acq_isr_push(11);
    temp_acq_isr_push(22);
    temp_acq_isr_push(33);
    CHECK(temp_acq_get_latest(&s));
    CHECK(s.raw == 33 && s.seq == 3 && s.missed == 2);
    CHECK(!temp_acq_get_latest(&s));          /* consumed */
}

static void test_app_rev_a(void) {
    mock_hal_reset();
    mock_eeprom_set_config(0u, "ABC1234");
    CHECK(app_init() == APP_OK);
    CHECK(mock_timer_running());
    CHECK(mock_timer_period_us() == 100u);
    CHECK(leds_are(false, false, false));     /* nothing sampled yet */

    mock_adc_convert(25);  app_run_once(); CHECK(leds_are(false, false, true));
    mock_adc_convert(85);  app_run_once(); CHECK(leds_are(false, true,  false));
    mock_adc_convert(105); app_run_once(); CHECK(leds_are(true,  false, false));
    mock_adc_convert(4);   app_run_once(); CHECK(leds_are(true,  false, false));
}

static void test_app_rev_b(void) {
    app_info_t info;

    mock_hal_reset();
    mock_eeprom_set_config(1u, "ABC1234");
    CHECK(app_init() == APP_OK);

    mock_adc_convert(250);  app_run_once(); CHECK(leds_are(false, false, true));
    mock_adc_convert(850);  app_run_once(); CHECK(leds_are(false, true,  false));
    mock_adc_convert(1050); app_run_once(); CHECK(leds_are(true,  false, false));

    /* Same raw value, other revision -> other meaning: 105 = 10.5 C */
    mock_adc_convert(105);  app_run_once(); CHECK(leds_are(false, false, true));

    app_get_info(&info);
    CHECK(info.last_temp_dC == 105);
    CHECK(info.samples_processed == 4u);
}

static void test_app_fault(void) {
    app_info_t info;

    mock_hal_reset();                         /* blank EEPROM */
    CHECK(app_init() == APP_ERR_CONFIG);
    CHECK(leds_are(true, true, false));
    CHECK(!mock_timer_running());

    mock_adc_convert(25); app_run_once();     /* ignored while faulted */
    CHECK(leds_are(true, true, false));
    app_get_info(&info);
    CHECK(info.fault && info.samples_processed == 0u);
}

int main(void) {
    test_sensor_conversion();
    test_classification_boundaries();
    test_hysteresis();
    test_eeprom_config();
    test_acquisition_handoff();
    test_app_rev_a();
    test_app_rev_b();
    test_app_fault();

    printf("%d checks, %d failed\n", s_run, s_failed);
    return (s_failed == 0) ? 0 : 1;
}
