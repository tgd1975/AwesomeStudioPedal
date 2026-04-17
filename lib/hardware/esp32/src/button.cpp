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
        // Double-press detection: compare this falling edge to the previous one
        if ((now - lastPressTime_) < doublePressWindow_ && lastPressTime_ != 0)
        {
            doublePressFlag_ = true;
            singlePressSuppressed_ = true; // suppress the pending single press
        }
        else
        {
            singlePressSuppressed_ = false;
        }
        lastPressTime_ = now;
        pressStartTime_ = now;
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
        if (singlePressSuppressed_)
        {
            singlePressSuppressed_ = false;
            return false;
        }
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
    pressStartTime_ = 0;
    lastPressTime_ = 0;
    doublePressFlag_ = false;
    singlePressSuppressed_ = false;
}

void Button::setDoublePressWindow(unsigned long ms) { doublePressWindow_ = ms; }

unsigned long Button::holdDurationMs() const
{
    if (awaitingRelease)
    {
        return millis() - pressStartTime_;
    }
    return 0;
}

bool Button::doublePressEvent()
{
    if (doublePressFlag_)
    {
        doublePressFlag_ = false;
        return true;
    }
    return false;
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
