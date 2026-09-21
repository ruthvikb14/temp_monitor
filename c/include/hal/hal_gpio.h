#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include "board.h"

void hal_gpio_init_output(board_pin_t pin);
/** @param on true = LED lit */
void hal_gpio_write(board_pin_t pin, bool on);

#endif /* HAL_GPIO_H */
