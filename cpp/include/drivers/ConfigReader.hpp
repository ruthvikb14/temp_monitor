/**
 * @file ConfigReader.hpp
 * @brief Device configuration stored in the I2C EEPROM.
 *
 * EEPROM layout:
 *   0x0000        1 byte    hardware revision (0 = Rev-A, 1 = Rev-B)
 *   0x0001..0x10  16 bytes  serial number, ASCII, NUL-padded
 */
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "Board.hpp"
#include "common/Types.hpp"
#include "hal/II2cBus.hpp"

namespace tempmon {

struct DeviceConfig {
    static constexpr std::size_t kSerialMaxLen = 16U;

    HwRevision revision = HwRevision::RevA;
    std::array<char, kSerialMaxLen + 1U> serial{};   ///< always NUL-terminated

    const char *serialStr() const { return serial.data(); }
};

enum class ConfigStatus : std::uint8_t {
    Ok,
    I2cError,           ///< EEPROM not responding
    InvalidRevision,    ///< unknown revision (e.g. blank 0xFF)
    InvalidSerial       ///< empty or non-printable serial
};

class ConfigReader {
public:
    static constexpr std::uint16_t kAddrRevision = 0x0000U;
    static constexpr std::uint16_t kAddrSerial   = 0x0001U;

    explicit ConfigReader(hal::II2cBus &i2c, std::uint8_t devAddr = board::kEepromI2cAddr)
        : i2c_{i2c}, devAddr_{devAddr} {}

    /// Read and validate. @p out is only written on success.
    ConfigStatus load(DeviceConfig &out) const;

private:
    hal::II2cBus &i2c_;
    std::uint8_t devAddr_;
};

} // namespace tempmon
