/**
 * @file IrqEvents.hpp
 * @brief Interfaces through which the C-linkage ISRs reach C++ objects.
 */
#pragma once

namespace tempmon {

class IAdcEventHandler {
public:
    virtual void onConversionComplete() = 0;   ///< called in ISR context
protected:
    ~IAdcEventHandler() = default;
};

class ITimerEventHandler {
public:
    virtual void onTimerUpdate() = 0;          ///< called in ISR context
protected:
    ~ITimerEventHandler() = default;
};

} // namespace tempmon
