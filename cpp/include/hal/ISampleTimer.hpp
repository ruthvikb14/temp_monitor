#pragma once

#include <cstdint>

namespace tempmon::hal {

/**
 * Periodic hardware timer used as the sampling time base.
 *
 * All HAL interfaces have a protected, non-virtual destructor: objects are
 * never deleted through the interface (no heap on this target), so no
 * virtual destructor / operator delete is pulled into the binary.
 */
class ISampleTimer {
public:
    /// @param triggerAdc true: route update event to ADC trigger, no IRQ.
    virtual void configure(std::uint32_t periodUs, bool triggerAdc) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void clearIrq() = 0;

protected:
    ~ISampleTimer() = default;
};

} // namespace tempmon::hal
