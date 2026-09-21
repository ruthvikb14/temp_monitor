/**
 * @file mock_hal.c
 * @brief PC implementation of all hal_*.h interfaces.
 */
#include "mock_hal.h"

#include <string.h>
#include "hal/hal_adc.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"
#include "hal/hal_irq.h"
#include "hal/hal_timer.h"
#include "isr.h"

static uint8_t  s_eeprom[MOCK_EEPROM_SIZE];
static bool     s_i2c_fail;
static uint16_t s_adc_raw;
static bool     s_gpio[BOARD_PIN_COUNT];
static bool     s_gpio_out[BOARD_PIN_COUNT];
static bool     s_timer_running;
static uint32_t s_timer_period_us;

/* ---------- mock control ---------- */

void mock_hal_reset(void) {
    memset(s_eeprom, 0xFF, sizeof s_eeprom);
    memset(s_gpio, 0, sizeof s_gpio);
    memset(s_gpio_out, 0, sizeof s_gpio_out);
    s_i2c_fail = false;
    s_adc_raw = 0u;
    s_timer_running = false;
    s_timer_period_us = 0u;
}

void mock_eeprom_write(uint16_t addr, const void *data, size_t len) {
    if ((size_t)addr + len <= sizeof s_eeprom) {
        memcpy(&s_eeprom[addr], data, len);
    }
}

void mock_eeprom_set_config(uint8_t revision, const char *serial) {
    uint8_t buf[16];
    size_t n = strlen(serial);
    memset(buf, 0, sizeof buf);
    memcpy(buf, serial, (n < sizeof buf) ? n : sizeof buf);
    mock_eeprom_write(0x0000u, &revision, 1u);
    mock_eeprom_write(0x0001u, buf, sizeof buf);
}

void mock_i2c_set_fail(bool fail)      { s_i2c_fail = fail; }
void mock_adc_set_raw(uint16_t raw)    { s_adc_raw = raw; }

void mock_adc_convert(uint16_t raw) {
    s_adc_raw = raw;
    ADC_IRQHandler();
}

bool mock_gpio_get(board_pin_t pin)       { return s_gpio[pin]; }
bool mock_gpio_is_output(board_pin_t pin) { return s_gpio_out[pin]; }
bool mock_timer_running(void)             { return s_timer_running; }
uint32_t mock_timer_period_us(void)       { return s_timer_period_us; }

/* ---------- HAL implementation ---------- */

void hal_timer_init_periodic(uint32_t period_us, bool trigger_adc) {
    (void)trigger_adc;
    s_timer_period_us = period_us;
}
void hal_timer_start(void)     { s_timer_running = true; }
void hal_timer_stop(void)      { s_timer_running = false; }
void hal_timer_clear_irq(void) { }

void hal_adc_init(bool hw_trigger) { (void)hw_trigger; }
void hal_adc_enable_irq(void)      { }
void hal_adc_start_conversion(void){ }
uint16_t hal_adc_read_result(void) { return s_adc_raw; }

void hal_gpio_init_output(board_pin_t pin) { s_gpio_out[pin] = true; }
void hal_gpio_write(board_pin_t pin, bool on) { s_gpio[pin] = on; }

hal_status_t hal_i2c_init(void) { return HAL_OK; }

hal_status_t hal_i2c_mem_read(uint8_t dev_addr, uint16_t mem_addr, uint8_t *buf, size_t len) {
    if (s_i2c_fail || (dev_addr != BOARD_EEPROM_I2C_ADDR) ||
        ((size_t)mem_addr + len > sizeof s_eeprom)) {
        return HAL_ERROR;   /* NACK */
    }
    memcpy(buf, &s_eeprom[mem_addr], len);
    return HAL_OK;
}

uint32_t hal_irq_save_disable(void) { return 0u; }
void hal_irq_restore(uint32_t state) { (void)state; }
