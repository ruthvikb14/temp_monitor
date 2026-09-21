/**
 * @file temp_acquisition.h
 * @brief 100 us sampling and the ISR -> main-loop hand-off.
 *
 * The timer triggers the ADC in hardware; the ADC end-of-conversion ISR
 * only stores the raw value and bumps a sequence counter (a few cycles).
 * All conversion and decision logic runs in the main loop, so ISR
 * execution time stays constant and short.
 */
#ifndef TEMP_ACQUISITION_H
#define TEMP_ACQUISITION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t raw;       /**< latest raw ADC value */
    uint32_t seq;       /**< sample sequence number */
    uint32_t missed;    /**< samples overwritten since the previous read */
} temp_sample_t;

void temp_acq_init(void);
void temp_acq_start(void);
void temp_acq_stop(void);

/** Called from the ADC ISR only. */
void temp_acq_isr_push(uint16_t raw);

/**
 * Fetch the newest sample (main-loop context).
 * @return true if a new sample arrived since the last call.
 */
bool temp_acq_get_latest(temp_sample_t *out);

#endif /* TEMP_ACQUISITION_H */
