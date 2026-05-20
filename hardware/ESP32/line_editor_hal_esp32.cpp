#include <string.h>
#include "line_editor_hal_esp32.h"

void LineEditor::moveCursorLeft() {
    if (cursor > 0) {
        cursor--;
        out.print('\b');
    }
}

void LineEditor::moveCursorRight() {
    if (cursor < length) {
        out.print(line[cursor]);
        cursor++;
    }
}

// prints the line from the cursor to its end, blanks the characters left over behind it and moves the cursor back
void LineEditor::redrawFromCursor(int leftOver) {
    for (int i = cursor; i < length; i++) out.print(line[i]);
    for (int i = 0; i < leftOver; i++) out.print(' ');
    for (int i = 0; i < length - cursor + leftOver; i++) out.print('\b');
}

void LineEditor::insertCharacter(char c) {
    if (length >= LINE_SIZE - 1) return;
    memmove(&line[cursor + 1], &line[cursor], length - cursor);
    line[cursor] = c;
    length++;
    moveCursorRight();
    redrawFromCursor(0);
}

void LineEditor::deleteAtCursor() {
    if (cursor >= length) return;
    memmove(&line[cursor], &line[cursor + 1], length - cursor - 1);
    length--;
    redrawFromCursor(1);
}

void LineEditor::replaceLine(const char* text) {
    while (cursor > 0) moveCursorLeft();
    int oldLength = length;
    length = strlen(text);
    memcpy(line, text, length);
    while (cursor < length) moveCursorRight();
    if (oldLength > length) redrawFromCursor(oldLength - length);
}

void LineEditor::showHistory(uint8_t position) {
    if (historyPos == historyCount) {
        memcpy(draftLine, line, length);
        draftLine[length] = '\0';
    }
    historyPos = position;
    replaceLine(position == historyCount ? draftLine : history[position]);
}

void LineEditor::addToHistory(const char* text) {
    // do not store the same command several times in a row
    if (historyCount > 0 && strcmp(history[historyCount - 1], text) == 0) return;
    if (historyCount == HISTORY_SIZE) {
        memmove(history[0], history[1], (HISTORY_SIZE - 1) * LINE_SIZE);
        historyCount--;
    }
    strcpy(history[historyCount], text);
    historyCount++;
}

void LineEditor::executeEscapeSequence(char finalCharacter) {
    switch (finalCharacter) {
        case 'A': if (historyPos > 0)            showHistory(historyPos - 1); break; // up
        case 'B': if (historyPos < historyCount) showHistory(historyPos + 1); break; // down
        case 'C': moveCursorRight(); break;
        case 'D': moveCursorLeft(); break;
        case 'H': while (cursor > 0) moveCursorLeft(); break;                        // home
        case 'F': while (cursor < length) moveCursorRight(); break;                  // end
        case '~':
            if (escapeParameter == 3) {
                deleteAtCursor();                                                    // delete
            } else if (escapeParameter == 1 || escapeParameter == 7) {
                executeEscapeSequence('H');
            } else if (escapeParameter == 4 || escapeParameter == 8) {
                executeEscapeSequence('F');
            }
            break;
    }
}

// returns true if the character belongs to an escape sequence
bool LineEditor::handleEscapeSequence(char c) {
    if (c == 0x1B) {
        escapeState = ESCAPE_STARTED;
        return true;
    }
    if (escapeState == ESCAPE_STARTED) {
        if (c == '[' || c == 'O') {
            escapeState = ESCAPE_SEQUENCE;
            escapeParameter = 0;
            return true;
        }
        // a single ESC, ignore it and handle the character as usual
        escapeState = NO_ESCAPE;
        return false;
    }
    if (escapeState == ESCAPE_SEQUENCE) {
        if (c >= '0' && c <= '9') {
            escapeParameter = escapeParameter * 10 + (c - '0');
        } else if (c >= 0x40 && c <= 0x7E) {
            // this character ends the sequence. Unknown sequences are ignored.
            escapeState = NO_ESCAPE;
            executeEscapeSequence(c);
        }
        return true;
    }
    return false;
}

LineEditor::Result LineEditor::process(char c, char* buf, size_t maxLen) {
    // some terminals send "\r\n" for the enter key, which counts only once
    bool isSecondPartOfNewline = (c == '\n' && lastWasCarriageReturn);
    lastWasCarriageReturn = (c == '\r');
    if (isSecondPartOfNewline) return NOTHING;

    if (handleEscapeSequence(c)) return NOTHING;

    if (c == '\n' || c == '\r') {
        out.print("\r\n");
        if (length == 0) {
            historyPos = historyCount;
            return EMPTY_LINE_ENTERED;
        }
        line[length] = '\0';
        strncpy(buf, line, maxLen - 1);
        buf[maxLen - 1] = '\0';
        addToHistory(line);
        length = 0;
        cursor = 0;
        historyPos = historyCount;
        return LINE_ENTERED;
    } else if (c == '\b' || c == 0x7F) {
        // backspace: depending on the terminal, either BS or DEL is sent
        if (cursor > 0) {
            moveCursorLeft();
            deleteAtCursor();
        }
    } else if (c == '\t') {
        insertCharacter(' ');
    } else if (c >= 0x20 && c < 0x7F) {
        insertCharacter(c);
    }
    return NOTHING;
}

void LineEditor::redisplay() {
    for (int i = 0; i < length; i++) out.print(line[i]);
    for (int i = cursor; i < length; i++) out.print('\b');
}

void LineEditor::reset() {
    length = 0;
    cursor = 0;
    historyCount = 0;
    historyPos = 0;
    escapeState = NO_ESCAPE;
    lastWasCarriageReturn = false;
}
