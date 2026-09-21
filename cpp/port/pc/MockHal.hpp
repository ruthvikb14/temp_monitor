/**
 * @file MockHal.hpp
 * @brief PC implementations of the HAL interfaces (demo + tests).
 *        Public members are the inspection / control surface.
 */
#pragma once

#include <array>
#include <cstring>
#include "Board.hpp"
#include "Isr.hpp"
#include "app/Application.hpp"

namespace tempmon::mock {

class MockTimer final : public hal::ISampleTimer {
public:
    void configure(std::uint32_t periodUs_, bool triggerAdc_) override {
        periodUs = periodUs_;
        triggerAdc = triggerAdc_;
    }
    void start() override { running = true; }
    void stop() override { running = false; }
    void clearIrq() override { ++irqClears; }

    std::uint32_t periodUs = 0U;
    bool triggerAdc = false;
    bool running = false;
    std::uint32_t irqClears = 0U;
};

class MockAdc final : public hal::IAdc {
public:
    void init(bool hwTrigger_) override { hwTrigger = hwTrigger_; }
    void enableIrq() override { irqEnabled = true; }
    void startConversion() override { ++swConversions; }
    std::uint16_t readResult() override { return raw; }

    /// Simulate one conversion: set the value and "fire" the ISR.
    void convert(std::uint16_t value) {
        raw = value;
        ADC_IRQHandler();
    }

    std::uint16_t raw = 0U;
    bool hwTrigger = false;
    bool irqEnabled = false;
    std::uint32_t swConversions = 0U;
};

class MockPin final : public hal::IDigitalOut {
public:
    void configureOutput() override { isOutput = true; }
    void write(bool on_) override {
        on = on_;
        ++writes;
    }

    bool on = false;
    bool isOutput = false;
    std::uint32_t writes = 0U;
};

class MockEeprom final : public hal::II2cBus {
public:
    MockEeprom() { erase(); }

    hal::Status init() override { return hal::Status::Ok; }

    hal::Status memRead(std::uint8_t devAddr, std::uint16_t memAddr, std::uint8_t *buf, std::size_t len) override {
        if (fail || (devAddr != board::kEepromI2cAddr) ||
            (static_cast<std::size_t>(memAddr) + len > mem.size())) {
            return hal::Status::Error;   // NACK
        }
        std::memcpy(buf, &mem[memAddr], len);
        return hal::Status::Ok;
    }

    void erase() { mem.fill(0xFFU); }

    void write(std::uint16_t addr, const void *data, std::size_t len) {
        if (static_cast<std::size_t>(addr) + len <= mem.size()) {
            std::memcpy(&mem[addr], data, len);
        }
    }

    void setConfig(std::uint8_t revision, const char *serial) {
        std::array<std::uint8_t, 16> buf{};
        const std::size_t n = std::strlen(serial);
        std::memcpy(buf.data(), serial, (n < buf.size()) ? n : buf.size());
        write(0x0000U, &revision, 1U);
        write(0x0001U, buf.data(), buf.size());
    }

    std::array<std::uint8_t, 256> mem{};
    bool fail = false;
};

class MockIrq final : public hal::IInterruptControl {
public:
    std::uint32_t saveAndDisable() override
    {
        ++lockCount;
        const std::uint32_t prev = disabled ? 1U : 0U;
        disabled = true;
        return prev;
    }
    void restore(std::uint32_t state) override { disabled = (state != 0U); }

    bool disabled = false;
    std::uint32_t lockCount = 0U;
};

/// A complete fake board.
struct MockBoard {
    MockTimer timer;
    MockAdc adc;
    MockEeprom eeprom;
    MockIrq irq;
    MockPin red;
    MockPin yellow;
    MockPin green;

    HalBundle hal() { return HalBundle{timer, adc, eeprom, irq, red, yellow, green}; }

    bool ledsAre(bool r, bool y, bool g) const {
        return (red.on == r) && (yellow.on == y) && (green.on == g);
    }
};

} // namespace tempmon::mock
