#include "applicationInternal/commandHandler.h"
#include "device_home.h"

uint16_t HOME_MQTT_COMMAND_CLOSE;
uint16_t HOME_MQTT_COMMAND_OPEN;

void register_device_home() {
  #if (ENABLE_WIFI_AND_MQTT == 1)
  register_command(&HOME_MQTT_COMMAND_CLOSE      , makeCommandData(MQTT, {"homeassistant/command", "close:cover.chambre" }));
  register_command(&HOME_MQTT_COMMAND_OPEN       , makeCommandData(MQTT, {"homeassistant/command", "open:cover.chambre" }));
  #endif
}
