#include <sstream>
#include <algorithm>
#include <vector>
#include <Arduino.h>
#include <cstring>
#include <time.h>
#include "WiFi.h"
#include <PubSubClient.h>
#include <Preferences.h>
#include "mqtt_hal_esp32.h"
#if (ENABLE_KEYBOARD_BLE == 1)
#include "keyboard_ble_hal_esp32.h"
#endif
#include "secrets.h"
#include "esp_sntp.h"

#if (ENABLE_WIFI_AND_MQTT == 1)
WiFiClient espClient;
PubSubClient mqttClient(espClient);
bool isWifiConnected = false;
bool wifiShutdownRequested = false;
bool wifiCredentialsLoaded = false;
constexpr const char* kNtpServer1 = "pool.ntp.org";
constexpr unsigned long kMqttReconnectBaseIntervalMs = 5000;
constexpr unsigned long kMqttReconnectMaxIntervalMs = 60000;
constexpr unsigned long kMqttFailureLogIntervalMs = 10000;
constexpr unsigned long kMqttConfigLogIntervalMs = 60000;
constexpr unsigned long kTimeSyncLogIntervalMs = 30000;
unsigned long lastWifiReconnectAttemptMs = 0;
unsigned long mqttReconnectIntervalMs = kMqttReconnectBaseIntervalMs;
unsigned long lastReconnectAttempt = 0;
unsigned long lastTimeSyncRequestMs = 9999;
unsigned long lastTimeSyncLogMs = 0;
volatile bool ntpSyncPending = false;

tAnnounceWiFiconnected_cb thisAnnounceWiFiconnected_cb = NULL;
void set_announceWiFiconnected_cb_HAL(tAnnounceWiFiconnected_cb pAnnounceWiFiconnected_cb) {
  thisAnnounceWiFiconnected_cb = pAnnounceWiFiconnected_cb;  
}

tAnnounceSubscribedTopics_cb thisAnnounceSubscribedTopics_cb = NULL;
void set_announceSubscribedTopics_cb_HAL(tAnnounceSubscribedTopics_cb pAnnounceSubscribedTopics_cb) {
  thisAnnounceSubscribedTopics_cb = pAnnounceSubscribedTopics_cb;
}

static void request_time_sync_internal(const char* reason) {
  const unsigned long now = millis();
  if (now - lastTimeSyncRequestMs < 2000) return;
  lastTimeSyncRequestMs = now;

  const char* tz = "CET-1CEST,M3.5.0/2,M10.5.0/3";

  setenv("TZ", tz, 1);
  tzset();

  esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, kNtpServer1);
  if (esp_sntp_enabled()) {
    esp_sntp_restart();
  } else {
    configTzTime(tz, kNtpServer1);
  }

  bool should_log = (reason != nullptr && reason[0] != '\0');
  if (!should_log && (now - lastTimeSyncLogMs >= kTimeSyncLogIntervalMs)) {
    should_log = true;
  }
  if (should_log) {
    if (reason != nullptr && reason[0] != '\0') {
      Serial.printf("NTP sync requested (%s)\r\n", reason);
    } else {
      Serial.printf("NTP sync requested\r\n");
    }
    lastTimeSyncLogMs = now;
  }
}

static bool start_wifi_with_effective_credentials() {
  WiFi.mode(WIFI_STA);
  wifiShutdownRequested = false;
  lastWifiReconnectAttemptMs = millis();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  return true;
}

bool getIsWifiConnected_HAL() {
  return isWifiConnected;
}

void wifi_request_time_sync_HAL() {
  request_time_sync_internal("manual");
}

