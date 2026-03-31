#pragma once
#include <string>

// Advertising is started automatically.
// As soon as a device is connected, a small indicator in the top left corner of the screen will appear

#if (ENABLE_KEYBOARD_BLE == 1)

#if (ENABLE_KEYBOARD_BLE == 1) && !(ENABLE_BLUETOOTH == 1)
static_assert(false, "You have to use \"-D ENABLE_BLUETOOTH=1\" in \"platformio.ini\" when having \"-D ENABLE_KEYBOARD_BLE=1\"");
#endif

// commands without a specific address
extern uint16_t KEYBOARD_BLE_FREEBOX_UP;
extern uint16_t KEYBOARD_BLE_FREEBOX_DOWN;
extern uint16_t KEYBOARD_BLE_FREEBOX_RIGHT;
extern uint16_t KEYBOARD_BLE_FREEBOX_LEFT;
extern uint16_t KEYBOARD_BLE_FREEBOX_SELECT;
extern uint16_t KEYBOARD_BLE_FREEBOX_SENDSTRING;
extern uint16_t KEYBOARD_BLE_FREEBOX_BACK;
extern uint16_t KEYBOARD_BLE_FREEBOX_HOME;
extern uint16_t KEYBOARD_BLE_FREEBOX_MENU;
extern uint16_t KEYBOARD_BLE_FREEBOX_SCAN_PREVIOUS_TRACK;
extern uint16_t KEYBOARD_BLE_FREEBOX_REWIND_LONG;
extern uint16_t KEYBOARD_BLE_FREEBOX_REWIND;
extern uint16_t KEYBOARD_BLE_FREEBOX_PLAYPAUSE;
extern uint16_t KEYBOARD_BLE_FREEBOX_FASTFORWARD;
extern uint16_t KEYBOARD_BLE_FREEBOX_FASTFORWARD_LONG;
extern uint16_t KEYBOARD_BLE_FREEBOX_SCAN_NEXT_TRACK;
extern uint16_t KEYBOARD_BLE_FREEBOX_MUTE;
extern uint16_t KEYBOARD_BLE_FREEBOX_VOLUME_INCREMENT;
extern uint16_t KEYBOARD_BLE_FREEBOX_VOLUME_DECREMENT;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_1;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_2;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_3;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_4;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_5;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_6;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_7;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_8;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_9;
extern uint16_t KEYBOARD_BLE_FREEBOX_NUM_0;


void register_device_keyboard_ble();
void keyboard_ble_executeCommand(uint16_t command, std::list<std::string> commandPayloads, std::string additionalPayload = "");

#endif
