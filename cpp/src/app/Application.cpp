#include "app/Application.hpp"

#include "Isr.hpp"
#include "drivers/TemperatureSensors.hpp"

namespace tempmon {

Application::Application(const HalBundle &hal)
    : i2c_{hal.i2c},
      config_{hal.i2c},
      leds_{hal.ledRed, hal.ledYellow, hal.ledGreen},
      acquisition_{hal.timer, hal.adc, hal.irq},
      monitor_{Temperature::fromDeciCelsius(10)}    // 1.0 degC hysteresis
{}

Application::~Application() {
    acquisition_.stop();
    isr::unbind(&acquisition_);     // never leave the ISR with a dangling pointer
}

AppStatus Application::init() {
    info_ = AppInfo{};
    sensor_ = nullptr;
    ledValid_ = false;
    monitor_.reset();

    // LEDs first: defined all-off state as early as possible.
    leds_.init();

    (void)i2c_.init();
    info_.configStatus = config_.load(info_.config);
    if (info_.configStatus != ConfigStatus::Ok) {
        enterFault();
        return AppStatus::ConfigError;
    }

    sensor_ = SensorFactory::forRevision(info_.config.revision);
    if (sensor_ == nullptr) {
        enterFault();
        return AppStatus::SensorError;
    }
    info_.sensorName = sensor_->name();

    // Sampling starts last, when every consumer of samples is ready.
    isr::bind(&acquisition_, &acquisition_);
    acquisition_.init();
    acquisition_.start();
    return AppStatus::Ok;
}

void Application::runOnce() {
    if (info_.fault) {
        return;
    }

    Sample sample;
    if (!acquisition_.fetchLatest(sample)) {
        return;
    }

    info_.lastRaw = sample.raw;
    info_.lastTemp = sensor_->convert(sample.raw);
    info_.samplesProcessed++;
    info_.samplesMissed += sample.missed;
    info_.level = monitor_.update(info_.lastTemp);

    // Touch the GPIOs only when the level changes.
    if (!ledValid_ || (info_.level != shownLevel_)) {
        leds_.show(info_.level);
        shownLevel_ = info_.level;
        ledValid_ = true;
    }
}

void Application::enterFault() {
    info_.fault = true;
    acquisition_.stop();
    leds_.showFault();
}

} // namespace tempmon
