#pragma once

#include <stdint.h>

void init_keys_HAL(void);
void keys_getKeys_HAL(void* ptr, unsigned long currentMillis);

#if(OMOTE_HARDWARE_REV >= 5)
    // called from the HAL
    void update_keyboardBrightness_HAL(void);
    void blink_keyboardBacklight_HAL(uint32_t durationMs, uint16_t phaseMs, uint8_t brightness);
    void set_keyboardBacklightLongBlink_HAL(bool enabled, uint16_t phaseMs, uint8_t brightness);
    uint8_t get_keyboardBrightness_HAL();
    void set_keyboardBrightness_HAL(uint8_t aKeyboardBrightness);
#endif