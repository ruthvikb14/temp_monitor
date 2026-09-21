#include "drivers/led_indicator.h"

#include "board.h"
#include "hal/hal_gpio.h"

void led_indicator_init(void) {
    for (int pin = 0; pin < (int)BOARD_PIN_COUNT; pin++) {
        hal_gpio_init_output((board_pin_t)pin);
        hal_gpio_write((board_pin_t)pin, false);
    }
}

void led_indicator_show(temp_level_t level) {
    hal_gpio_write(BOARD_PIN_LED_GREEN,  level == TEMP_LEVEL_NORMAL);
    hal_gpio_write(BOARD_PIN_LED_YELLOW, level == TEMP_LEVEL_WARNING);
    hal_gpio_write(BOARD_PIN_LED_RED,    level == TEMP_LEVEL_CRITICAL);
}

void led_indicator_show_fault(void) {
    hal_gpio_write(BOARD_PIN_LED_GREEN,  false);
    hal_gpio_write(BOARD_PIN_LED_YELLOW, true);
    hal_gpio_write(BOARD_PIN_LED_RED,    true);
}
