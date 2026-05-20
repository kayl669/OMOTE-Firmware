#include <Arduino.h>
#include <string.h>

static char    rxBuffer[128];
static uint8_t rxIndex = 0;

void serialIOInit() {
    printf("Waiting for commands. Type 'help'.\n");
}

bool serialIOReadLine(char* buf, size_t maxLen) {
    while (Serial.available()) {
        char c = Serial.read();
        Serial.print(c);
        if (c == '\n' || c == '\r') {
            if (rxIndex > 0) {
                rxBuffer[rxIndex] = '\0';
                strncpy(buf, rxBuffer, maxLen - 1);
                buf[maxLen - 1] = '\0';
                rxIndex = 0;
                return true;
            }
        } else if (rxIndex < sizeof(rxBuffer) - 1) {
            rxBuffer[rxIndex++] = c;
        }
    }
    return false;
}
