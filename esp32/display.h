#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <SPI.h>
#include "config.h"

// Minimal GC9A01 driver - we'll use a library or write basic commands
// For now, struct definition

class Display {
public:
    Display();
    void begin();
    
    // Screen management
    void clear(uint16_t color = 0xFFFF);  // Uses background color by default
    void setBrightness(uint8_t percent);  // 0-100 base, offset applied internally
    
    // Configuration
    void setBrightnessOffset(int8_t offset);  // -50 to +50
    int8_t getBrightnessOffset() const;
    void setBackgroundColor(uint16_t rgb565);
    uint16_t getBackgroundColor() const;
    
    // Text rendering (basic, no font library yet)
    void drawText(const char *text, int16_t x, int16_t y, uint16_t color, uint8_t size = 2);
    void drawCenteredText(const char *text, int16_t y, uint16_t color, uint8_t size = 2);
    
    // Layout sections
    void drawTime(const char *timeStr);
    void drawTrack(const char *trackText);
    void drawSticky(const char *stickyText, uint8_t secondsRemaining);
    void drawStatus(const char *status);  // "NO LINK", etc.
    
    // Marquee
    void setMarqueeText(const char *text);
    void marqueeStep();  // Call regularly
    
    void refresh();  // Push to screen
    
private:
    void gc9a01Init();
    void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void writePixel(uint16_t color);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void applyBrightness();  // Apply current brightness with offset
    
    uint16_t currentColor;
    uint16_t backgroundColor;
    uint8_t baseBrightness;   // 0-100 base brightness
    int8_t brightnessOffset;  // -50 to +50 offset
    char marqueeBuffer[128];
    uint16_t marqueeOffset;
    unsigned long lastMarqueeStep;
};

#endif
