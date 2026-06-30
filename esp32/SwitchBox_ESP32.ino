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

#include <EEPROM.h>
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

// Function declarations
void handleButton(uint8_t buttonId, ButtonAction action);
void setSticky(const char *text, uint8_t seconds);
void parsePhonePacket(const char *json);
void parseDisplayPacket(const char *json);
void parseConfigPacket(const char *json);
void loadConfig();
void saveConfig();
void updateDisplay();

// Config persistence (EEPROM addresses)
#define EEPROM_ADDR_BRIGHTNESS_OFFSET 0
#define EEPROM_ADDR_BG_COLOR_HIGH     1
#define EEPROM_ADDR_BG_COLOR_LOW      2
#define EEPROM_ADDR_INITIALIZED       3
#define EEPROM_MAGIC_VALUE            0x42

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n=== SwitchBox ESP32 ===");
    Serial.println("Initializing...");
    
    // Load saved config
    loadConfig();
    
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
        parseDisplayPacket(json);
    }
    // Check for config packet
    else if (strstr(json, "\"type\":\"config\"") != nullptr) {
        parseConfigPacket(json);
    }
}

void parseDisplayPacket(const char *json) {
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

void parseConfigPacket(const char *json) {
    Serial.println("Config packet received");
    
    // Parse brightness_offset
    const char *brightnessKey = "\"brightness_offset\":";
    char *brightnessStart = strstr(json, brightnessKey);
    if (brightnessStart != nullptr) {
        brightnessStart += strlen(brightnessKey);
        int offset = atoi(brightnessStart);
        offset = constrain(offset, BRIGHTNESS_OFFSET_MIN, BRIGHTNESS_OFFSET_MAX);
        
        display.setBrightnessOffset((int8_t)offset);
        Serial.print("Brightness offset set to: ");
        Serial.println(offset);
        
        // Show feedback
        char feedback[32];
        snprintf(feedback, sizeof(feedback), "BRIGHTNESS %+d%%", offset);
        setSticky(feedback, 3);
        
        // Persist to EEPROM
        saveConfig();
    }
    
    // Parse bg_color (hex string like "#FFAA00" or "FFAA00")
    const char *colorKey = "\"bg_color\":\"";
    char *colorStart = strstr(json, colorKey);
    if (colorStart != nullptr) {
        colorStart += strlen(colorKey);
        
        // Skip # if present
        if (*colorStart == '#') {
            colorStart++;
        }
        
        // Parse 6 hex characters
        uint32_t rgb = 0;
        for (int i = 0; i < 6; i++) {
            char c = colorStart[i];
            uint8_t nibble = 0;
            if (c >= '0' && c <= '9') nibble = c - '0';
            else if (c >= 'A' && c <= 'F') nibble = c - 'A' + 10;
            else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
            else break;  // Invalid character
            
            rgb = (rgb << 4) | nibble;
        }
        
        // Convert RGB888 to RGB565
        uint8_t r = (rgb >> 16) & 0xFF;
        uint8_t g = (rgb >> 8) & 0xFF;
        uint8_t b = rgb & 0xFF;
        uint16_t rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        
        display.setBackgroundColor(rgb565);
        Serial.print("Background color set to: 0x");
        Serial.println(rgb565, HEX);
        
        // Show feedback with color name approximation
        const char* colorName = "CUSTOM";
        if (rgb565 == DEFAULT_BG_COLOR_AMBER) colorName = "AMBER";
        else if (rgb565 == DEFAULT_BG_COLOR_BLUE) colorName = "BLUE";
        else if (rgb565 == DEFAULT_BG_COLOR_RED) colorName = "RED";
        else if (rgb565 == DEFAULT_BG_COLOR_GREEN) colorName = "GREEN";
        else if (rgb565 == DEFAULT_BG_COLOR_WHITE) colorName = "WHITE";
        else if (rgb565 == DEFAULT_BG_COLOR_BLACK) colorName = "BLACK";
        
        char feedback[32];
        snprintf(feedback, sizeof(feedback), "COLOR: %s", colorName);
        setSticky(feedback, 3);
        
        // Clear display to show new color immediately
        display.clear();
        
        // Persist to EEPROM
        saveConfig();
    }
}

void loadConfig() {
    // Check if EEPROM has been initialized
    uint8_t initialized = EEPROM.read(EEPROM_ADDR_INITIALIZED);
    
    if (initialized == EEPROM_MAGIC_VALUE) {
        // Load saved values
        int8_t offset = (int8_t)EEPROM.read(EEPROM_ADDR_BRIGHTNESS_OFFSET);
        uint8_t bgHigh = EEPROM.read(EEPROM_ADDR_BG_COLOR_HIGH);
        uint8_t bgLow = EEPROM.read(EEPROM_ADDR_BG_COLOR_LOW);
        uint16_t bgColor = ((uint16_t)bgHigh << 8) | bgLow;
        
        // Apply to display (will be applied after begin())
        // Store temporarily - display not initialized yet
        // These will be overwritten by display.begin() defaults,
        // so we need to apply them after begin()
        
        Serial.print("Loaded config - offset: ");
        Serial.print(offset);
        Serial.print(", bg: 0x");
        Serial.println(bgColor, HEX);
    } else {
        Serial.println("No saved config found, using defaults");
    }
}

void saveConfig() {
    // Save current config to EEPROM
    EEPROM.write(EEPROM_ADDR_BRIGHTNESS_OFFSET, (uint8_t)display.getBrightnessOffset());
    uint16_t bgColor = display.getBackgroundColor();
    EEPROM.write(EEPROM_ADDR_BG_COLOR_HIGH, (uint8_t)(bgColor >> 8));
    EEPROM.write(EEPROM_ADDR_BG_COLOR_LOW, (uint8_t)(bgColor & 0xFF));
    EEPROM.write(EEPROM_ADDR_INITIALIZED, EEPROM_MAGIC_VALUE);
    
    Serial.println("Config saved to EEPROM");
}

void updateDisplay() {
    // Layout:
    // Line 1: Time (top, centered)
    // Line 2: Marquee track (middle)
    // Line 3: Sticky button feedback (bottom, fades after timeout)
    
    // Only redraw if something changed... for now, always redraw
    // TODO: Add dirty flags for optimization
    
    display.clear();
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
