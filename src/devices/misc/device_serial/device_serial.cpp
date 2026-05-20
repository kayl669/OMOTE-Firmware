#include "device_serial.h"
#include "../../../applicationInternal/commandHandler.h"
#include "../../../applicationInternal/hardware/hardwarePresenter.h"
#include "../../../applicationInternal/scenes/sceneRegistry.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <vector>
#if (ENABLE_WIFI_AND_MQTT == 1)
#include <mutex>
#endif

void serialIOInit();
bool serialIOReadLine(char* buf, size_t maxLen);

// commands come from the serial port or by telnet. Their output goes back to where they came from.
static bool outputToTelnet = false;

static void commandPrintf(const char* format, ...) {
    char shortText[128];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(shortText, sizeof(shortText), format, args);
    va_end(args);
    if (length < 0) return;

    // e.g. the list of the BLE bonds can be longer
    std::vector<char> longText;
    const char* text = shortText;
    if ((size_t)length >= sizeof(shortText)) {
        longText.resize(length + 1);
        va_start(args, format);
        vsnprintf(longText.data(), longText.size(), format, args);
        va_end(args);
        text = longText.data();
    }

#if (ENABLE_WIFI_AND_MQTT == 1)
    if (outputToTelnet) {
        telnet_write(text);
        return;
    }
#endif
    fputs(text, stdout);
}

#if (ENABLE_WIFI_AND_MQTT == 1)
// messages waiting to be shown in the telnet session. They can be added by other threads, e.g. by BLE
static std::mutex notificationMutex;
static std::string pendingNotifications;
static const size_t MAX_PENDING_NOTIFICATIONS = 1024;

void showOnTelnet(const std::string &message) {
    std::lock_guard<std::mutex> lock(notificationMutex);
    if (pendingNotifications.size() + message.size() >= MAX_PENDING_NOTIFICATIONS) return;
    if (!pendingNotifications.empty()) pendingNotifications += "\n";
    pendingNotifications += message;
}

static void flushTelnetNotifications() {
    std::string messages;
    {
        std::lock_guard<std::mutex> lock(notificationMutex);
        messages.swap(pendingNotifications);
    }
    if (!messages.empty()) telnet_notify(messages.c_str());
}
#endif

static int serialStrcasecmp(const char* a, const char* b) {
#ifdef _WIN32
    return _stricmp(a, b);
#else
    return strcasecmp(a, b);
#endif
}

static int serialStrncasecmp(const char* a, const char* b, size_t n) {
#ifdef _WIN32
    return _strnicmp(a, b, n);
#else
    return strncasecmp(a, b, n);
#endif
}

static void trimString(char* str) {
    int len = strlen(str);
    while (len > 0 && (str[len-1] == ' '  || str[len-1] == '\r' ||
                        str[len-1] == '\n' || str[len-1] == '\t')) {
        str[--len] = '\0';
    }
    char* start = str;
    while (*start == ' ' || *start == '\t') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
}

