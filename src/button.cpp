#include "button.h"
#include <Arduino.h>

Button::Button(uint8_t PIN) {
    this->PIN = PIN;
}

bool Button::isDebounced() {
    unsigned long now = millis();
    if ((now - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = now;
        return true;
    }
    return false;
}

void Button::incKeyPresses() {
    numberKeyPresses++;
}

void Button::isr() {
    if (isDebounced()) {
        incKeyPresses();
        pressed = true;
    }
}

bool Button::event() {
    if (pressed) {
        pressed = false;
        return true;
    }
    return false;
}
