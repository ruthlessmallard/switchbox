/*
  SwitchBox ESP32 Firmware
  Post-apocalypse edition: works offline, zero cloud, maximum spite
  
  Hardware: ESP32-S3-WROOM-1 N16R8 + GC9A01 display + 3 Sanwa buttons
  Protocol: UDP over WiFi soft-AP
  
  Button map:
    1 short: Audible play
    1 long:  Global play/pause
    2 short: YouTube Music play
    2 long:  Skip back 30s
    3 short: End call
    3 long:  Gemini activate
*/

#include "config.h"
#include "buttons.h"
#include "display.h"
#include "network.h"

// State machines
Buttons buttons;
Display display;
Network network;

// Display state
char currentTime[16] = "--:--";
char currentTrack[128] = "";
char stickyText[32] = "";
unsigned long stickyExpiry = 0;

// JSON parsing buffer
char udpBuffer[UDP_BUFFER_SIZE];

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== SwitchBox ESP32 ===");
    Serial.println("Initializing...");
    
    // Init subsystems
    buttons.begin();
    display.begin();
    network.begin();
    
    // Boot screen
    display.drawStatus("SWITCHBOX\nMINE READY");
    delay(1500);
    
    Serial.println("Ready for buttons");
}

void loop() {
    unsigned long now = millis();
    
    // === BUTTON HANDLING ===
    uint8_t buttonId = 0;
    ButtonAction action = buttons.update(buttonId);
    
    if (action != ACTION_NONE && buttonId > 0) {
        handleButton(buttonId, action);
    }
    
    // === NETWORK ===
    network.update();
    
    // Check for display data from phone
    if (network.receivePacket(udpBuffer, sizeof(udpBuffer))) {
        parsePhonePacket(udpBuffer);
    }
    
    // === DISPLAY UPDATE ===
    // Update sticky timeout
    if (stickyExpiry > 0 && now > stickyExpiry) {
        stickyText[0] = '\0';
        stickyExpiry = 0;
    }
    
    // Step marquee
    display.marqueeStep();
    
    // Refresh display at ~10fps (no need for 60fps on a gauge)
    static unsigned long lastDisplayUpdate = 0;
    if (now - lastDisplayUpdate > 100) {
        lastDisplayUpdate = now;
        updateDisplay();
    }
    
    // Small delay to prevent busy-wait
    delay(5);
}

void handleButton(uint8_t buttonId, ButtonAction action) {
    const char *actionStr = (action == ACTION_LONG) ? "long" : "short";
    
    Serial.print("Button ");
    Serial.print(buttonId);
    Serial.print(" ");
    Serial.println(actionStr);
    
    // Send to phone
    network.sendButtonEvent(buttonId, actionStr);
    
    // Local display feedback
    const char *label = "BUTTON";
    switch (buttonId) {
        case 1: label = (action == ACTION_LONG) ? "PLAY/PAUSE" : "AUDIBLE"; break;
        case 2: label = (action == ACTION_LONG) ? "SKIP -30S" : "YOUTUBE"; break;
        case 3: label = (action == ACTION_LONG) ? "GEMINI" : "END CALL"; break;
    }
    
    setSticky(label, DISPLAY_STICKY_S);
}

void setSticky(const char *text, uint8_t seconds) {
    strncpy(stickyText, text, sizeof(stickyText) - 1);
    stickyText[sizeof(stickyText) - 1] = '\0';
    stickyExpiry = millis() + (seconds * 1000);
}

void parsePhonePacket(const char *json) {
    // Minimal JSON parser - look for key patterns
    // Production: use ArduinoJSON library
    
    // Check for display packet
    if (strstr(json, "\"type\":\"display\"") != nullptr) {
        // Extract time
        const char *timeKey = "\"time\":\"";
        char *timeStart = strstr(json, timeKey);
        if (timeStart != nullptr) {
            timeStart += strlen(timeKey);
            char *timeEnd = strchr(timeStart, '"');
            if (timeEnd != nullptr) {
                size_t len = min((size_t)(timeEnd - timeStart), sizeof(currentTime) - 1);
                strncpy(currentTime, timeStart, len);
                currentTime[len] = '\0';
            }
        }
        
        // Extract track
        const char *trackKey = "\"track\":\"";
        char *trackStart = strstr(json, trackKey);
        if (trackStart != nullptr) {
            trackStart += strlen(trackKey);
            char *trackEnd = strchr(trackStart, '"');
            if (trackEnd != nullptr) {
                size_t len = min((size_t)(trackEnd - trackStart), sizeof(currentTrack) - 1);
                strncpy(currentTrack, trackStart, len);
                currentTrack[len] = '\0';
                display.setMarqueeText(currentTrack);
            }
        }
        
        // Extract sticky (optional override from phone)
        const char *stickyKey = "\"sticky\":\"";
        char *stickyStart = strstr(json, stickyKey);
        if (stickyStart != nullptr) {
            stickyStart += strlen(stickyKey);
            char *stickyEnd = strchr(stickyStart, '"');
            if (stickyEnd != nullptr) {
                size_t len = min((size_t)(stickyEnd - stickyStart), sizeof(stickyText) - 1);
                strncpy(stickyText, stickyStart, len);
                stickyText[len] = '\0';
                
                // Check for TTL
                const char *ttlKey = "\"sticky_ttl\":";
                char *ttlStart = strstr(json, ttlKey);
                if (ttlStart != nullptr) {
                    ttlStart += strlen(ttlKey);
                    uint8_t ttl = atoi(ttlStart);
                    stickyExpiry = millis() + (ttl * 1000);
                }
            }
        }
    }
}

void updateDisplay() {
    // Layout:
    // Line 1: Time (top, centered)
    // Line 2: Marquee track (middle)
    // Line 3: Sticky button feedback (bottom, fades after timeout)
    
    // Only redraw if something changed... for now, always redraw
    // TODO: Add dirty flags for optimization
    
    display.clear(0x0000);
    display.drawTime(currentTime);
    
    // Draw marquee at y=100
    // TODO: Actually render marquee graphics
    
    // Draw sticky if active
    if (stickyText[0] != '\0' && millis() < stickyExpiry) {
        uint8_t remaining = (stickyExpiry - millis()) / 1000;
        display.drawSticky(stickyText, remaining);
    }
    
    display.refresh();
}
