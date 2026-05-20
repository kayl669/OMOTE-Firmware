#pragma once

#include <stdint.h>

// one blink signal. Both signals have the same settings
struct BlinkSettings {
  uint16_t batteryPercent; // battery level which controls the signal, 0 = signal is off
  uint16_t seconds;        // how long it blinks, 0 = no limit
  uint16_t phaseMs;        // duration of each "on" and each "off" phase
  uint16_t lightPercent;   // brightness of the "on" phase
};

// Battery state shown with the keyboard backlight. All values can be changed on the serial port
// (command "blink") and are saved in the preferences. The values given here are the defaults.
// This struct is also used by the hardware layer to save and restore it.
struct BacklightSignalSettings {
  // warning when the remote is picked up: blinks if the battery level is below batteryPercent
  BlinkSettings low    = {10, 5, 250, 100};
  // while charging: blinks until the battery level reaches batteryPercent
  BlinkSettings charge = {95, 0, 2000, 25};
};

// to be called at the end of setup(), the blinking itself is done in loop()
void warnIfBatteryLowOnPickUp();
// to be called regularly in loop()
void showChargingState();
