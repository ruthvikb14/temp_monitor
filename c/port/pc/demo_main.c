/**
 * @file demo_main.c
 * @brief PC demonstration: runs the same temperature profile on both
 *        hardware revisions, plus a corrupted-EEPROM case.
 */
#include <stdio.h>
#include "app/app.h"
#include "mock_hal.h"

/* Temperature profile in deci-Celsius, converted to each sensor's raw unit. */
static const temp_dC_t k_profile_dC[] = {
    250,  849,  850,  950, 1049, 1050,  /* heating through all thresholds */
    1045, 1039,                         /* hysteresis: stays red, then yellow */
    845,  839,                          /* stays yellow, then green */
    60,   49,   55,   61,   250         /* cold side */
};

static const char *level_name(temp_level_t l) {
    switch (l) {
    case TEMP_LEVEL_NORMAL:   return "NORMAL";
    case TEMP_LEVEL_WARNING:  return "WARNING";
    case TEMP_LEVEL_CRITICAL: return "CRITICAL";
    default:                  return "?";
    }
}

static void print_leds(void) {
    printf("[%c %c %c]",
           mock_gpio_get(BOARD_PIN_LED_RED)    ? 'R' : '.',
           mock_gpio_get(BOARD_PIN_LED_YELLOW) ? 'Y' : '.',
           mock_gpio_get(BOARD_PIN_LED_GREEN)  ? 'G' : '.');
}

static void run(uint8_t revision, const char *serial) {
    app_info_t info;
    app_status_t st;

    mock_hal_reset();
    mock_eeprom_set_config(revision, serial);
    st = app_init();
    app_get_info(&info);

    printf("\n=== EEPROM rev=%u serial=%s ===\n", revision, serial);
    if (st != APP_OK) {
        printf("init failed (app=%d, config=%d) -> fault LEDs ",
               (int)st, (int)info.config_status);
        print_leds();
        printf("\n");
        return;
    }
    printf("sensor: %s, sampling every %u us\n",
           info.sensor_name, (unsigned)mock_timer_period_us());
    printf("  raw   temp[C]  level      LEDs\n");

    for (size_t i = 0; i < sizeof k_profile_dC / sizeof k_profile_dC[0]; i++) {
        /* Rev-A can only represent whole degrees (truncates). */
        uint16_t raw = (uint16_t)((revision == 0u) ? k_profile_dC[i] / 10
                                                   : k_profile_dC[i]);
        mock_adc_convert(raw);   /* timer -> ADC -> ADC_IRQHandler */
        app_run_once();          /* main loop iteration            */

        app_get_info(&info);
        printf("%5u  %4ld.%ld   %-9s  ", (unsigned)info.last_raw,
               (long)(info.last_temp_dC / 10), (long)(info.last_temp_dC % 10),
               level_name(info.level));
        print_leds();
        printf("\n");
    }
}

int main(void) {
    run(0u, "ABC1234");     /* Rev-A */
    run(1u, "ABC1234");     /* Rev-B */
    run(7u, "ABC1234");     /* invalid revision in EEPROM */
    return 0;
}
