/**
 * @file Isr.hpp
 * @brief Interrupt service routines and their binding to C++ objects.
 *
 * The vector table needs plain C functions (no `this`), so each ISR
 * forwards to a handler object bound at init time, before the interrupt
 * is enabled.
 */
#pragma once

#include "drivers/IrqEvents.hpp"

extern "C" {
void ADC_IRQHandler(void);
void TIM_IRQHandler(void);
}

namespace tempmon::isr {

void bind(IAdcEventHandler *adc, ITimerEventHandler *timer);

void unbindAdc(const IAdcEventHandler *adc);
void unbindTimer(const ITimerEventHandler *timer);

/// Unbind if (and only if) @p owner is currently bound.
template <typename Owner>
void unbind(Owner *owner) {
    unbindAdc(owner);
    unbindTimer(owner);
}

} // namespace tempmon::isr
