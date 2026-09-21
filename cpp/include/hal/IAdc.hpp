#pragma once

#include <cstdint>

namespace tempmon::hal {

/// ADC channel connected to the temperature sensor.
class IAdc {
public:
    virtual void init(bool hwTrigger) = 0;
    virtual void enableIrq() = 0;
    virtual void startConversion() = 0;          ///< software trigger mode
    virtual std::uint16_t readResult() = 0;      ///< also clears EOC flag

protected:
    ~IAdc() = default;
};

} // namespace tempmon::hal
