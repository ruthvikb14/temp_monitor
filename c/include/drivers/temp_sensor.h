/**
 * @file temp_sensor.h
 * @brief Sensor abstraction: converts raw ADC counts to deci-Celsius.
 *
 * Each hardware revision provides one const instance ("strategy"). The
 * instance is selected once at boot from the EEPROM revision; the rest of
 * the software only ever sees deci-Celsius and never checks the revision.
 * Adding Rev-C = one new conversion function + one table entry.
 */
#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "common/temp_types.h"

typedef struct {
    const char *name;
    temp_dC_t (*to_deci_celsius)(uint16_t raw);
} temp_sensor_t;

/** @return sensor for @p rev, or NULL if the revision is unsupported. */
const temp_sensor_t *temp_sensor_select(hw_revision_t rev);

#endif /* TEMP_SENSOR_H */
