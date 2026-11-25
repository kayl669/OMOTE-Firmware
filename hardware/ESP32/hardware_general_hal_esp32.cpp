#include <Arduino.h>
#include <Wire.h>
#include "hardware_general_hal_esp32.h"

#if (OMOTE_HARDWARE_REV >= 5)
const uint8_t SDA_GPIO = 20;
const uint8_t SCL_GPIO = 19;
const uint8_t LCD_BL_GPIO = 9;
const uint8_t LCD_EN_GPIO = 38;
#else
const uint8_t SDA_GPIO = 19;
const uint8_t SCL_GPIO = 22;
const uint8_t LCD_BL_GPIO = 4;
const uint8_t LCD_EN_GPIO = 10;
#endif

void init_hardware_general_HAL(void) {
    // Make sure ESP32 is running at full speed
    setCpuFrequencyMhz(240);

    // peripheral power off, until init_i2cBus_HAL() switches it on
    digitalWrite(LCD_EN_GPIO, HIGH);
    pinMode(LCD_EN_GPIO, OUTPUT);
    // there is no display, so the backlight always stays off
    digitalWrite(LCD_BL_GPIO, HIGH);
    pinMode(LCD_BL_GPIO, OUTPUT);
}

void init_i2cBus_HAL(void) {
#if (OMOTE_HARDWARE_REV == 1)
    // Slowly charge the VSW voltage to prevent a brownout
    // Workaround for hardware rev 1!
    for (int i = 0; i < 100; i++) {
        digitalWrite(LCD_EN_GPIO, HIGH); // power off
        delayMicroseconds(1);
        digitalWrite(LCD_EN_GPIO, LOW); // power on
    }
#else
    digitalWrite(LCD_EN_GPIO, LOW);
#endif

    delay(100); // Wait for the peripherals to power on

    // Start the I2C bus on OMOTE's pins before the I2C devices are initialized.
    // Several libraries call Wire.begin() without arguments (LIS3DH, Adafruit BusIO for the
    // TCA8418 keypad). If Wire is not running at that point, they start it on the board defaults
    // - on rev5 SDA=8/SCL=9, which are the keypad interrupt and the backlight.
    Wire.begin(SDA_GPIO, SCL_GPIO, 100000);
}
