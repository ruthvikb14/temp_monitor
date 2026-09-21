/**
 * @file led_indicator.h
 * @brief Maps a temperature level to the three status LEDs.
 */
#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include "common/temp_types.h"

void led_indicator_init(void);

/** Exactly one LED lit: G = normal, Y = warning, R = critical. */
void led_indicator_show(temp_level_t level);

/** System fault (bad config): R + Y lit, so it is distinguishable from a
 *  genuine critical temperature while still failing safe (red on). */
void led_indicator_show_fault(void);

#endif /* LED_INDICATOR_H */
