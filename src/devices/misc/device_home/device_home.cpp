#include "applicationInternal/commandHandler.h"
#include "device_home.h"

#include "applicationInternal/hardware/IRremoteProtocols.h"

uint16_t HOME_MQTT_COMMAND;
uint16_t HOME_LIGHT_LIVING_ROOM;
uint16_t HOME_LIGHT_DINING_ROOM;
uint16_t HOME_LIGHT_KITCHEN;
uint16_t HOME_LIGHT_OFF;
uint16_t HOME_HOOD_POWER;
uint16_t HOME_HOOD_PLUS;
uint16_t HOME_HOOD_MINUS;
uint16_t HOME_HOOD_FAN_MAX;
uint16_t HOME_HOOD_LIGHT;
//uint16_t HOME_HOOD_TIMER;
//uint16_t HOME_HOOD_HUMIDITY;

std::vector<std::string> light_list_home = {"switch.eclairage_salon","switch.eclairage_salle","light.eclairage_cuisine"};
std::vector<std::string> cover_list_home = {"cover.salon","cover.salle_a_manger","cover.cuisine", "cover.store_cuisine"};

void register_device_home() {
  #if (ENABLE_WIFI_AND_MQTT == 1)
  register_command(&HOME_MQTT_COMMAND      , makeCommandData(MQTT, {"homeassistant/command"   }));
  register_command(&HOME_LIGHT_LIVING_ROOM , makeCommandData(MQTT, {"homeassistant/command", "toggle:" + light_list_home[0] }));
  register_command(&HOME_LIGHT_DINING_ROOM , makeCommandData(MQTT, {"homeassistant/command", "toggle:" + light_list_home[1] }));
  register_command(&HOME_LIGHT_KITCHEN     , makeCommandData(MQTT, {"homeassistant/command", "toggle:" + light_list_home[2] }));
  register_command(&HOME_LIGHT_OFF         , makeCommandData(MQTT, {"homeassistant/command", "off:" + light_list_home[0] + "," + light_list_home[1] + "," + light_list_home[2] }));
  #endif
  register_command(&HOME_HOOD_POWER        , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("764, 726,  712, 1424,  1462, 728,  712, 732,  712, 1422,  1464, 2116,  710, 1424,  712, 730,  1408", 17, 38)}));  // 0x4D06F7CD
  register_command(&HOME_HOOD_PLUS         , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("762, 730,  2214, 728,  712, 728,  714, 728,  2212, 728,  714, 2116,  2214, 1420,  1490", 15, 38)}));  // 0xD366A636
  register_command(&HOME_HOOD_MINUS        , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("790, 1400,  1518, 670,  740, 704,  738, 1398,  1520, 674,  766, 2756,  1518, 1366,  1516", 15, 38)}));  // 0x9A38260A
  register_command(&HOME_HOOD_FAN_MAX        , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("760, 736,  1454, 1426,  708, 734,  708, 732,  1460, 1426,  708, 2122,  1456, 2120,  1458", 15, 38)}));  // 0xF7B0BF8F
  register_command(&HOME_HOOD_LIGHT        , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("794, 702, 740, 702, 1490, 1394, 742, 700, 742, 700, 1490, 2782, 740, 700, 738, 704, 2240", 17, 38)}));  // 0x4668257
//  register_command(&HOME_HOOD_TIMER        , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("794, 1396, 1490, 1392, 742, 2088, 1492, 2086, 768, 1368, 1492", 11, 38)}));  // 0x2BBE08AB
//  register_command(&HOME_HOOD_HUMIDITY        , makeCommandData(IR, {std::to_string(IR_PROTOCOL_RAW), concatenateIRsendRawParams("792, 700, 742, 1394, 742, 702, 740, 1394, 746, 1390, 742, 1396, 738, 702, 740, 1396, 738",17, 38)}));  // 0x83318915
}
