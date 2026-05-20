#pragma once

void init_keys_HAL(void);
void keys_getKeys_HAL(void* ptr, unsigned long currentMillis);

#if(OMOTE_HARDWARE_REV >= 5)
    // called from the HAL
    void update_keyboardBrightness_HAL(void);
    uint8_t get_keyboardBrightness_HAL();
    void set_keyboardBrightness_HAL(uint8_t aKeyboardBrightness);
#endif