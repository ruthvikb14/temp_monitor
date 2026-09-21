#include "drivers/TemperatureSensors.hpp"

namespace tempmon {

namespace {
const RevASensor kRevASensor{};
const RevBSensor kRevBSensor{};
} // namespace

const ITemperatureSensor *SensorFactory::forRevision(HwRevision rev) {
    switch (rev) {
    case HwRevision::RevA: return &kRevASensor;
    case HwRevision::RevB: return &kRevBSensor;
    }
    return nullptr;     // value outside the enum
}

} // namespace tempmon
