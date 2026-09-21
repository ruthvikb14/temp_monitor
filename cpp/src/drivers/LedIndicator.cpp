#include "drivers/LedIndicator.hpp"

namespace tempmon {

void LedIndicator::init() {
    red_.configureOutput();
    yellow_.configureOutput();
    green_.configureOutput();
    set(false, false, false);
}

void LedIndicator::show(TempLevel level) {
    set(level == TempLevel::Critical, level == TempLevel::Warning, level == TempLevel::Normal);
}

void LedIndicator::showFault() {
    set(true, true, false);
}

void LedIndicator::set(bool r, bool y, bool g) {
    red_.write(r);
    yellow_.write(y);
    green_.write(g);
}

} // namespace tempmon