// WiFi status event
void WiFiEvent(WiFiEvent_t event){
  //Serial.printf("[WiFi-event] event: %d\r\n", event);
  if(event == ARDUINO_EVENT_WIFI_STA_GOT_IP){
    // connection to MQTT server will be done in checkMQTTconnection()
    // mqttClient.setServer(MQTT_SERVER, 1883); // MQTT initialization
    // mqttClient.connect("OMOTE"); // Connect using a client id
    isWifiConnected = true;
    wifiShutdownRequested = false;
    mqttReconnectIntervalMs = kMqttReconnectBaseIntervalMs;
    if (thisAnnounceWiFiconnected_cb != NULL) {
      thisAnnounceWiFiconnected_cb(true);
    }
    Serial.printf("WiFi connected, IP address: %s\r\n", WiFi.localIP().toString().c_str());
    ntpSyncPending = true;

  } else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
    isWifiConnected = false;
    if (thisAnnounceWiFiconnected_cb != NULL) {
      thisAnnounceWiFiconnected_cb(false);
    }
    if (wifiShutdownRequested || WiFi.getMode() == WIFI_OFF) {
      // Intentional disconnect during sleep/power down.
      return;
    }
    // automatically try to reconnect, throttled
    const unsigned long now = millis();
    if (now - lastWifiReconnectAttemptMs >= 3000) {
      Serial.printf("WiFi disconnected. Reconnecting...\r\n");
      start_wifi_with_effective_credentials();
    }

  } else {
    // e.g. ARDUINO_EVENT_WIFI_STA_CONNECTED or many others
    // connected is not enough, will wait for IP
    isWifiConnected = false;
    if (thisAnnounceWiFiconnected_cb != NULL) {
    thisAnnounceWiFiconnected_cb(false);
    }
  }
}

void init_mqtt_HAL(void) {
  // Setup WiFi
  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.onEvent(WiFiEvent);
  wifiShutdownRequested = false;
  mqttReconnectIntervalMs = kMqttReconnectBaseIntervalMs;
  lastReconnectAttempt = 0;
  start_wifi_with_effective_credentials();
  WiFi.setSleep(true);
}

std::string subscribeTopicOMOTEtest = "OMOTE/test";
// For connecting to one or several BLE clients
std::string subscribeTopicOMOTE_BLEstartAdvertisingForAll        = "OMOTE/BLE/startAdvertisingForAll";
std::string subscribeTopicOMOTE_BLEstartAdvertisingWithWhitelist = "OMOTE/BLE/startAdvertisingWithWhitelist";
std::string subscribeTopicOMOTE_BLEstartAdvertisingDirected      = "OMOTE/BLE/startAdvertisingDirected";
std::string subscribeTopicOMOTE_BLEstopAdvertising               = "OMOTE/BLE/stopAdvertising";
std::string subscribeTopicOMOTE_BLEprintConnectedClients         = "OMOTE/BLE/printConnectedClients";
std::string subscribeTopicOMOTE_BLEdisconnectAllClients          = "OMOTE/BLE/disconnectAllClients";
std::string subscribeTopicOMOTE_BLEprintBonds                    = "OMOTE/BLE/printBonds";
std::string subscribeTopicOMOTE_BLEdeleteBonds                   = "OMOTE/BLE/deleteBonds";

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  std::string topicReceived(topic);
  std::string strPayload(reinterpret_cast<const char *>(payload), length);
  Serial.printf("MQTT: received topic %s with payload %s\r\n", topicReceived.c_str(), strPayload.c_str());

  if (topicReceived == subscribeTopicOMOTEtest) {
    // Do whatever you want here, if it is ESP32 hardware related.
    // ...

    // Or forward the topic to "void receiveMQTTmessage_cb" in the "commandHandler.cpp", if it is not ESP32 hardware related
    if (thisAnnounceSubscribedTopics_cb != NULL) {
    thisAnnounceSubscribedTopics_cb(topicReceived, strPayload);
    }

  #if (ENABLE_KEYBOARD_BLE == 1)
  } else if (topicReceived == subscribeTopicOMOTE_BLEstartAdvertisingForAll) {
    keyboardBLE_startAdvertisingForAll_HAL();  
  } else if (topicReceived == subscribeTopicOMOTE_BLEstartAdvertisingWithWhitelist) {
    keyboardBLE_startAdvertisingWithWhitelist_HAL(strPayload);  
  } else if (topicReceived == subscribeTopicOMOTE_BLEstartAdvertisingDirected) {
    // the payload are two values, separated by comma: peerAddress and isRandomAddress 
    std::stringstream ss(strPayload);
    if (ss.good())  {
      std::string peerAddress;
      std::getline(ss, peerAddress, ',');
      
      if (ss.good())  {
        std::string isRandomAddressStr;
        std::getline(ss, isRandomAddressStr, ',');
        bool isRandomAddress = false;
        if (isRandomAddressStr == "true") {
          isRandomAddress = true;  
        }
        keyboardBLE_startAdvertisingDirected_HAL(peerAddress, isRandomAddress);  
      }
    }
  } else if (topicReceived == subscribeTopicOMOTE_BLEstopAdvertising) {
    keyboardBLE_stopAdvertising_HAL();  
  } else if (topicReceived == subscribeTopicOMOTE_BLEprintConnectedClients) {
    keyboardBLE_printConnectedClients_HAL();
  } else if (topicReceived == subscribeTopicOMOTE_BLEdisconnectAllClients) {
    keyboardBLE_disconnectAllClients_HAL();  
  } else if (topicReceived == subscribeTopicOMOTE_BLEprintBonds) {
    keyboardBLE_printBonds_HAL();  
  } else if (topicReceived == subscribeTopicOMOTE_BLEdeleteBonds) {
    keyboardBLE_deleteBonds_HAL();  
  #endif

  } else {
    // forward all other topics to the commandHandler
    if (thisAnnounceSubscribedTopics_cb != NULL) {
    thisAnnounceSubscribedTopics_cb(topicReceived, strPayload);
    }
  }
}

