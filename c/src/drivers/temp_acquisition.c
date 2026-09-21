#include "drivers/temp_acquisition.h"

#include <stddef.h>
#include "board.h"
#include "hal/hal_adc.h"
#include "hal/hal_irq.h"
#include "hal/hal_timer.h"

/* Shared with the ISR -> volatile. Read together under a critical section
 * so raw and seq always belong to the same sample. */
static volatile uint16_t s_latest_raw;
static volatile uint32_t s_seq;

/* Main-loop only. */
static uint32_t s_last_read_seq;

void temp_acq_init(void) {
    s_latest_raw = 0u;
    s_seq = 0u;
    s_last_read_seq = 0u;

    hal_adc_init(BOARD_ADC_HW_TRIGGER != 0);
    hal_timer_init_periodic(BOARD_SAMPLE_PERIOD_US, BOARD_ADC_HW_TRIGGER != 0);
}

void temp_acq_start(void) {
    hal_adc_enable_irq();
    hal_timer_start();
}

void temp_acq_stop(void) {
    hal_timer_stop();
}

void temp_acq_isr_push(uint16_t raw) {
    s_latest_raw = raw;
    s_seq = s_seq + 1u;
}

bool temp_acq_get_latest(temp_sample_t *out) {
    uint16_t raw;
    uint32_t seq;
    uint32_t key;

    if (out == NULL) {
        return false;
    }

    key = hal_irq_save_disable();
    raw = s_latest_raw;
    seq = s_seq;
    hal_irq_restore(key);

    if (seq == s_last_read_seq) {
        return false;
    }

    out->raw = raw;
    out->seq = seq;
    out->missed = seq - s_last_read_seq - 1u;   /* wrap-safe unsigned math */
    s_last_read_seq = seq;
    return true;
}
