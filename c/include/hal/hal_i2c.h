#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stddef.h>
#include <stdint.h>
#include "hal/hal_common.h"

hal_status_t hal_i2c_init(void);

/**
 * Blocking read from a memory-addressed I2C device (EEPROM random read).
 * Only used during init, so blocking is acceptable.
 */
hal_status_t hal_i2c_mem_read(uint8_t dev_addr, uint16_t mem_addr,
                              uint8_t *buf, size_t len);

#endif /* HAL_I2C_H */
