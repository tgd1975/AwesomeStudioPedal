#include "button.h"
#include <Arduino.h>

Button::Button(uint8_t PIN) : PIN(PIN) {}

void Button::setup() { pinMode(PIN, INPUT_PULLUP); }

bool Button::isDebounced()
{
    unsigned long now = millis();
    bool accepted = (now - lastDebounceTime) > debounceDelay;
    lastDebounceTime = now;
    return accepted;
}

void Button::isr()
{
    if (digitalRead(PIN) == HIGH)
    {
        awaitingRelease = false;
        return;
    }
    if (! awaitingRelease && isDebounced())
    {
        pressCount++;
        awaitingRelease = true;
    }
}

bool Button::event()
{
    if (pressCount > 0)
    {
        pressCount--;
        return true;
    }
    return false;
}

void Button::reset()
{
    pressCount = 0;
    awaitingRelease = false;
}