void mqtt_subscribeTopics() {
  mqttClient.setCallback(&callback);

  mqttClient.subscribe(subscribeTopicOMOTEtest.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEstartAdvertisingForAll.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEstartAdvertisingWithWhitelist.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEstartAdvertisingDirected.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEstopAdvertising.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEprintConnectedClients.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEdisconnectAllClients.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEprintBonds.c_str());
  mqttClient.subscribe(subscribeTopicOMOTE_BLEdeleteBonds.c_str());
  Serial.printf("  Successfully subscribed to MQTT topics\r\n");

}

bool checkMQTTconnection() {
  if (WiFi.getMode() == WIFI_OFF || wifiShutdownRequested) {
    return false;
  }
  if (!WiFi.isConnected()) {
    return false;
  }
  if (mqttClient.connected()) {
    return true;
  }

  // try to connect to mqtt server
  mqttClient.setBufferSize(512);   // default is 256
  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT); // MQTT initialization

  std::string mqttClientName = std::string(MQTT_CLIENTNAME) + "_esp32_" + std::string(WiFi.macAddress().c_str());
  if (mqttClient.connect(mqttClientName.c_str(), MQTT_USER, MQTT_PASS)) {
    Serial.printf("  Successfully connected to MQTT broker\r\n");
    mqtt_subscribeTopics();
    mqttReconnectIntervalMs = kMqttReconnectBaseIntervalMs;
    return true;
  }

  // Exponential backoff (capped) and throttled logging to prevent error spam.
  mqttReconnectIntervalMs = std::min(mqttReconnectIntervalMs * 2, kMqttReconnectMaxIntervalMs);
  return false;
}

void mqtt_loop_HAL() {
  if (wifiShutdownRequested || WiFi.getMode() == WIFI_OFF || !WiFi.isConnected()) {
    return;
  }

  if (ntpSyncPending) {
    ntpSyncPending = false;
    request_time_sync_internal("wifi connected");
  }

  if (!mqttClient.connected()) {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastReconnectAttempt) > mqttReconnectIntervalMs) {
      lastReconnectAttempt = currentMillis;
      // Attempt to reconnect
      checkMQTTconnection();
    }
  }  

  if (mqttClient.connected()) {
    mqttClient.loop();
  }
}

bool publishMQTTMessage_HAL(const char *topic, const char *payload){
  if (wifiShutdownRequested || WiFi.getMode() == WIFI_OFF) {
    Serial.printf("MQTT publish skipped: WiFi is off\r\n");
    return false;
  }
  const char* safe_topic = (topic != nullptr) ? topic : "";
  const char* safe_payload = (payload != nullptr) ? payload : "";
  if (checkMQTTconnection()) {
    Serial.printf("MQTT publish attempt: topic=\"%s\" payload=\"%s\"\r\n", safe_topic, safe_payload);
      
    if (mqttClient.publish(safe_topic, safe_payload)) {
      Serial.printf("MQTT publish ok: %s\r\n", safe_topic);
      return true;
    }
  }
  return false;
}

void wifi_shutdown_HAL() {
  wifiShutdownRequested = true;
  isWifiConnected = false;
  if (thisAnnounceWiFiconnected_cb != NULL) {
    thisAnnounceWiFiconnected_cb(false);
  }
  if (mqttClient.connected()) {
    mqttClient.disconnect();
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

#endif
