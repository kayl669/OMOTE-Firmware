#pragma once

#include <Arduino.h>

// Line editor for the command line: cursor keys, backspace, delete, home, end and a command history.
// To the terminal, only characters, spaces and backspaces are sent, no escape sequences. Not every serial
// monitor interprets them, e.g. the one of PlatformIO does not with its default settings.
class LineEditor {
public:
    enum Result {NOTHING, LINE_ENTERED, EMPTY_LINE_ENTERED};

    explicit LineEditor(Print &output) : out(output) {}

    // handles one received character. With LINE_ENTERED, the line has been copied to buf
    Result process(char c, char* buf, size_t maxLen);
    // prints the line being typed again, e.g. after a message was shown
    void redisplay();
    // forgets the line being typed and the history
    void reset();

private:
    static const size_t LINE_SIZE = 128;
    static const uint8_t HISTORY_SIZE = 10;
    // cursor keys are sent by the terminal as "ESC [ A", "ESC O A" or "ESC [ 3 ~"
    enum EscapeState {NO_ESCAPE, ESCAPE_STARTED, ESCAPE_SEQUENCE};

    Print &out;

    char    line[LINE_SIZE];
    uint8_t length = 0;
    uint8_t cursor = 0;

    // the history is kept in RAM, so it is lost when the device goes to sleep
    char    history[HISTORY_SIZE][LINE_SIZE];
    uint8_t historyCount = 0;
    uint8_t historyPos = 0;       // historyCount means: the new line being typed, not one from the history
    char    draftLine[LINE_SIZE]; // the new line being typed, saved while browsing the history

    EscapeState escapeState = NO_ESCAPE;
    unsigned int escapeParameter = 0;
    bool lastWasCarriageReturn = false;

    void moveCursorLeft();
    void moveCursorRight();
    void redrawFromCursor(int leftOver);
    void insertCharacter(char c);
    void deleteAtCursor();
    void replaceLine(const char* text);
    void showHistory(uint8_t position);
    void addToHistory(const char* text);
    void executeEscapeSequence(char finalCharacter);
    bool handleEscapeSequence(char c);
};
