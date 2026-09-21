/**
 * @file hal_timer.h
 * @brief Periodic hardware timer used as the sampling time base.
 */
#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Configure a periodic timer.
 * @param period_us    period in microseconds
 * @param trigger_adc  true: route the update event (TRGO) to the ADC start
 *                     trigger, no timer interrupt. false: enable the timer
 *                     update interrupt instead.
 */
void hal_timer_init_periodic(uint32_t period_us, bool trigger_adc);
void hal_timer_start(void);
void hal_timer_stop(void);
/** Clear the update interrupt flag (software-trigger mode only). */
void hal_timer_clear_irq(void);

#endif /* HAL_TIMER_H */
