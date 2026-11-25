#include "ota_hal_esp32.h"

#if (ENABLE_OTA == 1)
#include <ESPmDNS.h>
#include <Update.h>
#include <WebServer.h>
#include <string.h>

#include "secrets.h"

static WebServer otaServer(3232);

// uploads need HTTP basic authentication with this user name and OTA_PASSWORD
static const char* OTA_USER = "omote";
static const unsigned long LOCKOUT_MS = 30000; // after too many wrong passwords
static const uint8_t MAX_WRONG_PASSWORDS = 3;

static tOtaStartCallback    s_start_cb    = NULL;
static tOtaProgressCallback s_progress_cb = NULL;

void set_ota_start_cb_HAL(tOtaStartCallback cb)    { s_start_cb    = cb; }
void set_ota_progress_cb_HAL(tOtaProgressCallback cb) { s_progress_cb = cb; }

static bool   enabled      = false;
static size_t ota_total    = 0;
static size_t ota_written  = 0;
static int    ota_last_pct = -1;
static bool   uploadAuthorized = false; // only then the received data is written to the flash
static bool   uploadSucceeded  = false;

static uint8_t       wrongPasswords = 0;
static bool          lockedOut      = false;
static unsigned long lockoutStart   = 0;

static bool isLockedOut() {
    if (lockedOut && millis() - lockoutStart >= LOCKOUT_MS) { lockedOut = false; }
    return lockedOut;
}

static void handleUpdatePost() {
    bool succeeded = uploadSucceeded;
    uploadSucceeded  = false;
    uploadAuthorized = false;

    otaServer.sendHeader("Connection", "close");
    if (isLockedOut()) {
        otaServer.send(429, "text/plain", "Too many wrong passwords, try again later");
        return;
    }
    if (!otaServer.authenticate(OTA_USER, OTA_PASSWORD)) {
        // a request without credentials is not a wrong password
        if (otaServer.hasHeader("Authorization")) {
            Serial.println("OTA: wrong password");
            wrongPasswords++;
            if (wrongPasswords >= MAX_WRONG_PASSWORDS) {
                wrongPasswords = 0;
                lockedOut = true;
                lockoutStart = millis();
            }
        }
        otaServer.requestAuthentication();
        return;
    }
    wrongPasswords = 0;

    if (!succeeded) {
        otaServer.send(200, "text/plain", "FAIL");
        if (s_progress_cb) { s_progress_cb(-1); }
    } else {
        otaServer.send(200, "text/plain", "OK");
        Serial.println("OTA: rebooting");
        delay(100);
        ESP.restart();
    }
}

static void handleUpdateUpload() {
    // The web server also calls this for requests which are not a file upload.
    // upload() must not be used then, it would crash.
    if (!otaServer.header("Content-Type").startsWith("multipart/")) { return; }

    HTTPUpload& upload = otaServer.upload();

    if (upload.status == UPLOAD_FILE_START) {
        uploadSucceeded  = false;
        uploadAuthorized = !isLockedOut() && otaServer.authenticate(OTA_USER, OTA_PASSWORD);
        if (!uploadAuthorized) {
            Serial.println("OTA: upload refused");
            return;
        }
        ota_written  = 0;
        ota_last_pct = -1;
        ota_total    = otaServer.header("Content-Length").toInt();
        Serial.printf("OTA: receiving, expected %u bytes\n", ota_total);
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
        if (s_start_cb) { s_start_cb(); }
        return;
    }

    if (!uploadAuthorized) { return; }

    if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
        ota_written += upload.currentSize;
        // Only fire the progress callback when the integer percent advances.
        if (s_progress_cb && ota_total > 0) {
            int pct = (int)((ota_written * 100) / ota_total);
            if (pct > 99) { pct = 99; } // reserve 100% for after Update.end()
            if (pct != ota_last_pct) {
                ota_last_pct = pct;
                s_progress_cb(pct);
            }
        }

    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            uploadSucceeded = true;
            Serial.printf("OTA: success, %u bytes written\n", upload.totalSize);
            if (s_progress_cb) { s_progress_cb(100); }
        } else {
            Update.printError(Serial);
        }

    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        // otherwise the next upload could not be started
        Update.abort();
        uploadAuthorized = false;
        if (s_progress_cb) { s_progress_cb(-1); }
    }
}

void init_ota_HAL(void) {
    // the host name is also used to reach the telnet server
    MDNS.begin(WIFI_HOSTNAME);
    if (strlen(OTA_PASSWORD) == 0) {
        Serial.println("OTA: disabled, because OTA_PASSWORD is not set");
        return;
    }
    const char* headerKeys[] = {"Content-Length", "Content-Type"};
    otaServer.collectHeaders(headerKeys, 2);
    MDNS.addService("http", "tcp", 3232);
    otaServer.on("/update", HTTP_POST, handleUpdatePost, handleUpdateUpload);
    otaServer.begin();
    enabled = true;
    Serial.printf("OTA: HTTP server ready on port 3232 (%s)\n", WIFI_HOSTNAME);
}

void ota_loop_HAL(void) {
    if (enabled) { otaServer.handleClient(); }
}

#endif
