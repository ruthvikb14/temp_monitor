#include "drivers/ConfigReader.hpp"

namespace tempmon {

namespace {
constexpr bool isPrintableAscii(std::uint8_t c) { return (c >= 0x20U) && (c <= 0x7EU); }

constexpr bool isKnownRevision(std::uint8_t rev)  {
    return (rev == static_cast<std::uint8_t>(HwRevision::RevA)) ||
           (rev == static_cast<std::uint8_t>(HwRevision::RevB));
}
} // namespace

ConfigStatus ConfigReader::load(DeviceConfig &out) const {
    std::uint8_t rev = 0U;
    if (i2c_.memRead(devAddr_, kAddrRevision, &rev, 1U) != hal::Status::Ok) {
        return ConfigStatus::I2cError;
    }
    if (!isKnownRevision(rev)) {
        return ConfigStatus::InvalidRevision;
    }

    std::array<std::uint8_t, DeviceConfig::kSerialMaxLen> raw{};
    if (i2c_.memRead(devAddr_, kAddrSerial, raw.data(), raw.size()) != hal::Status::Ok) {
        return ConfigStatus::I2cError;
    }

    std::size_t len = 0U;
    while ((len < raw.size()) && (raw[len] != 0U)) {
        if (!isPrintableAscii(raw[len])) {
            return ConfigStatus::InvalidSerial;
        }
        ++len;
    }
    if (len == 0U) {
        return ConfigStatus::InvalidSerial;
    }

    out.revision = static_cast<HwRevision>(rev);
    out.serial.fill('\0');
    for (std::size_t i = 0U; i < len; ++i) {
        out.serial[i] = static_cast<char>(raw[i]);
    }
    return ConfigStatus::Ok;
}

} // namespace tempmon
