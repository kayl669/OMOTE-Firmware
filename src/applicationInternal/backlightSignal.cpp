#include "applicationInternal/backlightSignal.h"
#include "applicationInternal/hardware/hardwarePresenter.h"
#include "applicationInternal/omote_log.h"

// only hardware rev 5 and higher has a keyboard backlight
#if(OMOTE_HARDWARE_REV >= 5)

static uint8_t brightnessFromPercent(uint16_t percent) {
  return percent * 255 / 100;
}

void warnIfBatteryLowOnPickUp() {
  if (!get_wokenUpByMotion()) {return;}

  int battery_voltage;
  int battery_percentage;
  bool battery_ischarging;
  get_battery_status(&battery_voltage, &battery_percentage, &battery_ischarging);

  BlinkSettings low = get_backlightSignalSettings().low;
  // while charging, showChargingState() does the blinking
  if (!battery_ischarging && (battery_percentage < low.batteryPercent)) {
    omote_log_i("Battery at %d%%, will blink the keyboard backlight\r\n", battery_percentage);
    // without a time limit, it blinks until the device goes to sleep
    uint32_t durationMs = (low.seconds == 0) ? UINT32_MAX : low.seconds * 1000UL;
    blink_keyboardBacklight(durationMs, low.phaseMs, brightnessFromPercent(low.lightPercent));
  }
}

void showChargingState() {
  static bool wasCharging = false;
  static unsigned long chargingSince = 0;

  int battery_voltage;
  int battery_percentage;
  bool battery_ischarging;
  get_battery_status(&battery_voltage, &battery_percentage, &battery_ischarging);

  BlinkSettings charge = get_backlightSignalSettings().charge;
  bool charging = battery_ischarging && (battery_percentage < charge.batteryPercent);
  if (charging && !wasCharging) {
    chargingSince = millis();
  }
  wasCharging = charging;

  bool blinking = charging && ((charge.seconds == 0) || (millis() - chargingSince < charge.seconds * 1000UL));
  set_keyboardBacklightLongBlink(blinking, charge.phaseMs, brightnessFromPercent(charge.lightPercent));
  // there is no blinking in deep sleep, so stay awake as long as we are blinking
  if (blinking) {
    setLastActivityTimestamp();
  }
}

#endif
