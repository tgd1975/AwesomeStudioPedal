#include "button.h"
#include <Arduino.h>

Button::Button(uint8_t PIN) : PIN(PIN) {}

void Button::setup() {
    pinMode(PIN, INPUT_PULLUP);
}

bool Button::isDebounced() {
    unsigned long now = millis();
    if ((now - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = now;
        return true;
    }
    return false;
}

void Button::isr() {
    if (isDebounced()) {
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

void Button::reset() {
    pressed = false;
}
