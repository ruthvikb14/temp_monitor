#include "app/temp_monitor.h"

#include <stddef.h>

/* margin > 0 moves every threshold towards the safe side; used to decide
 * whether we may LEAVE the current level. */
static temp_level_t classify_with_margin(temp_dC_t t, temp_dC_t margin) {
    if ((t >= TEMP_CRIT_HIGH_DC - margin) || (t < TEMP_CRIT_LOW_DC + margin)) {
        return TEMP_LEVEL_CRITICAL;
    }
    if (t >= TEMP_WARN_HIGH_DC - margin) {
        return TEMP_LEVEL_WARNING;
    }
    return TEMP_LEVEL_NORMAL;
}

void temp_monitor_init(temp_monitor_t *m, temp_dC_t hysteresis_dC) {
    if (m == NULL) {
        return;
    }
    m->level = TEMP_LEVEL_NORMAL;
    m->hysteresis_dC = (hysteresis_dC > 0) ? hysteresis_dC : 0;
    m->has_level = false;
}

temp_level_t temp_monitor_classify(temp_dC_t t) {
    return classify_with_margin(t, 0);
}

temp_level_t temp_monitor_update(temp_monitor_t *m, temp_dC_t t) {
    temp_level_t entry = classify_with_margin(t, 0);

    if (!m->has_level || (entry >= m->level)) {
        /* first sample, or same/more severe: apply immediately */
        m->level = entry;
        m->has_level = true;
    } else {
        /* less severe: only step down as far as the hysteresis band allows */
        temp_level_t hold = classify_with_margin(t, m->hysteresis_dC);
        if (hold < m->level) {
            m->level = (hold > entry) ? hold : entry;
        }
    }
    return m->level;
}
