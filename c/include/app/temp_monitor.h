/**
 * @file temp_monitor.h
 * @brief Temperature classification with hysteresis. Pure logic, no HW.
 *
 *   NORMAL   (G)  5.0 <= T <  85.0 degC
 *   WARNING  (Y) 85.0 <= T < 105.0 degC
 *   CRITICAL (R)         T >= 105.0 degC  or  T < 5.0 degC
 *
 * Entering a more severe level is immediate. Leaving it requires the
 * temperature to move back by the hysteresis margin, so the LEDs do not
 * flicker when the temperature sits on a threshold with ADC noise.
 */
#ifndef TEMP_MONITOR_H
#define TEMP_MONITOR_H

#include <stdbool.h>
#include "common/temp_types.h"

#define TEMP_WARN_HIGH_DC     850     /*  85.0 degC */
#define TEMP_CRIT_HIGH_DC    1050     /* 105.0 degC */
#define TEMP_CRIT_LOW_DC       50     /*   5.0 degC */

typedef struct {
    temp_level_t level;
    temp_dC_t    hysteresis_dC;
    bool         has_level;
} temp_monitor_t;

/** @param hysteresis_dC margin in deci-Celsius, 0 disables hysteresis. */
void temp_monitor_init(temp_monitor_t *m, temp_dC_t hysteresis_dC);

/** Classify without hysteresis (the plain requirement). */
temp_level_t temp_monitor_classify(temp_dC_t t);

/** Feed one temperature, returns the (possibly unchanged) level. */
temp_level_t temp_monitor_update(temp_monitor_t *m, temp_dC_t t);

#endif /* TEMP_MONITOR_H */
