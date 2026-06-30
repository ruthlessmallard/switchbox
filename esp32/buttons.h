#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "config.h"

enum ButtonAction {
    ACTION_NONE,
    ACTION_SHORT,
    ACTION_LONG,
    ACTION_REPEAT
};

struct ButtonState {
    uint8_t pin;
    bool pressed;
    bool wasPressed;
    unsigned long pressStart;
    unsigned long lastRepeat;
    bool longFired;  // Prevent multiple long-press events
};

class Buttons {
public:
    Buttons();
    void begin();
    
    // Call this every loop iteration
    // Returns ACTION_NONE if nothing happened
    // If action is SHORT or LONG, id is filled with button index (1-3)
    ButtonAction update(uint8_t &id);
    
private:
    ButtonState states[3];
    
    void readButton(uint8_t idx);
};

#endif
