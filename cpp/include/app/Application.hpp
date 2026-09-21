/**
 * @file Application.hpp
 * @brief Top-level composition: owns the drivers, wires them to the HAL.
 *
 * All HAL objects are injected by reference, so the same Application runs
 * on the target (register-level HAL) and on the PC (mock HAL).
 */
#pragma once

#include <cstdint>
#include "app/TempMonitor.hpp"
#include "drivers/ConfigReader.hpp"
#include "drivers/ITemperatureSensor.hpp"
#include "drivers/LedIndicator.hpp"
#include "drivers/TempAcquisition.hpp"
#include "hal/IAdc.hpp"
#include "hal/IDigitalOut.hpp"
#include "hal/II2cBus.hpp"
#include "hal/IInterruptControl.hpp"
#include "hal/ISampleTimer.hpp"

namespace tempmon {

/// Every hardware dependency of the application, in one place.
struct HalBundle {
    hal::ISampleTimer &timer;
    hal::IAdc &adc;
    hal::II2cBus &i2c;
    hal::IInterruptControl &irq;
    hal::IDigitalOut &ledRed;
    hal::IDigitalOut &ledYellow;
    hal::IDigitalOut &ledGreen;
};

enum class AppStatus : std::uint8_t {
    Ok,
    ConfigError,    ///< EEPROM unreadable / invalid -> fault indication
    SensorError     ///< no sensor implementation for this revision
};

struct AppInfo {
    bool fault = false;
    ConfigStatus configStatus = ConfigStatus::Ok;
    DeviceConfig config{};
    const char *sensorName = "";
    std::uint16_t lastRaw = 0U;
    Temperature lastTemp{};
    TempLevel level = TempLevel::Normal;
    std::uint32_t samplesProcessed = 0U;
    std::uint32_t samplesMissed = 0U;
};

class Application {
public:
    explicit Application(const HalBundle &hal);
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    /// Load config, select sensor, start sampling. Call once after reset.
    AppStatus init();

    /// One main-loop iteration: process the newest sample if there is one.
    void runOnce();

    const AppInfo &info() const { return info_; }

private:
    void enterFault();

    hal::II2cBus &i2c_;
    ConfigReader config_;
    LedIndicator leds_;
    TempAcquisition acquisition_;
    TempMonitor monitor_;

    const ITemperatureSensor *sensor_ = nullptr;
    AppInfo info_{};
    TempLevel shownLevel_ = TempLevel::Normal;
    bool ledValid_ = false;
};

} // namespace tempmon
