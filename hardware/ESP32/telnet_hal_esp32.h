#pragma once

#include <stddef.h>

#if (ENABLE_WIFI_AND_MQTT == 1)
// Telnet server for one session at a time, protected by OTA_PASSWORD. Has to be started after init_mqtt_HAL()
void init_telnet_HAL(void);
// accepts connections and closes them after a timeout
void telnet_loop_HAL(void);
// handles the login and the line editing. Returns true if a command was entered
bool telnet_readLine_HAL(char* buf, size_t maxLen);
// to be called after the command returned by telnet_readLine_HAL() has been executed
void telnet_endOfCommand_HAL(void);
// output of the command being executed
void telnet_write_HAL(const char* text);
// a message which is not the answer to a command
void telnet_notify_HAL(const char* text);
void telnet_close_HAL(void);
#endif