static void printHelp() {
    // laid out like the help of a shell command:
    // UPPERCASE = value to be replaced, [ ] = optional, | = one of the choices
    commandPrintf("Usage: COMMAND [ARGUMENTS]\n");
    commandPrintf("\n");
    commandPrintf("Activities and commands:\n");
    commandPrintf("  scene [list]                  list the activities, * marks the active one\n");
    commandPrintf("  scene NAME [force]            start an activity, force: even if active\n");
    commandPrintf("  cmd [list [TEXT]]             list the commands, or those containing TEXT\n");
    commandPrintf("  cmd NAME [PAYLOAD]            send a command, PAYLOAD: MQTT and BLE only\n");
#if(OMOTE_HARDWARE_REV >= 5)
    commandPrintf("\n");
    commandPrintf("Keyboard backlight:\n");
    commandPrintf("  bl                            show the brightness\n");
    commandPrintf("  bl on|off|PERCENT             set the brightness, PERCENT: 0..100\n");
    commandPrintf("  bl+, bl-                      increase or decrease the brightness by 10%%\n");
    commandPrintf("  bl timeout [0..6]             show or set the sleep timeout:\n");
    commandPrintf("                                10s, 20s, 40s, 1m, 3m, 10m, 1h\n");
    commandPrintf("  bl threshold [0..2]           show or set the motion threshold:\n");
    commandPrintf("                                low, mid, high\n");
    commandPrintf("  bl lift [0|1]                 show or set lift to wake\n");
    commandPrintf("\n");
    commandPrintf("Battery blink with the keyboard backlight:\n");
    commandPrintf("  blink [reset]                 show the settings, or restore the defaults\n");
    commandPrintf("  blink STATE PERCENT           battery level, 0..100, 0 = off     [10, 95]\n");
    commandPrintf("  blink STATE time SECONDS      duration, 0..3600, 0 = no limit      [5, 0]\n");
    commandPrintf("  blink STATE speed MS          on and off time, 50..10000      [250, 2000]\n");
    commandPrintf("  blink STATE light PERCENT     brightness, 1..100                [100, 25]\n");
    commandPrintf("  STATE is one of (defaults are given in this order):\n");
    commandPrintf("    low                         picked up with a battery below the level\n");
    commandPrintf("    charge                      charging, until the level is reached\n");
#endif
#if (ENABLE_KEYBOARD_BLE == 1)
    commandPrintf("\n");
    commandPrintf("Bluetooth:\n");
    commandPrintf("  ble status                    show the connection state and the bonds\n");
    commandPrintf("  ble list                      list the BLE devices\n");
    commandPrintf("  ble advertise start|stop      start or stop advertising\n");
    commandPrintf("  ble connect ADDRESS           advertise to one bonded device only\n");
    commandPrintf("  ble disconnect                disconnect the devices\n");
    commandPrintf("  ble bonds list                list the bonds\n");
    commandPrintf("  ble bonds delete all|INDEX    delete all bonds, or only one\n");
#endif
    commandPrintf("\n");
    commandPrintf("IR receiver:\n");
    commandPrintf("  ir on|off                     print the received IR codes, or stop\n");
    commandPrintf("  ir status                     show the state of the receiver\n");
    commandPrintf("\n");
    commandPrintf("Battery:\n");
    commandPrintf("  battery                       show the voltage, level and charge state\n");
    commandPrintf("\n");
    commandPrintf("General:\n");
    commandPrintf("  help                          display this help\n");
#if !defined(ARDUINO)
    commandPrintf("  exit, quit                    quit the simulator\n");
#elif (ENABLE_WIFI_AND_MQTT == 1)
    commandPrintf("  exit, quit                    close the telnet session\n");
#endif
}

#if(OMOTE_HARDWARE_REV >= 5)
static bool isBacklightCommand(const char* line) {
    return (serialStrcasecmp(line, "bl") == 0 ||
            serialStrcasecmp(line, "bl on") == 0 ||
            serialStrcasecmp(line, "bl off") == 0 ||
            serialStrcasecmp(line, "bl+") == 0 ||
            serialStrcasecmp(line, "bl-") == 0 ||
            serialStrcasecmp(line, "bl timeout") == 0 ||
            serialStrncasecmp(line, "bl timeout ", 11) == 0 ||
            serialStrcasecmp(line, "bl threshold") == 0 ||
            serialStrncasecmp(line, "bl threshold ", 13) == 0 ||
            serialStrcasecmp(line, "bl lift") == 0 ||
            serialStrncasecmp(line, "bl lift ", 8) == 0 ||
            serialStrncasecmp(line, "bl ", 3) == 0);
}

