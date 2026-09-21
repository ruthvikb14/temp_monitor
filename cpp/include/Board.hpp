/**
 * @file Board.hpp
 * @brief Board-level constants.
 */
#pragma once

#include <cstdint>

namespace tempmon::board {

/// 7-bit I2C address of the configuration EEPROM.
constexpr std::uint8_t  kEepromI2cAddr  = 0x50U;

/// Temperature sampling period.
constexpr std::uint32_t kSamplePeriodUs = 100U;

/// true:  timer TRGO starts the ADC in hardware (preferred, lowest jitter).
/// false: timer ISR starts the ADC in software (fallback).
constexpr bool kAdcHwTrigger = true;

} // namespace tempmon::board
