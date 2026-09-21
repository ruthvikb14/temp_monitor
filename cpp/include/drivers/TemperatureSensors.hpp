#pragma once

#include "drivers/ITemperatureSensor.hpp"

namespace tempmon {

/// Rev-A: 1 digit = 1.0 degC (raw 10 -> 10.0 degC)
class RevASensor final : public ITemperatureSensor {
public:
    const char *name() const override { return "Rev-A (1.0 degC/digit)"; }
    Temperature convert(std::uint16_t raw) const override
    {
        return Temperature::fromCelsius(raw);
    }
};

/// Rev-B: 1 digit = 0.1 degC (raw 100 -> 10.0 degC)
class RevBSensor final : public ITemperatureSensor {
public:
    const char *name() const override { return "Rev-B (0.1 degC/digit)"; }
    Temperature convert(std::uint16_t raw) const override
    {
        return Temperature::fromDeciCelsius(raw);
    }
};

/**
 * Selects the sensor implementation for a revision. Returns statically
 * allocated instances: no heap. Adding Rev-C = one class + one case.
 */
class SensorFactory {
public:
    /// @return sensor for @p rev, or nullptr if unsupported.
    static const ITemperatureSensor *forRevision(HwRevision rev);
};

} // namespace tempmon
