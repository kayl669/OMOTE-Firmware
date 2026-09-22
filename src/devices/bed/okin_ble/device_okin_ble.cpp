#include "applicationInternal/commandHandler.h"
#include "applicationInternal/omote_log.h"
#include "device_okin_ble.h"
#if (ENABLE_KEYBOARD_BLE == 1)
#define OKIN_BED_COMMAND_LIGHT      "0x040200020000"
#define OKIN_BED_COMMAND_MEMORY_1   "0x040200001000"
#define OKIN_BED_COMMAND_MEMORY_2   "0x040200002000"
#define OKIN_BED_COMMAND_HEAD_UP    "0x040200000001"
#define OKIN_BED_COMMAND_HEAD_DOWN  "0x040200000002"
#define OKIN_BED_COMMAND_FEET_UP    "0x040200000004"
#define OKIN_BED_COMMAND_FEET_DOWN  "0x040200000008"

#if defined(ARDUINO)

#include <NimBLEDevice.h>

#define OKIN_SERVICE_UUID          "62741523-52f9-8864-b1ab-3b3a8d65950b"
#define OKIN_WRITE_CHAR_UUID       "62741525-52f9-8864-b1ab-3b3a8d65950b"

class DeviceOkinBLE : public NimBLEClientCallbacks {

public:
    DeviceOkinBLE(const char *bedMacAddress) : bed_mac_address(bedMacAddress) {
    }

    bool writeCommand(const std::string& hexStr);

private:
    void begin();
    bool connect();
    void onConnect(NimBLEClient* pClient);
    void onDisconnect(NimBLEClient* pClient);
    BLEClient* client = nullptr;
    BLERemoteCharacteristic* g_writeChar = nullptr;
    bool initialized = false;
    bool connected = false;
    const char *bed_mac_address;
};

bool DeviceOkinBLE::writeCommand(const std::string& hexStr)
{
    if (!initialized) {
        begin();
    }
    if (!connected && !connect()) {
        omote_log_i("Impossible to connect. Check MAC address and pairing");
    }

    std::string h = hexStr;
    if (h.rfind("0x", 0) == 0) h = h.substr(2);

    if (h.size() != 12) {
        omote_log_d("invalid command");
        return false;
    }
    uint8_t cmd[6];
    for (int i = 0; i < 6; i++)
        cmd[i] = strtoul(h.substr(i * 2, 2).c_str(), nullptr, 16);

    if (!connected || g_writeChar == nullptr) {
        omote_log_e("Not connected, command ignored");
        return false;
    }
    omote_log_d("%s\n", hexStr.c_str());
    return g_writeChar->writeValue(cmd, 6, /*response=*/true);
}

void DeviceOkinBLE::onConnect(NimBLEClient* pClient) {
    connected = true;
    omote_log_d("Connected to the bed\n");
}

void DeviceOkinBLE::onDisconnect(NimBLEClient* pClient) {
    connected = false;
    g_writeChar  = nullptr;
    omote_log_d("Disconnected to the bed\n");
}

void DeviceOkinBLE::begin() {
    if (initialized) {
        return;
    }
    NimBLEDevice::init("ESP32-DeviceOkinBLE");
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setMTU(512);
    initialized = true;
}

bool DeviceOkinBLE::connect() {
    NimBLEAddress addr(bed_mac_address, BLE_ADDR_RANDOM);
    client = NimBLEDevice::createClient();
    client->setClientCallbacks(this, false);
    client->setConnectionParams(16, 16, 0, 400);
    client->setConnectTimeout(15);
    if (!client->connect(addr)) {
        omote_log_e("Connection failed\n");
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        connected = false;
        return false;
    }
    delay(500);
    client->secureConnection();
    delay(500);

    auto* service =
            client->getService(OKIN_SERVICE_UUID);

    if (!service) {
        omote_log_e("OKIN service not found\n");
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        connected = false;
        return false;
    }

    g_writeChar = service->getCharacteristic(OKIN_WRITE_CHAR_UUID);

    if (!g_writeChar) {
        omote_log_e("Write characteristic not found\n");
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        connected = false;
        return false;
    }

    connected = true;

    omote_log_i("OKIN connected\n");

    return true;
}
#endif
#endif