static void processBacklight(const char* line) {
    uint8_t current = get_keyboardBrightness();

    if (serialStrcasecmp(line, "bl") == 0) {
        commandPrintf("Backlight: %d%%\n", current * 100 / 255);
    } else if (serialStrcasecmp(line, "bl on") == 0) {
        set_keyboardBrightness(255);
        commandPrintf("Backlight: 100%%\n");
    } else if (serialStrcasecmp(line, "bl off") == 0) {
        set_keyboardBrightness(0);
        commandPrintf("Backlight: eteint\n");
    } else if (serialStrcasecmp(line, "bl+") == 0) {
        uint8_t next = (current + 25 > 255) ? 255 : current + 25;
        set_keyboardBrightness(next);
        commandPrintf("Backlight: %d%%\n", next * 100 / 255);
    } else if (serialStrcasecmp(line, "bl-") == 0) {
        uint8_t next = (current < 25) ? 0 : current - 25;
        set_keyboardBrightness(next);
        commandPrintf("Backlight: %d%%\n", next * 100 / 255);
    } else if (serialStrcasecmp(line, "bl timeout") == 0) {
        switch (get_sleepTimeout()) {
            case   10000: {commandPrintf("Timeout: 10s\n"); break;}
            case   20000: {commandPrintf("Timeout: 20s\n"); break;}
            case   40000: {commandPrintf("Timeout: 40s\n"); break;}
            case   60000: {commandPrintf("Timeout: 1m\n"); break;}
            case  180000: {commandPrintf("Timeout: 3m\n"); break;}
            case  600000: {commandPrintf("Timeout: 10m\n"); break;}
            case 3600000: {commandPrintf("Timeout: 1h\n"); break;}
        }
    } else if (serialStrncasecmp(line, "bl timeout ", 11) == 0) {
        int selected = atoi(line + 11);
        switch (selected) {
            case 0: {set_sleepTimeout(  10000); break;}
            case 1: {set_sleepTimeout(  20000); break;}
            case 2: {set_sleepTimeout(  40000); break;}
            case 3: {set_sleepTimeout(  60000); break;}
            case 4: {set_sleepTimeout( 180000); break;}
            case 5: {set_sleepTimeout( 600000); break;}
            case 6: {set_sleepTimeout(3600000); break;}
        }
        commandPrintf("New timeout: %lu ms\n", get_sleepTimeout());
    } else if (serialStrcasecmp(line, "bl threshold") == 0) {
        switch (get_motionThreshold()) {
            case 120: {commandPrintf("Motion threshold: low\n"); break;}
            case  80: {commandPrintf("Motion threshold: mid\n"); break;}
            case  50: {commandPrintf("Motion threshold: high\n"); break;}
        }
    } else if (serialStrncasecmp(line, "bl threshold ", 13) == 0) {
        int selected = atoi(line + 13);
        switch (selected) {
            case 0: {set_motionThreshold(120); break;}
            case 1: {set_motionThreshold( 80); break;}
            case 2: {set_motionThreshold( 50); break;}
        }
        commandPrintf("New motion threshold: %u\n", get_motionThreshold());
    } else if (serialStrcasecmp(line, "bl lift") == 0) {
        commandPrintf("Lift to wake: %d\n", get_wakeupByIMUEnabled());
    } else if (serialStrncasecmp(line, "bl lift ", 8) == 0) {
        int selected = atoi(line + 8);
        set_wakeupByIMUEnabled(selected);
        commandPrintf("New lift to wake: %d\n", get_wakeupByIMUEnabled());
    } else if (serialStrncasecmp(line, "bl ", 3) == 0) {
        int pct = atoi(line + 3);
        if (pct < 0)   pct = 0;
        if (pct > 100) pct = 100;
        uint8_t val = (uint8_t)(pct * 255 / 100);
        set_keyboardBrightness(val);
        commandPrintf("Backlight: %d%%\n", pct);
    } else {
        commandPrintf("Unknown line: %s\n", line);
        return;
    }
    save_preferences();
}
#endif

static bool isBleCommand(const char* line) {
    return (serialStrncasecmp(line, "ble ", 4) == 0 ||
            serialStrcasecmp(line,  "ble list") == 0 ||
            serialStrcasecmp(line,  "ble status") == 0);
}

