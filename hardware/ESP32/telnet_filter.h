#pragma once

#include <stdint.h>

// removes the commands of the telnet protocol (RFC 854) from the received bytes
class TelnetFilter {
public:
    static const uint8_t SE   = 240; // end of a subnegotiation
    static const uint8_t SB   = 250; // start of a subnegotiation
    static const uint8_t WILL = 251;
    static const uint8_t DONT = 254; // WILL, WONT, DO and DONT are followed by an option byte
    static const uint8_t IAC  = 255; // "interpret as command"

    // returns true if the byte is part of the data typed by the user
    bool isData(uint8_t byte) {
        switch (state) {
            case DATA:
                if (byte != IAC) return true;
                state = COMMAND;
                return false;
            case COMMAND:
                if (byte == IAC) { // two IAC stand for one data byte with the value 255
                    state = DATA;
                    return true;
                }
                if (byte == SB) {
                    state = SUBNEGOTIATION;
                } else if (byte >= WILL && byte <= DONT) {
                    state = OPTION;
                } else {
                    state = DATA;
                }
                return false;
            case OPTION:
                state = DATA;
                return false;
            case SUBNEGOTIATION:
                if (byte == IAC) state = SUBNEGOTIATION_COMMAND;
                return false;
            case SUBNEGOTIATION_COMMAND:
                state = (byte == SE) ? DATA : SUBNEGOTIATION;
                return false;
        }
        return false;
    }

    void reset() {
        state = DATA;
    }

private:
    enum State {DATA, COMMAND, OPTION, SUBNEGOTIATION, SUBNEGOTIATION_COMMAND};
    State state = DATA;
};
