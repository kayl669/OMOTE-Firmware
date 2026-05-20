#include "telnet_hal_esp32.h"

#if (ENABLE_WIFI_AND_MQTT == 1)
#include <WiFi.h>
#include <string.h>
#include "line_editor_hal_esp32.h"
#include "telnet_filter.h"
#include "sleep_hal_esp32.h"
#include "secrets.h"

static const uint16_t TELNET_PORT = 23;
static const unsigned long LOGIN_TIMEOUT_MS = 30000;
static const unsigned long IDLE_TIMEOUT_MS = 5 * 60 * 1000; // an idle session would keep the device awake forever
static const unsigned long LOCKOUT_MS = 30000;              // after too many wrong passwords
static const uint8_t MAX_WRONG_PASSWORDS = 3;
static const char* PROMPT = "OMOTE> ";

static WiFiServer server(TELNET_PORT);
static WiFiClient client;

// collects the output, so that not every single character is sent in its own network packet
class TelnetOutput : public Print {
public:
    using Print::write;
    size_t write(uint8_t c) override {
        // telnet needs "\r\n" for a new line
        if (c == '\n' && lastCharacter != '\r') append('\r');
        append(c);
        lastCharacter = c;
        return 1;
    }
    void send() {
        if (length > 0 && client.connected()) client.write(buffer, length);
        length = 0;
    }
    void clear() {
        length = 0;
        lastCharacter = 0;
    }
private:
    uint8_t buffer[512];
    size_t length = 0;
    uint8_t lastCharacter = 0;
    void append(uint8_t c) {
        if (length == sizeof(buffer)) send();
        buffer[length++] = c;
    }
};

static TelnetOutput output;
static LineEditor editor(output);
static TelnetFilter filter;

enum SessionState {NOT_CONNECTED, LOGIN, LOGGED_IN};
static SessionState state = NOT_CONNECTED;
static bool enabled = false;

static char password[128];
static size_t passwordLength = 0;
static bool passwordTooLong = false;
static uint8_t wrongPasswords = 0; // counted over all connections, so that reconnecting does not reset it
static bool lockedOut = false;
static unsigned long lockoutStart = 0;

static unsigned long connectedSince = 0;
static unsigned long lastInput = 0;
static bool skipLineFeed = false;

void telnet_close_HAL(void) {
    if (state == NOT_CONNECTED) return;
    output.send();
    client.stop();
    state = NOT_CONNECTED;
    // the next session must not see the history of this one
    editor.reset();
    filter.reset();
    output.clear();
    memset(password, 0, sizeof(password));
    Serial.println("Telnet: connection closed");
}

static void closeWithMessage(const char* message) {
    output.print("\r\n");
    output.print(message);
    output.print("\r\n");
    telnet_close_HAL();
}

static void acceptClient(WiFiClient newClient) {
    client = newClient;
    state = LOGIN;
    editor.reset();
    filter.reset();
    output.clear();
    passwordLength = 0;
    passwordTooLong = false;
    skipLineFeed = false;
    connectedSince = millis();
    setLastActivityTimestamp_HAL();
    Serial.printf("Telnet: connection from %s\r\n", client.remoteIP().toString().c_str());

    // we echo the characters ourselves and want to receive each key at once, not only complete lines
    const uint8_t negotiation[] = {
        TelnetFilter::IAC, 251 /* WILL */, 1  /* echo */,
        TelnetFilter::IAC, 251 /* WILL */, 3  /* suppress go ahead */,
        TelnetFilter::IAC, 254 /* DONT */, 34 /* linemode */,
    };
    client.write(negotiation, sizeof(negotiation));
    output.print("OMOTE telnet\r\nPassword: ");
    output.send();
}