static void processBle(const char* line) {
#if (ENABLE_KEYBOARD_BLE == 1)
    if (serialStrcasecmp(line, "ble status") == 0) {
        commandPrintf("[ble] Connected    : %s\n", keyboardBLE_isConnected() ? "yes" : "no");
        commandPrintf("[ble] Bonds        : %s\n", keyboardBLE_getBonds().c_str());
        return;
    }
    if (serialStrcasecmp(line, "ble list") == 0) {
        commandPrintf("%s\n", keyboardBLE_printDevices().c_str());
        return;
    }
#endif
    const char* sub = line + 4;

#if (ENABLE_KEYBOARD_BLE == 1)
    if (serialStrcasecmp(sub, "advertise start") == 0) {
        commandPrintf("[ble] Start advertising...\n");
        keyboardBLE_startAdvertisingForAll();
        return;
    }

    if (serialStrcasecmp(sub, "advertise stop") == 0) {
        commandPrintf("[ble] Stop advertising\n");
        keyboardBLE_stopAdvertising();
        return;
    }

    if (serialStrcasecmp(sub, "disconnect") == 0) {
        keyboardBLE_disconnectAllClients();
        return;
    }

    // directed advertising, only works for an already bonded peer. See "ble bonds list" for the addresses
    if (serialStrncasecmp(sub, "connect ", 8) == 0) {
        keyboardBLE_startAdvertisingDirected(sub + 8, false);
        return;
    }

    if (serialStrcasecmp(sub, "bonds list") == 0) {
        commandPrintf("%s\n", keyboardBLE_printBonds().c_str());
        return;
    }

    if (serialStrcasecmp(sub, "bonds delete all") == 0) {
        commandPrintf("[ble] Delete bonds...\n");
        keyboardBLE_deleteBonds();
        return;
    }

    if (serialStrncasecmp(sub, "bonds delete ", 13) == 0) {
        int idx = atoi(sub + 13);
        keyboardBLE_deleteBond(idx);
        return;
    }
#endif
    commandPrintf("[ble] Unknown command : '%s' (type 'help')\n", sub);
}

#if(OMOTE_HARDWARE_REV >= 5)
// The settings of the battery signals shown with the keyboard backlight.
// Each signal has the same parameters, so the commands are "blink <signal> [<parameter>] <value>"
struct BlinkSignal {
    const char* name;
    BlinkSettings BacklightSignalSettings::*settings;
};
static const BlinkSignal blinkSignals[] = {
    {"low",    &BacklightSignalSettings::low},
    {"charge", &BacklightSignalSettings::charge},
};

struct BlinkParameter {
    const char* suffix; // appended to "blink <signal>"
    uint16_t BlinkSettings::*value;
    long minValue;
    long maxValue;
    const char* description;
};
static const BlinkParameter blinkParameters[] = {
    {"",       &BlinkSettings::batteryPercent,  0,   100, "battery level in %, 0 = off"},
    {" time",  &BlinkSettings::seconds,         0,  3600, "duration in s, 0 = no limit"},
    {" speed", &BlinkSettings::phaseMs,        50, 10000, "on and off time in ms"},
    {" light", &BlinkSettings::lightPercent,    1,   100, "brightness in %"},
};

static bool isBlinkCommand(const char* line) {
    return (serialStrcasecmp(line, "blink") == 0 ||
            serialStrncasecmp(line, "blink ", 6) == 0);
}

static void printBlinkSettings() {
    BacklightSignalSettings settings = get_backlightSignalSettings();
    for (const BlinkSignal &signal : blinkSignals) {
        for (const BlinkParameter &parameter : blinkParameters) {
            std::string command = std::string("blink ") + signal.name + parameter.suffix;
            commandPrintf("[blink] %-18s %5u   (%s)\n", command.c_str(), (settings.*signal.settings).*parameter.value, parameter.description);
        }
    }
}

static void processBlink(const char* line) {
    if (serialStrcasecmp(line, "blink") == 0) {
        printBlinkSettings();
        return;
    }

    if (serialStrcasecmp(line, "blink reset") == 0) {
        set_backlightSignalSettings(BacklightSignalSettings());
        save_preferences();
        printBlinkSettings();
        return;
    }

    for (const BlinkSignal &signal : blinkSignals) {
        for (const BlinkParameter &parameter : blinkParameters) {
            std::string command = std::string("blink ") + signal.name + parameter.suffix;
            size_t length = command.length();
            if (serialStrncasecmp(line, command.c_str(), length) != 0 || line[length] != ' ') continue;

            // the rest of the line has to be a number, otherwise it is another command starting with the same words
            const char* argument = line + length + 1;
            char* end;
            long value = strtol(argument, &end, 10);
            if (end == argument || *end != '\0') continue;

            if (value < parameter.minValue || value > parameter.maxValue) {
                commandPrintf("[blink] Value has to be between %ld and %ld\n", parameter.minValue, parameter.maxValue);
                return;
            }
            BacklightSignalSettings settings = get_backlightSignalSettings();
            (settings.*signal.settings).*parameter.value = value;
            set_backlightSignalSettings(settings);
            save_preferences();
            commandPrintf("[blink] New value of '%s': %ld\n", command.c_str(), value);
            return;
        }
    }

    commandPrintf("[blink] Unknown command : '%s' (type 'help')\n", line);
}
#endif

