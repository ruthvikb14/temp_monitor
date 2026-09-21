/**
 * @file TempAcquisition.hpp
 * @brief 100 us sampling and the ISR -> main-loop hand-off.
 *
 * The timer triggers the ADC in hardware; the end-of-conversion ISR only
 * stores the raw value and bumps a sequence counter. Conversion and
 * decisions run in the main loop.
 */
#pragma once

#include <cstdint>
#include "drivers/IrqEvents.hpp"
#include "hal/IAdc.hpp"
#include "hal/IInterruptControl.hpp"
#include "hal/ISampleTimer.hpp"

namespace tempmon {

struct Sample {
    std::uint16_t raw = 0U;
    std::uint32_t seq = 0U;
    std::uint32_t missed = 0U;   ///< samples overwritten since previous fetch
};

class TempAcquisition final : public IAdcEventHandler, public ITimerEventHandler {
public:
    TempAcquisition(hal::ISampleTimer &timer, hal::IAdc &adc, hal::IInterruptControl &irq)
        : timer_{timer}, adc_{adc}, irq_{irq} {}

    void init();
    void start();
    void stop();

    /// Main-loop context. @return true if a new sample arrived since last call.
    bool fetchLatest(Sample &out);

    // ISR context
    void onConversionComplete() override;
    void onTimerUpdate() override;

private:
    hal::ISampleTimer &timer_;
    hal::IAdc &adc_;
    hal::IInterruptControl &irq_;

    volatile std::uint16_t latestRaw_ = 0U;   // written by ISR
    volatile std::uint32_t seq_ = 0U;         // written by ISR
    std::uint32_t lastReadSeq_ = 0U;          // main loop only
};

} // namespace tempmon
