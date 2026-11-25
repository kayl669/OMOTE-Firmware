#pragma once

#include <stdint.h>

// The names come from the OMOTE board. Even without a display, these two pins have to be driven.
extern const uint8_t LCD_EN_GPIO; // switches the power which is also needed by the I2C bus, active low
extern const uint8_t LCD_BL_GPIO; // backlight, active low

void init_hardware_general_HAL(void);
void init_i2cBus_HAL(void);