static bool isIrCommand(const char* line) {
    return (serialStrcasecmp(line, "ir on") == 0 ||
            serialStrcasecmp(line, "ir off") == 0 ||
            serialStrcasecmp(line, "ir status") == 0);
}

// the IR receiver decodes the codes of other remotes and prints them on the serial output
static void processIr(const char* line) {
    if (serialStrcasecmp(line, "ir on") == 0) {
        if (!get_irReceiverEnabled()) {
            set_irReceiverEnabled(true);
            start_infraredReceiver();
        }
    } else if (serialStrcasecmp(line, "ir off") == 0) {
        if (get_irReceiverEnabled()) {
            set_irReceiverEnabled(false);
            shutdown_infraredReceiver();
        }
    }
    commandPrintf("[ir] Receiver     : %s\n", get_irReceiverEnabled() ? "on" : "off");
}

static bool isBatteryCommand(const char* line) {
    return (serialStrcasecmp(line, "battery") == 0);
}

static void processBattery() {
    int voltage = 0; // in mV
    int percentage = 0;
    bool isCharging = false;
    get_battery_status(&voltage, &percentage, &isCharging);

    commandPrintf("[battery] Voltage    : %d.%03d V\n", voltage / 1000, voltage % 1000);
    commandPrintf("[battery] Level      : %d%%\n", percentage);
#if(OMOTE_HARDWARE_REV >= 4)
    commandPrintf("[battery] Charging   : %s\n", isCharging ? "yes" : "no");
#else
    // hardware rev 3 and older cannot recognize the charge state in a reliable way
    commandPrintf("[battery] Charging   : not available\n");
#endif
}

static bool isSceneCommand(const char* line) {
    return (serialStrcasecmp(line, "scene") == 0 ||
            serialStrncasecmp(line, "scene ", 6) == 0);
}

static const char* skipSpaces(const char* text) {
    while (*text == ' ' || *text == '\t') text++;
    return text;
}

static void printScenes() {
    std::string activeScene = gui_memoryOptimizer_getActiveSceneName();
    for (const auto &scene : registered_scenes) {
        commandPrintf("[scene] %c %s\n", scene.first == activeScene ? '*' : ' ', scene.first.c_str());
    }
}

// returns the name of the scene as registered, or an empty string if it is unknown
static std::string findScene(const std::string &name) {
    for (const auto &scene : registered_scenes) {
        if (serialStrcasecmp(scene.first.c_str(), name.c_str()) == 0) return scene.first;
    }
    return "";
}

static void processScene(const char* line) {
    const char* argument = skipSpaces(line + 5);
    if (*argument == '\0' || serialStrcasecmp(argument, "list") == 0) {
        printScenes();
        return;
    }

    // the names can contain spaces, so the whole rest of the line is the name, optionally followed by "force"
    std::string name = argument;
    std::string sceneName = findScene(name);
    bool force = false;
    if (sceneName.empty() && name.size() > 6 && serialStrcasecmp(name.c_str() + name.size() - 6, " force") == 0) {
        name.erase(name.find_last_not_of(" \t", name.size() - 6) + 1);
        sceneName = findScene(name);
        force = true;
    }

    uint16_t command = sceneName.empty() ? 0 : get_activate_scene_command(sceneName);
    if (command == 0) {
        commandPrintf("[scene] Unknown activity : '%s' (type 'scene')\n", argument);
        return;
    }

    if (!force && sceneName == gui_memoryOptimizer_getActiveSceneName()) {
        commandPrintf("[scene] '%s' is already active, use 'scene %s force' to start it again\n", sceneName.c_str(), sceneName.c_str());
        return;
    }

    executeCommand(command, force ? "FORCE" : "");
    commandPrintf("[scene] Active activity: %s\n", gui_memoryOptimizer_getActiveSceneName().c_str());
}

static bool isCmdCommand(const char* line) {
    return (serialStrcasecmp(line, "cmd") == 0 ||
            serialStrncasecmp(line, "cmd ", 4) == 0);
}

