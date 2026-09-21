#pragma once

#include <cstdint>

namespace tempmon::hal {

class IInterruptControl {
public:
    virtual std::uint32_t saveAndDisable() = 0;     ///< e.g. PRIMASK on Cortex-M
    virtual void restore(std::uint32_t state) = 0;

protected:
    ~IInterruptControl() = default;
};

/**
 * RAII critical section: interrupts are disabled for the lifetime of the
 * object and restored on every exit path.
 */
class CriticalSection {
public:
    explicit CriticalSection(IInterruptControl &irq) : irq_{irq}, state_{irq.saveAndDisable()} {}
    ~CriticalSection() { irq_.restore(state_); }

    CriticalSection(const CriticalSection &) = delete;
    CriticalSection &operator=(const CriticalSection &) = delete;

private:
    IInterruptControl &irq_;
    std::uint32_t state_;
};

} // namespace tempmon::hal
