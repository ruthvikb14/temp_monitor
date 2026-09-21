#pragma once

#include <cstddef>
#include <cstdint>

namespace tempmon::hal {

enum class Status : std::uint8_t { Ok, Error };

class II2cBus {
public:
    virtual Status init() = 0;
    /// Blocking random read from a memory-addressed device (init only).
    virtual Status memRead(std::uint8_t devAddr, std::uint16_t memAddr,
                           std::uint8_t *buf, std::size_t len) = 0;

protected:
    ~II2cBus() = default;
};

} // namespace tempmon::hal
