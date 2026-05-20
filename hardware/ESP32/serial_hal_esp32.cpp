#include <Arduino.h>
#include "line_editor_hal_esp32.h"

static LineEditor serialEditor(Serial);

void serialIOInit() {
    printf("Waiting for commands. Type 'help'.\n");
}

bool serialIOReadLine(char* buf, size_t maxLen) {
    while (Serial.available()) {
        char c = Serial.read();
        if (serialEditor.process(c, buf, maxLen) == LineEditor::LINE_ENTERED) {
            return true;
        }
    }
    return false;
}
