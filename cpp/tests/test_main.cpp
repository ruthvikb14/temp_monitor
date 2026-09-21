/**
 * @brief Unit + integration tests against the mock HAL. No framework needed.
 */
#include <cstdio>
#include <cstring>
#include "MockHal.hpp"
#include "drivers/TemperatureSensors.hpp"

using namespace tempmon;

namespace {

int g_run = 0;
int g_failed = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        ++g_run;                                                               \
        if (!(cond)) {                                                         \
            ++g_failed;                                                        \
            std::printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                      \
    } while (0)

constexpr Temperature dC(std::int32_t v) { return Temperature::fromDeciCelsius(v); }

void testTemperatureType() {
    static_assert(Temperature::fromCelsius(85) == Temperature::fromDeciCelsius(850), "");
    static_assert(dC(10) + dC(5) == dC(15), "");
    CHECK(dC(849) < TempMonitor::kWarnHigh);
    CHECK(Temperature{}.deciCelsius() == 0);
}

void testSensors() {
    const ITemperatureSensor *a = SensorFactory::forRevision(HwRevision::RevA);
    const ITemperatureSensor *b = SensorFactory::forRevision(HwRevision::RevB);

    CHECK(a != nullptr && b != nullptr);
    CHECK(a->convert(10) == dC(100));       // spec example: 10 -> 10 C
    CHECK(b->convert(100) == dC(100));      // spec example: 100 -> 10 C
    CHECK(a->convert(105) == dC(1050));
    CHECK(b->convert(1050) == dC(1050));
    CHECK(SensorFactory::forRevision(static_cast<HwRevision>(5)) == nullptr);
}

void testClassificationBoundaries() {
    CHECK(TempMonitor::classify(dC(0))    == TempLevel::Critical);
    CHECK(TempMonitor::classify(dC(49))   == TempLevel::Critical);   //  4.9
    CHECK(TempMonitor::classify(dC(50))   == TempLevel::Normal);     //  5.0
    CHECK(TempMonitor::classify(dC(849))  == TempLevel::Normal);     // 84.9
    CHECK(TempMonitor::classify(dC(850))  == TempLevel::Warning);    // 85.0
    CHECK(TempMonitor::classify(dC(1049)) == TempLevel::Warning);    //104.9
    CHECK(TempMonitor::classify(dC(1050)) == TempLevel::Critical);   //105.0
    CHECK(TempMonitor::classify(dC(-100)) == TempLevel::Critical);
}

void testHysteresis() {
    TempMonitor m{dC(10)};
    CHECK(m.update(dC(849))  == TempLevel::Normal);
    CHECK(m.update(dC(850))  == TempLevel::Warning);    // up: immediate
    CHECK(m.update(dC(845))  == TempLevel::Warning);    // in band: hold
    CHECK(m.update(dC(840))  == TempLevel::Warning);
    CHECK(m.update(dC(839))  == TempLevel::Normal);     // left band
    CHECK(m.update(dC(1100)) == TempLevel::Critical);   // jump 2 levels
    CHECK(m.update(dC(1040)) == TempLevel::Critical);
    CHECK(m.update(dC(700))  == TempLevel::Normal);     // big drop: 2 levels
    CHECK(m.update(dC(49))   == TempLevel::Critical);   // cold
    CHECK(m.update(dC(59))   == TempLevel::Critical);
    CHECK(m.update(dC(60))   == TempLevel::Normal);

    TempMonitor off{dC(0)};
    CHECK(off.update(dC(850)) == TempLevel::Warning);
    CHECK(off.update(dC(849)) == TempLevel::Normal);
}

void testConfigReader() {
    mock::MockEeprom ee;
    ConfigReader reader{ee};
    DeviceConfig cfg;

    ee.setConfig(1U, "ABC1234");
    CHECK(reader.load(cfg) == ConfigStatus::Ok);
    CHECK(cfg.revision == HwRevision::RevB);
    CHECK(std::strcmp(cfg.serialStr(), "ABC1234") == 0);

    ee.erase();                                         // blank = 0xFF
    CHECK(reader.load(cfg) == ConfigStatus::InvalidRevision);

    ee.setConfig(2U, "ABC1234");
    CHECK(reader.load(cfg) == ConfigStatus::InvalidRevision);

    ee.setConfig(0U, "");
    CHECK(reader.load(cfg) == ConfigStatus::InvalidSerial);

    const std::uint8_t badSerial[] = {'A', 0x01U};
    ee.setConfig(0U, "X");
    ee.write(0x0001U, badSerial, sizeof badSerial);
    CHECK(reader.load(cfg) == ConfigStatus::InvalidSerial);

    ee.setConfig(0U, "ABC1234");
    ee.fail = true;
    CHECK(reader.load(cfg) == ConfigStatus::I2cError);
    ee.fail = false;

    ee.setConfig(0U, "0123456789ABCDEF");               // 16 chars, no NUL
    CHECK(reader.load(cfg) == ConfigStatus::Ok);
    CHECK(std::strlen(cfg.serialStr()) == 16U);

    ConfigReader wrongAddr{ee, 0x51U};
    CHECK(wrongAddr.load(cfg) == ConfigStatus::I2cError);
}

void testAcquisitionHandoff() {
    mock::MockBoard b;
    TempAcquisition acq{b.timer, b.adc, b.irq};
    Sample s;

    acq.init();
    acq.start();
    CHECK(b.timer.running && b.timer.periodUs == 100U && b.timer.triggerAdc);
    CHECK(b.adc.irqEnabled && b.adc.hwTrigger);
    CHECK(!acq.fetchLatest(s));

    b.adc.raw = 11U; acq.onConversionComplete();
    b.adc.raw = 22U; acq.onConversionComplete();
    b.adc.raw = 33U; acq.onConversionComplete();
    CHECK(acq.fetchLatest(s));
    CHECK(s.raw == 33U && s.seq == 3U && s.missed == 2U);
    CHECK(!acq.fetchLatest(s));
    CHECK(b.irq.lockCount == 3U && !b.irq.disabled);
}

void testAppRevA() {
    mock::MockBoard b;
    b.eeprom.setConfig(0U, "ABC1234");
    Application app{b.hal()};

    CHECK(app.init() == AppStatus::Ok);
    CHECK(b.red.isOutput && b.yellow.isOutput && b.green.isOutput);
    CHECK(b.ledsAre(false, false, false));

    b.adc.convert(25U);  app.runOnce(); CHECK(b.ledsAre(false, false, true));
    b.adc.convert(85U);  app.runOnce(); CHECK(b.ledsAre(false, true,  false));
    b.adc.convert(105U); app.runOnce(); CHECK(b.ledsAre(true,  false, false));
    b.adc.convert(4U);   app.runOnce(); CHECK(b.ledsAre(true,  false, false));
}

void testAppRevB() {
    mock::MockBoard b;
    b.eeprom.setConfig(1U, "ABC1234");
    Application app{b.hal()};
    CHECK(app.init() == AppStatus::Ok);

    b.adc.convert(250U);  app.runOnce(); CHECK(b.ledsAre(false, false, true));
    b.adc.convert(850U);  app.runOnce(); CHECK(b.ledsAre(false, true,  false));
    b.adc.convert(1050U); app.runOnce(); CHECK(b.ledsAre(true,  false, false));

    // Same raw value, other revision -> other meaning: 105 = 10.5 C
    b.adc.convert(105U);  app.runOnce(); CHECK(b.ledsAre(false, false, true));

    // LEDs are only rewritten when the level changes
    const std::uint32_t writes = b.green.writes;
    b.adc.convert(200U); app.runOnce();
    b.adc.convert(300U); app.runOnce();
    CHECK(b.green.writes == writes);

    CHECK(app.info().lastTemp == dC(300));
    CHECK(app.info().samplesProcessed == 6U);
    CHECK(std::strcmp(app.info().config.serialStr(), "ABC1234") == 0);
}

void testAppFault() {
    mock::MockBoard b;                                  // blank EEPROM
    Application app{b.hal()};

    CHECK(app.init() == AppStatus::ConfigError);
    CHECK(b.ledsAre(true, true, false));
    CHECK(!b.timer.running);

    b.adc.convert(25U); app.runOnce();                  // ignored
    CHECK(b.ledsAre(true, true, false));
    CHECK(app.info().fault && app.info().samplesProcessed == 0U);
}

void testIsrUnbindOnDestruction() {
    mock::MockBoard b;
    b.eeprom.setConfig(1U, "ABC1234");
    {
        Application app{b.hal()};
        CHECK(app.init() == AppStatus::Ok);
    }
    b.adc.convert(500U);    // must not call into the destroyed object
    CHECK(true);
}

} // namespace

int main() {
    testTemperatureType();
    testSensors();
    testClassificationBoundaries();
    testHysteresis();
    testConfigReader();
    testAcquisitionHandoff();
    testAppRevA();
    testAppRevB();
    testAppFault();
    testIsrUnbindOnDestruction();

    std::printf("%d checks, %d failed\n", g_run, g_failed);
    return (g_failed == 0) ? 0 : 1;
}
