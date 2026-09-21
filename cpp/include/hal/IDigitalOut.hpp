#pragma once

namespace tempmon::hal {

/// One GPIO output pin. One instance per LED.
class IDigitalOut {
public:
    virtual void configureOutput() = 0;
    /// @param on true = LED lit (implementation handles active-low wiring)
    virtual void write(bool on) = 0;

protected:
    ~IDigitalOut() = default;
};

} // namespace tempmon::hal
