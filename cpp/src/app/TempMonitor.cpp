#include "app/TempMonitor.hpp"

namespace tempmon {

TempMonitor::TempMonitor(Temperature hysteresis)
    : hysteresis_{(hysteresis > Temperature{}) ? hysteresis : Temperature{}}
{}

void TempMonitor::reset() {
    level_ = TempLevel::Normal;
    hasLevel_ = false;
}

// margin > 0 moves every threshold towards the safe side; used to decide
// whether the current level may be LEFT.
TempLevel TempMonitor::classifyWithMargin(Temperature t, Temperature margin) {
    if ((t >= kCritHigh - margin) || (t < kCritLow + margin)) {
        return TempLevel::Critical;
    }
    if (t >= kWarnHigh - margin) {
        return TempLevel::Warning;
    }
    return TempLevel::Normal;
}

TempLevel TempMonitor::update(Temperature t) {
    const TempLevel entry = classify(t);

    if (!hasLevel_ || (entry >= level_)) {
        level_ = entry;             // first sample, or same / more severe
        hasLevel_ = true;
    } else {
        // less severe: step down only as far as the hysteresis band allows
        const TempLevel hold = classifyWithMargin(t, hysteresis_);
        if (hold < level_) {
            level_ = (hold > entry) ? hold : entry;
        }
    }
    return level_;
}

} // namespace tempmon
