#pragma once

#include "common/Types.hpp"
#include "hal/IDigitalOut.hpp"

namespace tempmon {

/// Maps a temperature level to the three status LEDs.
class LedIndicator {
public:
    LedIndicator(hal::IDigitalOut &red, hal::IDigitalOut &yellow, hal::IDigitalOut &green)
        : red_{red}, yellow_{yellow}, green_{green} {}

    void init();                    ///< configure outputs, all off
    void show(TempLevel level);     ///< exactly one LED lit
    void showFault();               ///< R + Y: fails safe, but distinguishable from Critical

private:
    void set(bool r, bool y, bool g);

    hal::IDigitalOut &red_;
    hal::IDigitalOut &yellow_;
    hal::IDigitalOut &green_;
};

} // namespace tempmon
