#include "drivers/TempAcquisition.hpp"

#include "Board.hpp"

namespace tempmon {

void TempAcquisition::init() {
    latestRaw_ = 0U;
    seq_ = 0U;
    lastReadSeq_ = 0U;
    adc_.init(board::kAdcHwTrigger);
    timer_.configure(board::kSamplePeriodUs, board::kAdcHwTrigger);
}

void TempAcquisition::start() {
    adc_.enableIrq();
    timer_.start();
}

void TempAcquisition::stop() {
    timer_.stop();
}

void TempAcquisition::onConversionComplete() {
    latestRaw_ = adc_.readResult();
    seq_ = seq_ + 1U;
}

void TempAcquisition::onTimerUpdate() {
    if (!board::kAdcHwTrigger) {
        timer_.clearIrq();
        adc_.startConversion();
    }
}

bool TempAcquisition::fetchLatest(Sample &out) {
    std::uint16_t raw;
    std::uint32_t seq;
    {
        hal::CriticalSection lock{irq_};   // raw + seq from the same sample
        raw = latestRaw_;
        seq = seq_;
    }

    if (seq == lastReadSeq_) {
        return false;
    }
    out.raw = raw;
    out.seq = seq;
    out.missed = seq - lastReadSeq_ - 1U;    // wrap-safe unsigned math
    lastReadSeq_ = seq;
    return true;
}

} // namespace tempmon
