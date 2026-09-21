/**
 * @file main_target.cpp
 * @brief Entry point for the real target. NOT part of the PC build.
 *
 * Illustrative: requires port/<mcu>/TargetHal.hpp with register-level
 * classes implementing the hal:: interfaces. All objects are static - no
 * heap. Their constructors run from the startup code (__libc_init_array)
 * before main().
 */
#include "TargetHal.hpp"
#include "app/Application.hpp"

namespace {
target::SampleTimer  g_timer;                          // e.g. TIM2, TRGO -> ADC
target::Adc          g_adc;                            // e.g. ADC1, channel 0
target::I2cBus       g_i2c;                            // e.g. I2C1
target::IrqControl   g_irq;                            // PRIMASK
target::GpioOut      g_red{target::Port::B, 0U};
target::GpioOut      g_yellow{target::Port::B, 1U};
target::GpioOut      g_green{target::Port::B, 2U};

tempmon::Application g_app{{g_timer, g_adc, g_i2c, g_irq, g_red, g_yellow, g_green}};
} // namespace

int main() {
    (void)g_app.init();     // on failure the LEDs already show the fault

    for (;;) {
        g_app.runOnce();
        // __WFI();  sleep until the next interrupt (ADC EOC every 100 us)
    }
}
