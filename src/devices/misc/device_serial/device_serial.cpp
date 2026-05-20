#include "device_serial.h"
#include "../../../applicationInternal/commandHandler.h"
#include "../../../applicationInternal/hardware/hardwarePresenter.h"
#include <string.h>
#include <stdio.h>

void serialIOInit();
bool serialIOReadLine(char* buf, size_t maxLen);

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
    printf("\n");
    printf("|-----------------------------------------------|\n");
    printf("|      OMOTE - Serial commands                  |\n");
    printf("|-----------------------------------------------|\n");
#if(OMOTE_HARDWARE_REV >= 5)
    printf("| Backlight keyboard                            |\n");
    printf("|-----------------------------------------------|\n");
    printf("| bl                | Backlight                 |\n");
    printf("| bl on             | Backlight max             |\n");
    printf("| bl off            | Backlight off             |\n");
    printf("| bl+               | Light +10%%                |\n");
    printf("| bl-               | Light -10%%                |\n");
    printf("| bl 0..100         | Light in %%                |\n");
    printf("| bl timeout        | Timeout                   |\n");
    printf("| bl timeout 0..6   | Set timeout               |\n");
    printf("| bl threshold      | Motion threshold          |\n");
    printf("| bl threshold 0..2 | Set motion threshold      |\n");
    printf("| bl lift           | Lift to wake              |\n");
    printf("| bl lift 0..1      | Set lift to wake          |\n");
    printf("|-----------------------------------------------|\n");
#endif
#if (ENABLE_KEYBOARD_BLE == 1)
    printf("| BLE Bluetooth                                 |\n");
    printf("|-----------------------------------------------|\n");
    printf("| ble status        | State connection + bonds  |\n");
    printf("| ble list          | List BLE devices          |\n");
    printf("| ble advertise     | Start advertising         |\n");
    printf("|   start           |                           |\n");
    printf("| ble advertise     | Stop advertising          |\n");
    printf("|   stop            |                           |\n");
    printf("| ble disconnect    | Disconnect device         |\n");
    printf("| ble bonds list    | List inscriptions         |\n");
    printf("| ble bonds         | Delete all bonds          |\n");
    printf("|   delete all      |                           |\n");
    printf("| ble bonds         | Delete by index           |\n");
    printf("|   delete <N>      |                           |\n");
    printf("|-----------------------------------------------|\n");
