#pragma once

#include <vector>

extern uint16_t HOME_MQTT_COMMAND_CLOSE;
extern uint16_t HOME_MQTT_COMMAND_OPEN;
extern std::vector<std::string> cover_list_home;

void register_device_home();
