#pragma once

#include <string>

void register_device_serial();
// handles the commands received on the serial port and by telnet
void handleSerialInput();
#if (ENABLE_WIFI_AND_MQTT == 1)
// shows a message in the telnet session, if there is one. Can be called from every thread
void showOnTelnet(const std::string &message);
#endif
