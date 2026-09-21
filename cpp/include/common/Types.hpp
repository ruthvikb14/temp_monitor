/**
 * @file Types.hpp
 * @brief Value types shared by all layers.
 */
#pragma once

#include <cstdint>

namespace tempmon {

/** Hardware revision as stored in EEPROM. */
enum class HwRevision : std::uint8_t {
    RevA = 0,   ///< sensor resolution 1.0 degC / digit
    RevB = 1    ///< sensor resolution 0.1 degC / digit
};

/** Temperature condition, ordered by severity (higher = worse). */
enum class TempLevel : std::uint8_t {
    Normal   = 0,   ///< green
    Warning  = 1,   ///< yellow
    Critical = 2    ///< red
};

/**
 * Strongly typed temperature, stored as integer tenths of a degree.
 * Only constructible through named factories, so a raw ADC count can never
 * be mistaken for a temperature by accident. No floating point.
 */
class Temperature {
public:
    constexpr Temperature() = default;

    static constexpr Temperature fromDeciCelsius(std::int32_t dC) { return Temperature{dC}; }
    static constexpr Temperature fromCelsius(std::int32_t c)      { return Temperature{c * 10}; }

    constexpr std::int32_t deciCelsius() const { return dC_; }

    friend constexpr bool operator==(Temperature a, Temperature b) { return a.dC_ == b.dC_; }
    friend constexpr bool operator!=(Temperature a, Temperature b) { return a.dC_ != b.dC_; }
    friend constexpr bool operator<(Temperature a, Temperature b)  { return a.dC_ <  b.dC_; }
    friend constexpr bool operator<=(Temperature a, Temperature b) { return a.dC_ <= b.dC_; }
    friend constexpr bool operator>(Temperature a, Temperature b)  { return a.dC_ >  b.dC_; }
    friend constexpr bool operator>=(Temperature a, Temperature b) { return a.dC_ >= b.dC_; }
    friend constexpr Temperature operator+(Temperature a, Temperature b) { return Temperature{a.dC_ + b.dC_}; }
    friend constexpr Temperature operator-(Temperature a, Temperature b) { return Temperature{a.dC_ - b.dC_}; }

private:
    explicit constexpr Temperature(std::int32_t dC) : dC_{dC} {}
    std::int32_t dC_ = 0;
};

} // namespace tempmon
