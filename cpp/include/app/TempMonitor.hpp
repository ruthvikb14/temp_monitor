/**
 * @file TempMonitor.hpp
 * @brief Temperature classification with hysteresis. Pure logic, no HW.
 *
 *   Normal   (G)  5.0 <= T <  85.0 degC
 *   Warning  (Y) 85.0 <= T < 105.0 degC
 *   Critical (R)         T >= 105.0 degC  or  T < 5.0 degC
 *
 * Escalation is immediate; de-escalation requires the temperature to move
 * back by the hysteresis margin (prevents LED flicker at a threshold).
 */
#pragma once

#include "common/Types.hpp"

namespace tempmon {

class TempMonitor {
public:
    static constexpr Temperature kWarnHigh = Temperature::fromDeciCelsius(850);
    static constexpr Temperature kCritHigh = Temperature::fromDeciCelsius(1050);
    static constexpr Temperature kCritLow  = Temperature::fromDeciCelsius(50);

    explicit TempMonitor(Temperature hysteresis = Temperature::fromDeciCelsius(10));

    /// Plain requirement, no hysteresis.
    static TempLevel classify(Temperature t) { return classifyWithMargin(t, Temperature{}); }

    /// Feed one temperature, returns the (possibly unchanged) level.
    TempLevel update(Temperature t);

    TempLevel level() const { return level_; }
    void reset();

private:
    static TempLevel classifyWithMargin(Temperature t, Temperature margin);

    Temperature hysteresis_;
    TempLevel level_ = TempLevel::Normal;
    bool hasLevel_ = false;
};

} // namespace tempmon
