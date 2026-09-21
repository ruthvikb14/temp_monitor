/**
 * @file mock_hal.h
 * @brief Control/inspection API of the PC mock HAL (demo + tests only).
 */
#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "board.h"

#define MOCK_EEPROM_SIZE 256u

/** Blank EEPROM (0xFF), LEDs off, no I2C error, timer stopped. */
void mock_hal_reset(void);

void mock_eeprom_write(uint16_t addr, const void *data, size_t len);
/** Convenience: program revision byte + serial string. */
void mock_eeprom_set_config(uint8_t revision, const char *serial);
void mock_i2c_set_fail(bool fail);

/** Value the next hal_adc_read_result() returns. */
void mock_adc_set_raw(uint16_t raw);
/** Simulate one conversion: set the value and "fire" ADC_IRQHandler. */
void mock_adc_convert(uint16_t raw);

bool     mock_gpio_get(board_pin_t pin);
bool     mock_gpio_is_output(board_pin_t pin);
bool     mock_timer_running(void);
uint32_t mock_timer_period_us(void);

#endif /* MOCK_HAL_H */
