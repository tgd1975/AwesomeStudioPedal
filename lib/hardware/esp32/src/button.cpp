#include "button.h"
#include <Arduino.h>
#ifndef HOST_TEST_BUILD
#include <driver/gpio.h>
#endif

Button::Button(uint8_t PIN) : PIN(PIN) {}

void Button::setup() {
    gpio_pad_select_gpio(static_cast<gpio_num_t>(PIN));
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
