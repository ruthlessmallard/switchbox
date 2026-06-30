// SwitchBox ESP32 Configuration
// Post-apocalypse edition: robust, simple, zero cloud

#ifndef CONFIG_H
#define CONFIG_H

// ===== HARDWARE PINS =====
// ESP32-S3-WROOM-1 (N16R8)

// Buttons (GPIO 1-3, internal pullup)
#define BUTTON_1_PIN    1
#define BUTTON_2_PIN    2
#define BUTTON_3_PIN    3

// Display (GC9A01, SPI)
#define TFT_DC          9
#define TFT_CS          10
#define TFT_MOSI        11
#define TFT_SCLK        12
#define TFT_BL          13   // Backlight PWM
#define TFT_RST         14

// Ambient Light Sensor (BH1750, I2C) - v0.2
#define ALS_SDA         6
#define ALS_SCL         7

// WS2812B Button LEDs - v0.2
#define LED_DATA        8

// ===== NETWORK CONFIG =====
// ESP32 runs as soft-AP, phone connects to it
#define AP_SSID         "SwitchBox"
#define AP_PASSWORD     "mine1234"  // Change me, or don't
#define AP_CHANNEL      1
#define AP_MAX_CLIENTS  1

// UDP communication
#define UDP_PORT        4210
#define UDP_BUFFER_SIZE 256

// ===== TIMING =====
#define BUTTON_DEBOUNCE_MS      20
#define BUTTON_SHORT_MAX_MS     750     // <750ms = short press
#define BUTTON_LONG_MIN_MS      750     // >750ms = long press
#define BUTTON_REPEAT_MS        100     // Repeat rate during hold

#define DISPLAY_STICKY_S        10      // Button feedback display time
#define DISPLAY_MARQUEE_SPEED   150     // ms per frame

#define WIFI_RETRY_MS           5000    // Reconnect interval
#define UDP_HEARTBEAT_MS        3000    // Keepalive to phone

// ===== DISPLAY =====
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   240
#define DISPLAY_ROTATION 0

// Default amber color (RGB565)
#define DEFAULT_COLOR   0xFD20  // Warm amber

// ===== DISPLAY CONFIGURATION =====
// Brightness offset range (-50 to +50 percent)
#define BRIGHTNESS_OFFSET_MIN   -50
#define BRIGHTNESS_OFFSET_MAX   50
#define BRIGHTNESS_OFFSET_DEFAULT 0

// Default background color for truck dash matching (RGB565)
// Amber #FFAA00 in RGB565 = 0xFD20 (matches DEFAULT_COLOR)
#define DEFAULT_BG_COLOR_AMBER  0xFD20  // #FFAA00 - Classic truck dash amber
#define DEFAULT_BG_COLOR_BLUE   0x041F  // #0066FF - Cool blue
#define DEFAULT_BG_COLOR_RED    0xF800  // #CC0000 - Warning red
#define DEFAULT_BG_COLOR_GREEN  0x05E0  // #00AA00 - Status green
#define DEFAULT_BG_COLOR_WHITE  0xFFFF  // #FFFFFF - Full white
#define DEFAULT_BG_COLOR_BLACK  0x0000  // #000000 - Black/off

// Default background color (amber for truck dash matching)
#define DEFAULT_BG_COLOR        DEFAULT_BG_COLOR_AMBER

// ===== PROTOCOL =====
// JSON messages over UDP
// Button -> Phone: {"type":"button","id":1,"action":"short"}
// Phone -> Display: {"type":"display","time":"17:56","track":"...","sticky":"AUDIBLE","sticky_ttl":8}

#endif
