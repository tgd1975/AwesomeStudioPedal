#include "button.h"
#include <Arduino.h>
#ifndef HOST_TEST_BUILD
#include <driver/gpio.h>
#endif

Button::Button(uint8_t PIN) : PIN(PIN) {}

void Button::setup()
{
    gpio_pad_select_gpio(static_cast<gpio_num_t>(PIN));
    pinMode(PIN, INPUT_PULLUP);
}

bool Button::isDebounced(unsigned long now) const
{
    return (now - lastDebounceTime) > debounceDelay;
}

void Button::isr()
{
    unsigned long now = millis();
    if (digitalRead(PIN) == HIGH)
    {
        if (awaitingRelease && isDebounced(now))
        {
            awaitingRelease = false;
            released = true;
            lastDebounceTime = now;
        }
        return;
    }
    if (! awaitingRelease && isDebounced(now))
    {
        pressCount++;
        awaitingRelease = true;
        lastDebounceTime = now;
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
    released = false;
    lastDebounceTime = 0;
}

bool Button::releaseEvent()
{
    if (released)
    {
        released = false;
        return true;
    }
    return false;
}
