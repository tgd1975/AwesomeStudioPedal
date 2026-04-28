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

void LEDController::startBlink(uint32_t intervalMs, int16_t count)
{
    if (blinking)
        return;
    stateBeforeBlink = currentState;
    blinkInterval = intervalMs;
    blinkRemaining = (count < 0) ? -1 : static_cast<int16_t>(count * 2);
    blinking = true;
    lastToggleTime = 0;
}

void LEDController::stopBlink()
{
    if (! blinking)
        return;
    blinking = false;
    setState(stateBeforeBlink);
}

void LEDController::update(uint32_t now)
{
    if (! blinking)
        return;
    if (lastToggleTime == 0)
    {
        lastToggleTime = now;
        setState(true);
        return;
    }
    if (now - lastToggleTime < blinkInterval)
        return;
    lastToggleTime = now;
    toggle();
    if (blinkRemaining > 0)
    {
        blinkRemaining--;
        if (blinkRemaining == 0)
            stopBlink();
    }
}
