#include "drivers/eeprom_config.h"

#include <string.h>
#include "board.h"
#include "hal/hal_i2c.h"

static int is_printable_ascii(uint8_t c) {
    return (c >= 0x20u) && (c <= 0x7Eu);
}

config_status_t eeprom_config_load(device_config_t *out) {
    uint8_t rev = 0u;
    uint8_t serial[EEPROM_SERIAL_MAX_LEN];
    size_t len = 0u;

    if (out == NULL) {
        return CONFIG_ERR_PARAM;
    }

    if (hal_i2c_mem_read(BOARD_EEPROM_I2C_ADDR, EEPROM_ADDR_REVISION,
                         &rev, 1u) != HAL_OK) {
        return CONFIG_ERR_I2C;
    }
    if (rev >= (uint8_t)HW_REV_COUNT) {
        return CONFIG_ERR_REVISION;
    }

    if (hal_i2c_mem_read(BOARD_EEPROM_I2C_ADDR, EEPROM_ADDR_SERIAL,
                         serial, sizeof serial) != HAL_OK) {
        return CONFIG_ERR_I2C;
    }
    while ((len < sizeof serial) && (serial[len] != 0u)) {
        if (!is_printable_ascii(serial[len])) {
            return CONFIG_ERR_SERIAL;
        }
        len++;
    }
    if (len == 0u) {
        return CONFIG_ERR_SERIAL;
    }

    out->revision = (hw_revision_t)rev;
    memcpy(out->serial, serial, len);
    out->serial[len] = '\0';
    return CONFIG_OK;
}
