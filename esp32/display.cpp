#include "display.h"

// GC9A01 command codes
#define GC9A01_NOP     0x00
#define GC9A01_SWRESET 0x01
#define GC9A01_SLPIN   0x10
#define GC9A01_SLPOUT  0x11
#define GC9A01_PTLON   0x12
#define GC9A01_NORON   0x13
#define GC9A01_INVOFF  0x20
#define GC9A01_INVON   0x21
#define GC9A01_DISPOFF 0x28
#define GC9A01_DISPON  0x29
#define GC9A01_CASET   0x2A
#define GC9A01_RASET   0x2B
#define GC9A01_RAMWR   0x2C
#define GC9A01_COLMOD  0x3A
#define GC9A01_MADCTL  0x36

Display::Display() {
    currentColor = DEFAULT_COLOR;
    marqueeBuffer[0] = '\0';
    marqueeOffset = 0;
    lastMarqueeStep = 0;
}

void Display::begin() {
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    pinMode(TFT_BL, OUTPUT);
    
    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TFT_BL, LOW);  // Backlight off during init
    
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    SPI.setFrequency(40000000);  // 40MHz
    
    gc9a01Init();
    setBrightness(80);
    
    clear(0x0000);
    drawStatus("SWITCHBOX");
    refresh();
}

void Display::gc9a01Init() {
    digitalWrite(TFT_RST, LOW);
    delay(10);
    digitalWrite(TFT_RST, HIGH);
    delay(120);
    
    // Simplified init sequence - real one is longer
    // You'll want to use TFT_eSPI or LovyanGFX library for production
    // This is a stub for structure
    
    // Command: SLPOUT (wake up)
    // Command: COLMOD (16-bit color)
    // Command: MADCTL (orientation)
    // Command: DISPON (display on)
    
    // TODO: Implement full init or use library
}

void Display::setBrightness(uint8_t percent) {
    uint8_t duty = map(percent, 0, 100, 0, 255);
    analogWrite(TFT_BL, duty);
}

void Display::clear(uint16_t color) {
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
}

void Display::drawText(const char *text, int16_t x, int16_t y, uint16_t color, uint8_t size) {
    // TODO: Implement bitmap font rendering
    // For now, stub
}

void Display::drawCenteredText(const char *text, int16_t y, uint16_t color, uint8_t size) {
    // TODO: Center calculation + draw
}

void Display::drawTime(const char *timeStr) {
    // Top margin, centered
    drawCenteredText(timeStr, 20, currentColor, 3);
}

void Display::drawTrack(const char *trackText) {
    // Middle area - triggers marquee
    setMarqueeText(trackText);
}

void Display::drawSticky(const char *stickyText, uint8_t secondsRemaining) {
    // Bottom area
    char buf[64];
    snprintf(buf, sizeof(buf), "%s (%ds)", stickyText, secondsRemaining);
    drawCenteredText(buf, 200, currentColor, 2);
}

void Display::drawStatus(const char *status) {
    // Big centered status text
    clear(0x0000);
    drawCenteredText(status, SCREEN_HEIGHT / 2 - 10, currentColor, 3);
    refresh();
}

void Display::setMarqueeText(const char *text) {
    strncpy(marqueeBuffer, text, sizeof(marqueeBuffer) - 1);
    marqueeBuffer[sizeof(marqueeBuffer) - 1] = '\0';
    marqueeOffset = 0;
}

void Display::marqueeStep() {
    if (marqueeBuffer[0] == '\0') return;
    
    unsigned long now = millis();
    if (now - lastMarqueeStep < DISPLAY_MARQUEE_SPEED) return;
    lastMarqueeStep = now;
    
    // TODO: Render scrolling text at y=110 (middle of screen)
    // advance marqueeOffset, wrap around
    // text is drawn at (x - marqueeOffset, y)
    // when x < -textWidth, offset resets
}

void Display::refresh() {
    // For buffered libraries, this would push to screen
    // For direct SPI, changes are immediate
}

void Display::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    // GC9A01 set column and row address, then write
    // TODO: SPI command sequence
}

void Display::writePixel(uint16_t color) {
    // TODO: SPI push 16-bit color
}

void Display::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // TODO: Set addr window, burst write color
}
