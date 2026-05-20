#pragma once

#include <stddef.h>

#if (ENABLE_WIFI_AND_MQTT == 1)
void init_telnet_HAL(void);
void telnet_loop_HAL(void);
bool telnet_readLine_HAL(char* buf, size_t maxLen);
void telnet_endOfCommand_HAL(void);
void telnet_write_HAL(const char* text);
void telnet_notify_HAL(const char* text);
void telnet_close_HAL(void);
#endif
