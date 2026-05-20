#include "telnet_hal_windows_linux.h"

#if (ENABLE_WIFI_AND_MQTT == 1)
// there is no telnet server in the simulator
void init_telnet_HAL(void) {}
void telnet_loop_HAL(void) {}
bool telnet_readLine_HAL(char* buf, size_t maxLen) {
  return false;
}
void telnet_endOfCommand_HAL(void) {}
void telnet_write_HAL(const char* text) {}
void telnet_notify_HAL(const char* text) {}
void telnet_close_HAL(void) {}
#endif
