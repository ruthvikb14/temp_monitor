/**
 * @file temp_types.h
 * @brief Types shared by all layers.
 */
#ifndef TEMP_TYPES_H
#define TEMP_TYPES_H

#include <stdint.h>

/**
 * Temperature in tenths of a degree Celsius ("deci-Celsius").
 * 85.0 degC == 850. Integer math only: no FPU needed, no rounding surprises.
 */
typedef int32_t temp_dC_t;

/** Hardware revision as stored in EEPROM. */
typedef enum {
    HW_REV_A = 0,   /**< sensor resolution 1.0 degC / digit */
    HW_REV_B = 1,   /**< sensor resolution 0.1 degC / digit */
    HW_REV_COUNT
} hw_revision_t;

/** Temperature condition, ordered by severity (higher = worse). */
typedef enum {
    TEMP_LEVEL_NORMAL   = 0,    /**< green  */
    TEMP_LEVEL_WARNING  = 1,    /**< yellow */
    TEMP_LEVEL_CRITICAL = 2     /**< red    */
} temp_level_t;

#endif /* TEMP_TYPES_H */
