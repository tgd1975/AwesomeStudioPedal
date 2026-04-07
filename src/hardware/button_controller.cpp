#include "hardware/button_controller.h"

ButtonController::ButtonController(gpio_num_t pin) : pin(pin) {}

void ButtonController::setup() {
    gpio_pad_select_gpio(pin);
    pinMode(pin, INPUT_PULLUP);
}

bool ButtonController::read() {
    return digitalRead(pin) == LOW;  // Active low due to pull-up
}EOF