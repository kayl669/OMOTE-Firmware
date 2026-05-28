/*
  If you add additional overrides here, you have to
  1. first add #undef
  2. add new #define
*/
#undef WIFI_SSID
#undef WIFI_PASSWORD
#undef WIFI_HOSTNAME
#undef MQTT_SERVER

#define WIFI_SSID                  "YourWifiSSID"              // override here
#define WIFI_PASSWORD              "YourWifiPassword"          // override here
#define WIFI_HOSTNAME              "YourHostName"              // override here
#define MQTT_SERVER                "IPAddressOfYourBroker"     // override here
