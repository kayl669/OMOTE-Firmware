#pragma once

extern uint16_t BED_BLE_OKIN_BED_LIGHT;
extern uint16_t BED_BLE_OKIN_BED_MEMORY_1;
extern uint16_t BED_BLE_OKIN_BED_MEMORY_2;
extern uint16_t BED_BLE_OKIN_BED_HEAD_UP;
extern uint16_t BED_BLE_OKIN_BED_HEAD_DOWN;
extern uint16_t BED_BLE_OKIN_BED_FEET_UP;
extern uint16_t BED_BLE_OKIN_BED_FEET_DOWN;

#if (ENABLE_KEYBOARD_BLE == 1)

void register_device_bed_ble();
void bed_ble_executeCommand(uint16_t command, std::list<std::string> commandPayloads);
#endif
