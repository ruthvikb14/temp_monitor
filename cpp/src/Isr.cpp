#include "Isr.hpp"

namespace {
// Written in init (before IRQs are enabled), read in ISR context
tempmon::IAdcEventHandler *volatile g_adcHandler = nullptr;
tempmon::ITimerEventHandler *volatile g_timerHandler = nullptr;
} // namespace

namespace tempmon::isr {

void bind(IAdcEventHandler *adc, ITimerEventHandler *timer) {
    g_adcHandler = adc;
    g_timerHandler = timer;
}

void unbindAdc(const IAdcEventHandler *adc) {
    if (g_adcHandler == adc) {
        g_adcHandler = nullptr;
    }
}

void unbindTimer(const ITimerEventHandler *timer) {
    if (g_timerHandler == timer) {
        g_timerHandler = nullptr;
    }
}

} // namespace tempmon::isr

extern "C" void ADC_IRQHandler(void) {
    tempmon::IAdcEventHandler *h = g_adcHandler;
    if (h != nullptr) {
        h->onConversionComplete();
    }
}

extern "C" void TIM_IRQHandler(void) {
    tempmon::ITimerEventHandler *h = g_timerHandler;
    if (h != nullptr) {
        h->onTimerUpdate();
    }
}