static void checkPassword() {
    password[passwordLength] = '\0';
    bool correct = !passwordTooLong && (strcmp(password, OTA_PASSWORD) == 0);
    memset(password, 0, sizeof(password));
    passwordLength = 0;
    passwordTooLong = false;

    if (correct) {
        wrongPasswords = 0;
        state = LOGGED_IN;
        lastInput = millis();
        output.print("\r\nType 'help' to list commands, 'exit' to close the session.\r\n");
        output.print(PROMPT);
        return;
    }

    Serial.println("Telnet: wrong password");
    wrongPasswords++;
    if (wrongPasswords >= MAX_WRONG_PASSWORDS) {
        wrongPasswords = 0;
        lockedOut = true;
        lockoutStart = millis();
        closeWithMessage("Access denied");
        return;
    }
    output.print("\r\nPassword: ");
}

// nothing is echoed while the password is typed
static void handleLoginCharacter(char c) {
    if (c == '\r' || c == '\n') {
        skipLineFeed = (c == '\r');
        checkPassword();
    } else if (c == '\b' || c == 0x7F) {
        if (passwordLength > 0) passwordLength--;
    } else if (c >= 0x20 && c < 0x7F) {
        if (passwordLength < sizeof(password) - 1) {
            password[passwordLength++] = c;
        } else {
            passwordTooLong = true;
        }
    }
}

void init_telnet_HAL(void) {
    if (strlen(OTA_PASSWORD) == 0) {
        Serial.println("Telnet: disabled, because OTA_PASSWORD is not set");
        return;
    }
    server.begin();
    server.setNoDelay(true);
    enabled = true;
    Serial.printf("Telnet: server ready on port %u\r\n", TELNET_PORT);
}

void telnet_loop_HAL(void) {
    if (!enabled) return;

    if (server.hasClient()) {
        WiFiClient newClient = server.available();
        if (lockedOut && millis() - lockoutStart >= LOCKOUT_MS) lockedOut = false;

        if (state != NOT_CONNECTED && client.connected()) {
            newClient.print("Another session is already active.\r\n");
            newClient.stop();
        } else if (lockedOut) {
            newClient.print("Too many wrong passwords, try again later.\r\n");
            newClient.stop();
        } else {
            telnet_close_HAL(); // the old connection is gone
            acceptClient(newClient);
        }
    }

    if (state == NOT_CONNECTED) return;

    if (!client.connected() && !client.available()) {
        telnet_close_HAL();
    } else if (state == LOGIN && millis() - connectedSince >= LOGIN_TIMEOUT_MS) {
        closeWithMessage("Login timeout");
    } else if (state == LOGGED_IN) {
        if (millis() - lastInput >= IDLE_TIMEOUT_MS) {
            closeWithMessage("Closed because of inactivity");
        } else {
            // do not go to sleep while a session is open
            setLastActivityTimestamp_HAL();
        }
    }
}

bool telnet_readLine_HAL(char* buf, size_t maxLen) {
    while (state != NOT_CONNECTED && client.available()) {
        int received = client.read();
        if (received < 0) break;
        if (!filter.isData(received)) continue;
        char c = received;

        // the second byte of the enter key which finished the password
        if (skipLineFeed) {
            skipLineFeed = false;
            if (c == '\n' || c == '\0') continue;
        }

        if (state == LOGIN) {
            handleLoginCharacter(c);
            continue;
        }

        lastInput = millis();
        LineEditor::Result result = editor.process(c, buf, maxLen);
        if (result == LineEditor::LINE_ENTERED) {
            output.send();
            return true;
        }
        if (result == LineEditor::EMPTY_LINE_ENTERED) output.print(PROMPT);
    }
    output.send();
    return false;
}

void telnet_endOfCommand_HAL(void) {
    if (state != LOGGED_IN) return;
    output.print(PROMPT);
    output.send();
}

void telnet_write_HAL(const char* text) {
    if (state != LOGGED_IN) return;
    output.print(text);
}

void telnet_notify_HAL(const char* text) {
    if (state != LOGGED_IN) return;
    output.print("\r\n");
    output.print(text);
    output.print("\r\n");
    output.print(PROMPT);
    editor.redisplay();
    output.send();
    // receiving messages, e.g. with "ir on", also counts as using the session
    lastInput = millis();
}

#endif
