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
    bool accepted = (now - lastDebounceTime) > debounceDelay;
    lastDebounceTime = now;
    return accepted;
}

void Button::isr() {
#ifndef HOST_TEST_BUILD
    if (digitalRead(PIN) == HIGH) {
        awaitingRelease = false;
        return;
    }
    if (!awaitingRelease && isDebounced()) {
        pressCount++;
        awaitingRelease = true;
    }
#else
    if (isDebounced()) {
        pressCount++;
    }
#endif
}

bool Button::event() {
    if (pressCount > 0) {
        pressCount--;
        return true;
    }
    return false;
}

void Button::reset() {
    pressCount = 0;
    awaitingRelease = false;
}
