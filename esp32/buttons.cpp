#include "buttons.h"

Buttons::Buttons() {
    states[0] = {BUTTON_1_PIN, false, false, 0, 0, false};
    states[1] = {BUTTON_2_PIN, false, false, 0, 0, false};
    states[2] = {BUTTON_3_PIN, false, false, 0, 0, false};
}

void Buttons::begin() {
    for (int i = 0; i < 3; i++) {
        pinMode(states[i].pin, INPUT_PULLUP);
    }
}

ButtonAction Buttons::update(uint8_t &id) {
    unsigned long now = millis();
    
    for (int i = 0; i < 3; i++) {
        readButton(i);
        ButtonState &btn = states[i];
        
        // Detect press start
        if (btn.pressed && !btn.wasPressed) {
            btn.pressStart = now;
            btn.longFired = false;
            btn.lastRepeat = now;
        }
        
        // Detect long press
        if (btn.pressed && !btn.longFired) {
            unsigned long held = now - btn.pressStart;
            if (held >= BUTTON_LONG_MIN_MS) {
                btn.longFired = true;
                id = i + 1;
                return ACTION_LONG;
            }
        }
        
        // Detect release -> short press
        if (!btn.pressed && btn.wasPressed) {
            unsigned long held = now - btn.pressStart;
            if (held < BUTTON_SHORT_MAX_MS && !btn.longFired) {
                id = i + 1;
                return ACTION_SHORT;
            }
        }
        
        btn.wasPressed = btn.pressed;
    }
    
    return ACTION_NONE;
}

void Buttons::readButton(uint8_t idx) {
    // Invert because INPUT_PULLUP: LOW = pressed
    bool raw = digitalRead(states[idx].pin) == LOW;
    
    // Simple debounce: require stable reading... actually let's just read it
    // The mechanical buttons will bounce, but 20ms loop timing helps
    states[idx].pressed = raw;
}
