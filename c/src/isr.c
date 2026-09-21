/**
 * @file isr.c
 * @brief Interrupt service routines.
 *
 * Names follow the CMSIS vector-table convention; on a real target the
 * startup file places them in the vector table. In the PC build they are
 * plain functions the demo may call to simulate an interrupt.
 *
 * Rule: ISRs do the minimum (read HW register, store, return). No
 * conversion, no classification, no GPIO, no I2C.
 */
#include "board.h"
#include "drivers/temp_acquisition.h"
#include "hal/hal_adc.h"
#include "hal/hal_timer.h"
#include "isr.h"

/* ADC end of conversion: fires every 100 us. */
void ADC_IRQHandler(void) {
    uint16_t raw = hal_adc_read_result();   /* also clears EOC flag */
    temp_acq_isr_push(raw);
}

/* Only used when the ADC cannot be triggered by the timer in hardware.
 * Jitter then equals the variation of this ISR's entry latency, so it must
 * have the highest priority and nothing may disable IRQs for long. */
void TIM_IRQHandler(void) {
#if !BOARD_ADC_HW_TRIGGER
    hal_timer_clear_irq();
    hal_adc_start_conversion();
#endif
}
