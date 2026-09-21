#include "drivers/temp_sensor.h"

#include <stddef.h>

/* Rev-A: 1 digit = 1.0 degC  ->  raw 10 = 10.0 degC = 100 dC */
static temp_dC_t rev_a_to_deci_celsius(uint16_t raw) {
    return (temp_dC_t)raw * 10;
}

/* Rev-B: 1 digit = 0.1 degC  ->  raw 100 = 10.0 degC = 100 dC */
static temp_dC_t rev_b_to_deci_celsius(uint16_t raw) {
    return (temp_dC_t)raw;
}

static const temp_sensor_t k_sensors[HW_REV_COUNT] = {
    [HW_REV_A] = { "Rev-A (1.0 degC/digit)", rev_a_to_deci_celsius },
    [HW_REV_B] = { "Rev-B (0.1 degC/digit)", rev_b_to_deci_celsius },
};

const temp_sensor_t *temp_sensor_select(hw_revision_t rev) {
    if ((unsigned)rev >= (unsigned)HW_REV_COUNT) {
        return NULL;
    }
    return &k_sensors[rev];
}
