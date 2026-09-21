/**
 * @file hal_adc.h
 * @brief ADC channel connected to the temperature sensor.
 */
#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdbool.h>
#include <stdint.h>

/** Configure channel, sample time and trigger source. */
void hal_adc_init(bool hw_trigger);
/** Enable the end-of-conversion interrupt. */
void hal_adc_enable_irq(void);
/** Start one conversion by software (software-trigger mode only). */
void hal_adc_start_conversion(void);
/** Read the conversion result. Reading also clears the EOC flag. */
uint16_t hal_adc_read_result(void);

#endif /* HAL_ADC_H */
