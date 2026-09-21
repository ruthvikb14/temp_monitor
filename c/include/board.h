/**
 * @file board.h
 * @brief Board-level constants. The only place that knows about wiring.
 */
#ifndef BOARD_H
#define BOARD_H

/** Logical GPIO pins. The port layer maps these to real port/pin numbers. */
typedef enum {
    BOARD_PIN_LED_GREEN = 0,
    BOARD_PIN_LED_YELLOW,
    BOARD_PIN_LED_RED,
    BOARD_PIN_COUNT
} board_pin_t;

/** 7-bit I2C address of the configuration EEPROM */
#define BOARD_EEPROM_I2C_ADDR     0x50u

/** Temperature sampling period. */
#define BOARD_SAMPLE_PERIOD_US    100u

/**
 * 1 = timer TRGO starts the ADC in hardware
 * 0 = timer interrupt starts the ADC in software
 */
#define BOARD_ADC_HW_TRIGGER      1

#endif /* BOARD_H */