#endif
    printf("| help              | Display this help         |\n");
    printf("|-----------------------------------------------|\n\n");
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
        printf("Backlight: %d%%\n", current * 100 / 255);
    } else if (serialStrcasecmp(line, "bl on") == 0) {
        set_keyboardBrightness(255);
        printf("Backlight: 100%%\n");
    } else if (serialStrcasecmp(line, "bl off") == 0) {
        set_keyboardBrightness(0);
        printf("Backlight: eteint\n");
    } else if (serialStrcasecmp(line, "bl+") == 0) {
        uint8_t next = (current + 25 > 255) ? 255 : current + 25;
        set_keyboardBrightness(next);
        printf("Backlight: %d%%\n", next * 100 / 255);
    } else if (serialStrcasecmp(line, "bl-") == 0) {
        uint8_t next = (current < 25) ? 0 : current - 25;
        set_keyboardBrightness(next);
        printf("Backlight: %d%%\n", next * 100 / 255);
    } else if (serialStrcasecmp(line, "bl timeout") == 0) {
        switch (get_sleepTimeout()) {
            case   10000: {printf("Timeout: 10s\n"); break;}
            case   20000: {printf("Timeout: 20s\n"); break;}
            case   40000: {printf("Timeout: 40s\n"); break;}
            case   60000: {printf("Timeout: 1m\n"); break;}
            case  180000: {printf("Timeout: 3m\n"); break;}
            case  600000: {printf("Timeout: 10m\n"); break;}
            case 3600000: {printf("Timeout: 1h\n"); break;}
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
        printf("New timeout: %lu ms\n", get_sleepTimeout());
    } else if (serialStrcasecmp(line, "bl threshold") == 0) {
        switch (get_motionThreshold()) {
            case 120: {printf("Motion threshold: low\n"); break;}
            case  80: {printf("Motion threshold: mid\n"); break;}
            case  50: {printf("Motion threshold: high\n"); break;}
        }
    } else if (serialStrncasecmp(line, "bl threshold ", 13) == 0) {
        int selected = atoi(line + 13);
        switch (selected) {
            case 0: {set_motionThreshold(120); break;}
            case 1: {set_motionThreshold( 80); break;}
            case 2: {set_motionThreshold( 50); break;}
        }
        printf("New motion threshold: %lu ms\n", get_motionThreshold());
    } else if (serialStrcasecmp(line, "bl lift") == 0) {
        printf("Lift to wake: %d\n", get_wakeupByIMUEnabled());
    } else if (serialStrncasecmp(line, "bl lift ", 8) == 0) {
        int selected = atoi(line + 8);
        set_wakeupByIMUEnabled(selected);
        printf("New lift to wake: %d\n", get_wakeupByIMUEnabled());
    } else if (serialStrncasecmp(line, "bl ", 3) == 0) {
        int pct = atoi(line + 3);
        if (pct < 0)   pct = 0;
        if (pct > 100) pct = 100;
        uint8_t val = (uint8_t)(pct * 255 / 100);
        set_keyboardBrightness(val);
        printf("Backlight: %d%%\n", pct);
    } else {
        printf("Unknown line: %s\n", line);
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
        printf("[ble] Connected    : %s\n", keyboardBLE_isConnected() ? "yes" : "no");
        printf("[ble] Bonds        : %s\n", keyboardBLE_getBonds().c_str());
        return;
    }
    if (serialStrcasecmp(line, "ble list") == 0) {
        printf("%s\n", keyboardBLE_printDevices().c_str());
        return;
    }
#endif
    const char* sub = line + 4;

#if (ENABLE_KEYBOARD_BLE == 1)
    if (serialStrcasecmp(sub, "advertise start") == 0) {
        printf("[ble] Start advertising...\n");
        keyboardBLE_startAdvertisingForAll();
        return;
    }

    if (serialStrcasecmp(sub, "advertise stop") == 0) {
        printf("[ble] Stop advertising\n");
        keyboardBLE_stopAdvertising();
        return;
    }

    if (serialStrcasecmp(sub, "disconnect") == 0) {
        keyboardBLE_disconnectAllClients();
        return;
    }

    if (serialStrcasecmp(sub, "bonds list") == 0) {
        printf("%s\n", keyboardBLE_printBonds().c_str());
        return;
    }

    if (serialStrcasecmp(sub, "bonds delete all") == 0) {
        printf("[ble] Delete bonds...\n");
        keyboardBLE_deleteBonds();
        return;
    }

    if (serialStrncasecmp(sub, "bonds delete ", 13) == 0) {
        int idx = atoi(sub + 13);
        keyboardBLE_deleteBond(idx);
        return;
    }
#endif
    printf("[ble] Unknown command : '%s' (type 'help')\n", sub);
}

static void processLine(char* line) {
    trimString(line);
    if (strlen(line) == 0) return;

    if (serialStrcasecmp(line, "help") == 0) {
        printHelp();
        return;
    }

#if(OMOTE_HARDWARE_REV >= 5)
    if (isBacklightCommand(line)) {
        processBacklight(line);
        return;
    }
#endif

    if (isBleCommand(line)) {
        processBle(line);
        return;
    }

    printf("Unknown command: '%s' (type 'help')\n", line);
}

void register_device_serial() {
    serialIOInit();
    printf("Ready. Type 'help' to list commands.\n");
}

void handleSerialInput() {
    char buf[128];
    if (serialIOReadLine(buf, sizeof(buf))) {
        processLine(buf);
    }
}
