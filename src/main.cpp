// OMOTE firmware for ESP32, remote without display
// 2023-2025 Maximilian Kern, Klaus Musch

#include "applicationInternal/omote_log.h"
// init hardware and hardware loop
#include "applicationInternal/hardware/hardwarePresenter.h"
// register devices and their commands
//   special
#include "applicationInternal/commandHandler.h"
#include "devices/misc/device_serial/device_serial.h"
//   keyboards
#if (ENABLE_KEYBOARD_MQTT == 1)
#include "devices/keyboard/device_keyboard_mqtt/device_keyboard_mqtt.h"
#endif // ENABLE_KEYBOARD_MQTT
#if (ENABLE_KEYBOARD_BLE == 1)
#include "devices/keyboard/device_keyboard_ble/device_keyboard_ble.h"
#endif // ENABLE_KEYBOARD_BLE
//   TV
#include "devices/TV/device_samsungTV/device_samsungTV.h"
// keys
#include "applicationInternal/keys.h"
#include "applicationInternal/memoryUsage.h"
// register scenes
#include "scenes/scene__default.h"
#include "scenes/scene_allOff.h"
#include "applicationInternal/scenes/sceneHandler.h"
// battery state shown with the keyboard backlight
#include "applicationInternal/backlightSignal.h"

void setup() {

  // --- Startup ---
  Serial.begin(115200);
  // do some general hardware setup, like powering the TFT, I2C, ...
  init_hardware_general();
  // get wakeup reason
  init_sleep();
  // Restore settings from internal flash memory
  init_preferences();
  // blinking led
  init_userled();
  // startup SD card
  #if(OMOTE_HARDWARE_REV >= 5)
  // SD card is currently not used, so save some time on startup and don't init the SD card
  // init_SD_card();
  #endif

  // setup IR sender
  init_infraredSender();
  register_device_serial();

  //   TV
  register_device_samsungTV();

  #if (ENABLE_KEYBOARD_MQTT == 1)
  register_device_keyboard_mqtt();
  #endif
  #if (ENABLE_KEYBOARD_BLE == 1)
  register_device_keyboard_ble();
  #endif
  register_keyboardCommands();

  // register the scenes and their key_commands_*
  register_scene_defaultKeys();
  register_scene_allOff();

  // power the peripherals and start the I2C bus
  init_i2cBus(); // This has to come before any other i2c devices are initialized, otherwise the i2c bus will not be powered

  // Power Pin and battery monitor definition
  init_battery();

  // init BLE keyboard. Has to be after init_i2cBus (because of powered I2C) and after init_battery (because of fuel gauge init)
  #if (ENABLE_KEYBOARD_BLE == 1)
  init_keyboardBLE();
  #endif

  // setup keyboard matrix driver
  init_keys();

  // setup the Inertial Measurement Unit (IMU) for motion detection. Has to be after init_i2cBus(), otherwise I2C will not work
  init_IMU();

  // init WiFi
  #if (ENABLE_WIFI_AND_MQTT == 1)
  init_mqtt();
  // telnet server for the same commands as on the serial port
  init_telnet();
  #endif

  #if (ENABLE_OTA == 1)
  init_ota();
  #endif

  // has to be at the end of setup, because the blinking is done in loop()
  #if(OMOTE_HARDWARE_REV >= 5)
  warnIfBatteryLowOnPickUp();
  #endif

  omote_log_i("Setup finished in %lu ms.\r\n", millis());

}

// Loop ------------------------------------------------------------------------------------------------------------------------------------
unsigned long IMUTaskTimer = 0;
unsigned long updateStatusTimer = 0;
void loop() {

  // --- do as often as possible --------------------------------------------------------
  // update keyboard brightness. Fade in on startup
  #if(OMOTE_HARDWARE_REV >= 5)
    update_keyboardBrightness();
  #endif
  // keypad handling: get key states from hardware and process them
  keypad_loop();
  // process IR receiver, if activated
  if (get_irReceiverEnabled()) {
    infraredReceiver_loop();
  }
  // call mqtt loop to receive mqtt messages, if you are subscribed to some topics
  #if (ENABLE_WIFI_AND_MQTT == 1)
  mqtt_loop();
  #endif

  #if (ENABLE_OTA == 1)
  ota_loop();
  #endif

  // --- every 100 ms -------------------------------------------------------------------
  // Refresh IMU data (motion detection) every 100 ms
  // If no action (key or motion), then go to sleep
  if(millis() - IMUTaskTimer >= 100){
    IMUTaskTimer = millis();

    check_activity();

  }

  // --- every 1000 ms ------------------------------------------------------------------
  if(millis() - updateStatusTimer >= 1000) {
    updateStatusTimer = millis();

    update_userled();
    #if(OMOTE_HARDWARE_REV >= 5)
    showChargingState();
    #endif
    doLogMemoryUsage();
  }

  handleSerialInput();
}
