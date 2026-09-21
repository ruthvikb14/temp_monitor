/**
 * @file demo_main.cpp
 * @brief PC demonstration: same temperature profile on both hardware
 *        revisions, plus a corrupted-EEPROM case.
 */
#include <cstdio>
#include "MockHal.hpp"

using namespace tempmon;

namespace {

// Temperature profile in deci-Celsius, converted to each sensor's raw unit.
constexpr std::int32_t kProfileDc[] = {
    250,  849,  850,  950, 1049, 1050,  // heating through all thresholds
    1045, 1039,                         // hysteresis: stays red, then yellow
    845,  839,                          // stays yellow, then green
    60,   49,   55,   61,   250         // cold side
};

const char *levelName(TempLevel l)
{
    switch (l) {
    case TempLevel::Normal:   return "NORMAL";
    case TempLevel::Warning:  return "WARNING";
    case TempLevel::Critical: return "CRITICAL";
    }
    return "?";
}

void printLeds(const mock::MockBoard &b) {
    std::printf("[%c %c %c]", b.red.on ? 'R' : '.', b.yellow.on ? 'Y' : '.', b.green.on ? 'G' : '.');
}

void run(std::uint8_t revision, const char *serial) {
    mock::MockBoard board;
    board.eeprom.setConfig(revision, serial);

    Application app{board.hal()};
    const AppStatus st = app.init();
    const AppInfo &info = app.info();

    std::printf("\n=== EEPROM rev=%u serial=%s ===\n", revision, serial);
    if (st != AppStatus::Ok) {
        std::printf("init failed (app=%d, config=%d) -> fault LEDs ",
                    static_cast<int>(st), static_cast<int>(info.configStatus));
        printLeds(board);
        std::printf("\n");
        return;
    }
    std::printf("sensor: %s, serial: %s, sampling every %u us\n", info.sensorName,
                info.config.serialStr(), static_cast<unsigned>(board.timer.periodUs));
    std::printf("  raw   temp[C]  level      LEDs\n");

    for (const std::int32_t dC : kProfileDc) {
        // Rev-A can only represent whole degrees (truncates).
        const auto raw = static_cast<std::uint16_t>((revision == 0U) ? dC / 10 : dC);
        board.adc.convert(raw);   // timer -> ADC -> ADC_IRQHandler
        app.runOnce();            // main loop iteration

        const std::int32_t t = info.lastTemp.deciCelsius();
        std::printf("%5u  %4ld.%ld   %-9s  ", static_cast<unsigned>(info.lastRaw),
                    static_cast<long>(t / 10), static_cast<long>(t % 10), levelName(info.level));
        printLeds(board);
        std::printf("\n");
    }
}

} // namespace

int main() {
    run(0U, "ABC1234");   // Rev-A
    run(1U, "ABC1234");   // Rev-B
    run(7U, "ABC1234");   // invalid revision in EEPROM
    return 0;
}