uint16_t BED_BLE_OKIN_BED_LIGHT      ; //"bed_ble_okin_bed_light";
uint16_t BED_BLE_OKIN_BED_MEMORY_1   ; //"bed_ble_okin_bed_memory_1";
uint16_t BED_BLE_OKIN_BED_MEMORY_2   ; //"bed_ble_okin_bed_memory_2";
uint16_t BED_BLE_OKIN_BED_HEAD_UP    ; //"bed_ble_okin_bed_head_up";
uint16_t BED_BLE_OKIN_BED_HEAD_DOWN  ; //"bed_ble_okin_bed_head_down";
uint16_t BED_BLE_OKIN_BED_FEET_UP    ; //"bed_ble_okin_bed_feet_up";
uint16_t BED_BLE_OKIN_BED_FEET_DOWN  ; //"bed_ble_okin_bed_feet_down";

#if (ENABLE_KEYBOARD_BLE == 1)
void register_device_bed_ble() {
  register_command(&BED_BLE_OKIN_BED_LIGHT      , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_LIGHT}));
  register_command(&BED_BLE_OKIN_BED_MEMORY_1   , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_MEMORY_1}));
  register_command(&BED_BLE_OKIN_BED_MEMORY_2   , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_MEMORY_2}));
  register_command(&BED_BLE_OKIN_BED_HEAD_UP    , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_HEAD_UP}));
  register_command(&BED_BLE_OKIN_BED_HEAD_DOWN  , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_HEAD_DOWN}));
  register_command(&BED_BLE_OKIN_BED_FEET_UP    , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_FEET_UP}));
  register_command(&BED_BLE_OKIN_BED_FEET_DOWN  , makeCommandData(BLE_BED, {OKIN_BED_COMMAND_FEET_DOWN}));
}

DeviceOkinBLE bed("D0:BF:BD:48:E2:5B");
// DeviceOkinBLE bed("dd:bb:20:77:c0:80");

void bed_ble_executeCommand(uint16_t command, std::list<std::string> commandPayloads) {
#if defined(ARDUINO)
  std::string cmd = *commandPayloads.begin();

  if (command == BED_BLE_OKIN_BED_LIGHT) {
    omote_log_d("BED_BLE_OKIN_BED_LIGHT received\r\n");
    bed.writeCommand(cmd);
  } else if (command == BED_BLE_OKIN_BED_MEMORY_1) {
    omote_log_d("BED_BLE_OKIN_BED_MEMORY_1 received\r\n");
    bed.writeCommand(cmd);
  } else if (command == BED_BLE_OKIN_BED_MEMORY_2) {
    omote_log_d("BED_BLE_OKIN_BED_MEMORY_2 received\r\n");
    bed.writeCommand(cmd);
  } else if (command == BED_BLE_OKIN_BED_HEAD_UP) {
    omote_log_d("BED_BLE_OKIN_BED_HEAD_UP received\r\n");
    bed.writeCommand(cmd);
  } else if (command == BED_BLE_OKIN_BED_HEAD_DOWN) {
    omote_log_d("BED_BLE_OKIN_BED_HEAD_DOWN received\r\n");
    bed.writeCommand(cmd);
  } else if (command == BED_BLE_OKIN_BED_FEET_UP) {
    omote_log_d("BED_BLE_OKIN_BED_FEET_UP received\r\n");
    bed.writeCommand(cmd);
  } else if (command == BED_BLE_OKIN_BED_FEET_DOWN) {
    omote_log_d("BED_BLE_OKIN_BED_FEET_DOWN received\r\n");
    bed.writeCommand(cmd);
  }
#endif
}
#endif
