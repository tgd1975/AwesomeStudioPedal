#include "button_controller.h"

ButtonController::ButtonController(uint8_t pin) : pin(pin) {}

void ButtonController::setup() {
    pinMode(pin, INPUT_PULLUP);
}

bool ButtonController::read() {
    return digitalRead(pin) == LOW;  // Active low due to pull-up
}
