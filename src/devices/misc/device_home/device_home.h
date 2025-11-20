#pragma once

#include <vector>

extern uint16_t HOME_MQTT_COMMAND;
extern uint16_t HOME_LIGHT_LIVING_ROOM;
extern uint16_t HOME_LIGHT_DINING_ROOM;
extern uint16_t HOME_LIGHT_KITCHEN;
extern uint16_t HOME_LIGHT_OFF;
extern uint16_t HOME_HOOD_POWER;
extern uint16_t HOME_HOOD_PLUS;
extern uint16_t HOME_HOOD_MINUS;
extern uint16_t HOME_HOOD_FAN_MAX;
extern uint16_t HOME_HOOD_LIGHT;
//extern uint16_t HOME_HOOD_TIMER;
//extern uint16_t HOME_HOOD_HUMIDITY;
extern std::vector<std::string> light_list_home;
extern std::vector<std::string> cover_list_home;

void register_device_home();
