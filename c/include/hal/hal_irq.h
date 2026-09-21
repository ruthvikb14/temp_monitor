/**
 * @file hal_irq.h
 * @brief Critical sections for data shared between ISR and main loop.
 */
#ifndef HAL_IRQ_H
#define HAL_IRQ_H

#include <stdint.h>

/** Disable interrupts, return previous state */
uint32_t hal_irq_save_disable(void);
/** Restore the state returned by hal_irq_save_disable(). */
void hal_irq_restore(uint32_t state);

#endif /* HAL_IRQ_H */
