/**
 * @file eeprom_config.h
 * @brief Device configuration stored in the I2C EEPROM.
 *
 * EEPROM layout:
 *   0x0000        1 byte    hardware revision (0 = Rev-A, 1 = Rev-B)
 *   0x0001..0x10  16 bytes  serial number, ASCII, NUL-padded
 */
#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#include "common/temp_types.h"

#define EEPROM_ADDR_REVISION   0x0000u
#define EEPROM_ADDR_SERIAL     0x0001u
#define EEPROM_SERIAL_MAX_LEN  16u

typedef struct {
    hw_revision_t revision;
    char serial[EEPROM_SERIAL_MAX_LEN + 1u];   /* always NUL-terminated */
} device_config_t;

typedef enum {
    CONFIG_OK = 0,
    CONFIG_ERR_PARAM,
    CONFIG_ERR_I2C,         /**< EEPROM not responding */
    CONFIG_ERR_REVISION,    /**< unknown revision (e.g. blank 0xFF) */
    CONFIG_ERR_SERIAL       /**< empty or non-printable serial */
} config_status_t;

/** Read and validate the configuration. @p out is only written on success. */
config_status_t eeprom_config_load(device_config_t *out);

#endif /* EEPROM_CONFIG_H */
