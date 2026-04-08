#include "led_controller.h"

LEDController::LEDController(uint8_t pin) : pin(pin) {}

void LEDController::setup(uint32_t initialState)
{
    pinMode(pin, OUTPUT);
    setState(initialState != 0);
}

void LEDController::setState(bool state)
{
    if (currentState != state)
    {
        digitalWrite(pin, state ? HIGH : LOW);
        currentState = state;
    }
}

void LEDController::toggle() { setState(! currentState); }