static void printCommandNames(const char* filter) {
    size_t filterLength = strlen(filter);
    int count = 0;
    for (const auto &command : get_command_names()) {
        const std::string &name = command.first;
        bool matches = (filterLength == 0);
        for (size_t i = 0; !matches && i + filterLength <= name.size(); i++) {
            matches = (serialStrncasecmp(name.c_str() + i, filter, filterLength) == 0);
        }
        if (!matches) continue;
        commandPrintf("[cmd] %s\n", name.c_str());
        count++;
    }
    commandPrintf("[cmd] %d command(s)\n", count);
}

static void processCmd(const char* line) {
    const char* argument = skipSpaces(line + 3);
    if (*argument == '\0' || serialStrcasecmp(argument, "list") == 0) {
        printCommandNames("");
        return;
    }
    if (serialStrncasecmp(argument, "list ", 5) == 0) {
        printCommandNames(skipSpaces(argument + 5));
        return;
    }

    // "<command> [payload]". The payload is the whole rest of the line and can contain spaces
    size_t nameLength = strcspn(argument, " \t");
    std::string name(argument, nameLength);
    std::string payload = skipSpaces(argument + nameLength);

    for (const auto &command : get_command_names()) {
        if (serialStrcasecmp(command.first.c_str(), name.c_str()) != 0) continue;

        commandPayloadModes payloadMode = get_command_payload_mode(command.second);
        if (payload.empty() && payloadMode == PAYLOAD_REQUIRED) {
            commandPrintf("[cmd] %s needs a payload\n", command.first.c_str());
        } else if (!payload.empty() && payloadMode == PAYLOAD_NOT_ALLOWED) {
            commandPrintf("[cmd] %s does not accept a payload\n", command.first.c_str());
        } else {
            executeCommand(command.second, payload);
            commandPrintf("[cmd] Executed: %s\n", command.first.c_str());
        }
        return;
    }

    commandPrintf("[cmd] Unknown command : '%s' (type 'cmd list')\n", name.c_str());
}

static bool isExitCommand(const char* line) {
    return (serialStrcasecmp(line, "exit") == 0 || serialStrcasecmp(line, "quit") == 0);
}

static void processExit() {
#if (ENABLE_WIFI_AND_MQTT == 1)
    if (outputToTelnet) {
        telnet_write("Bye\n");
        telnet_close();
        return;
    }
#endif
#if defined(ARDUINO)
    // the serial port has no session which could be closed by the device
    commandPrintf("Nothing to close on the serial port, quit your serial monitor instead.\n");
#else
    commandPrintf("Bye\n");
    fflush(stdout);
    // not exit(): its cleanup of the streams can block, because the thread reading the console holds stdin
    _Exit(0);
#endif
}

static void processLine(char* line) {
    trimString(line);
    if (strlen(line) == 0) return;

    // do not go to sleep while serial commands are being used
    setLastActivityTimestamp();

    if (isExitCommand(line)) {
        processExit();
        return;
    }

    if (serialStrcasecmp(line, "help") == 0) {
        printHelp();
        return;
    }

    if (isSceneCommand(line)) {
        processScene(line);
        return;
    }

    if (isCmdCommand(line)) {
        processCmd(line);
        return;
    }

    if (isIrCommand(line)) {
        processIr(line);
        return;
    }

    if (isBatteryCommand(line)) {
        processBattery();
        return;
    }

#if(OMOTE_HARDWARE_REV >= 5)
    if (isBacklightCommand(line)) {
        processBacklight(line);
        return;
    }

    if (isBlinkCommand(line)) {
        processBlink(line);
        return;
    }
#endif

    if (isBleCommand(line)) {
        processBle(line);
        return;
    }

    commandPrintf("Unknown command: '%s' (type 'help')\n", line);
}

void register_device_serial() {
    serialIOInit();
    commandPrintf("Ready. Type 'help' to list commands.\n");
}

void handleSerialInput() {
    char buf[128];
    if (serialIOReadLine(buf, sizeof(buf))) {
        processLine(buf);
    }

#if (ENABLE_WIFI_AND_MQTT == 1)
    telnet_loop();
    if (telnet_readLine(buf, sizeof(buf))) {
        outputToTelnet = true;
        processLine(buf);
        outputToTelnet = false;
        telnet_endOfCommand();
    }
    flushTelnetNotifications();
#endif
}
