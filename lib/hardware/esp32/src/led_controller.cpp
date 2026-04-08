#include "led_controller.h"

LEDController::LEDController(gpio_num_t pin) : pin(pin) {}

void LEDController::setup(uint32_t initialState) {
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    setState(initialState);
}

void LEDController::setState(bool state) {
    if (currentState != state) {
        gpio_set_level(pin, state ? 1 : 0);
        currentState = state;
    }
}

void LEDController::toggle() {
    setState(!currentState);
}
