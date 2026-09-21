#pragma once

#include <cstdint>
#include "common/Types.hpp"

namespace tempmon {

/**
 * Temperature sensor abstraction. Each hardware revision provides one
 * implementation; the rest of the software only sees Temperature.
 */
class ITemperatureSensor {
public:
    virtual const char *name() const = 0;
    virtual Temperature convert(std::uint16_t raw) const = 0;

protected:
    ~ITemperatureSensor() = default;
};

} // namespace tempmon
