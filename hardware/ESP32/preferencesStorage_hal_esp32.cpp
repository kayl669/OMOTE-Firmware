#include <Preferences.h>
#include "preferencesStorage_hal_esp32.h"
#include "sleep_hal_esp32.h"
#include "tft_hal_esp32.h"
#include "keypad_keys_hal_esp32.h"

Preferences preferences;

std::string activeScene;
std::string activeGUIname;
int activeGUIlist;
int activeTab;
int lastActiveGUIlistIndex;
BacklightSignalSettings backlightSignalSettings;

void init_preferences_HAL(void) {
  // Restore settings from internal flash memory
  preferences.begin("settings", false);
  if (preferences.getBool("alreadySetUp")) {
    // from sleep.h
    set_wakeupByIMUEnabled_HAL(preferences.getBool("wkpByIMU"));
    set_sleepTimeout_HAL(preferences.getUInt("slpTimeout", DEFAULT_SLEEP_TIMEOUT));
    set_motionThreshold_HAL(preferences.getUInt("motionThreshold", DEFAULT_MOTION_THRESHOLD));
    // from tft.h
    set_backlightBrightness_HAL(preferences.getUInt("blBrightness", 255));
    // from keyboard.h
    #if(OMOTE_HARDWARE_REV >= 5)
    set_keyboardBrightness_HAL(preferences.getUInt("kbBrightness", 255));
    #endif
    // from backlightSignal.h. If a value was never saved, the default is kept
    BacklightSignalSettings &s = backlightSignalSettings;
    s.low.batteryPercent    = preferences.getUInt("sigLowPct",   s.low.batteryPercent);
    s.low.seconds           = preferences.getUInt("sigLowSec",   s.low.seconds);
    s.low.phaseMs           = preferences.getUInt("sigLowMs",    s.low.phaseMs);
    s.low.lightPercent      = preferences.getUInt("sigLowLight", s.low.lightPercent);
    s.charge.batteryPercent = preferences.getUInt("sigChgPct",   s.charge.batteryPercent);
    s.charge.seconds        = preferences.getUInt("sigChgSec",   s.charge.seconds);
    s.charge.phaseMs        = preferences.getUInt("sigChgMs",    s.charge.phaseMs);
    s.charge.lightPercent   = preferences.getUInt("sigChgLight", s.charge.lightPercent);
    // from here
    activeScene = std::string(preferences.getString("currentScene").c_str());
    activeGUIname = std::string(preferences.getString("currentGUIname").c_str());
    activeGUIlist =(preferences.getInt("currentGUIlist"));
    activeTab = (preferences.getInt("activeTab"));
    lastActiveGUIlistIndex = (preferences.getInt("lastActiveIndex"));

    // Serial.printf("Preferences restored: blBrightness %d, kbBrightness %d, GUI %s, scene %s\r\n", get_backlightBrightness_HAL(), get_keyboardBrightness_HAL(), activeGUIname.c_str(), activeScene.c_str());
  } else {
    // Serial.printf("No preferences to restore\r\n");
  }
  preferences.end();
}

void save_preferences_HAL(void) {
  preferences.begin("settings", false);
  // from sleep.h
  preferences.putBool("wkpByIMU", get_wakeupByIMUEnabled_HAL());
  // from tft.h
  preferences.putUInt("slpTimeout", get_sleepTimeout_HAL());
  preferences.putUInt("motionThreshold", get_motionThreshold_HAL());
  preferences.putUInt("blBrightness", get_backlightBrightness_HAL());
  // from keyboard.h
  #if(OMOTE_HARDWARE_REV >= 5)
  preferences.putUInt("kbBrightness", get_keyboardBrightness_HAL());
  // Serial.printf("Preferences saved: blBrightness %d, kbBrightness %d, GUI %s, scene %s\r\n", get_backlightBrightness_HAL(), get_keyboardBrightness_HAL(), activeGUIname.c_str(), activeScene.c_str());
  #endif
  // from backlightSignal.h
  preferences.putUInt("sigLowPct",   backlightSignalSettings.low.batteryPercent);
  preferences.putUInt("sigLowSec",   backlightSignalSettings.low.seconds);
  preferences.putUInt("sigLowMs",    backlightSignalSettings.low.phaseMs);
  preferences.putUInt("sigLowLight", backlightSignalSettings.low.lightPercent);
  preferences.putUInt("sigChgPct",   backlightSignalSettings.charge.batteryPercent);
  preferences.putUInt("sigChgSec",   backlightSignalSettings.charge.seconds);
  preferences.putUInt("sigChgMs",    backlightSignalSettings.charge.phaseMs);
  preferences.putUInt("sigChgLight", backlightSignalSettings.charge.lightPercent);
  // from here
  preferences.putString("currentScene", activeScene.c_str());
  preferences.putString("currentGUIname", activeGUIname.c_str());
  preferences.putInt("currentGUIlist", activeGUIlist);
  preferences.putInt("activeTab", activeTab);
  preferences.putInt("lastActiveIndex", lastActiveGUIlistIndex);
  if (!preferences.getBool("alreadySetUp")) {
    preferences.putBool("alreadySetUp", true);
  }
  preferences.end();
}

BacklightSignalSettings get_backlightSignalSettings_HAL() {
  return backlightSignalSettings;
}
void set_backlightSignalSettings_HAL(BacklightSignalSettings aSettings) {
  backlightSignalSettings = aSettings;
}
std::string get_activeScene_HAL() {
  return activeScene;
}
void set_activeScene_HAL(std::string anActiveScene) {
  activeScene = anActiveScene;
}
std::string get_activeGUIname_HAL(){
  return activeGUIname;
}
void set_activeGUIname_HAL(std::string anActiveGUIname) {
  activeGUIname = anActiveGUIname;
}
int get_activeGUIlist_HAL() {
  return activeGUIlist;
}
void set_activeGUIlist_HAL(int anActiveGUIlist) {
  activeGUIlist = anActiveGUIlist;
}
int get_activeTab_HAL() {
  return activeTab;
}
void set_activeTab_HAL(int anActiveTab) {
  activeTab = anActiveTab;
}
int get_lastActiveGUIlistIndex_HAL() {
  return lastActiveGUIlistIndex;
}
void set_lastActiveGUIlistIndex_HAL(int aGUIlistIndex) {
  lastActiveGUIlistIndex = aGUIlistIndex;
}
