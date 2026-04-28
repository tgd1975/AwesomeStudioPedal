#include "arduino_shim.h"

#include <unordered_map>

namespace fake_time
{
    unsigned long value = 0;
}

namespace fake_gpio
{
    namespace
    {
        std::unordered_map<uint8_t, int>& pinStates()
        {
            static std::unordered_map<uint8_t, int> map;
            return map;
        }
    } // namespace

    void setPinState(uint8_t pin, int state) { pinStates()[pin] = state; }

    int getPinState(uint8_t pin)
    {
        // Pin explicitly set → return its value. Otherwise fall back to the
        // legacy global `pin_state` so older tests that set only the global
        // continue to work for any pin they read.
        auto it = pinStates().find(pin);
        return it == pinStates().end() ? pin_state : it->second;
    }

    void reset()
    {
        pinStates().clear();
        pin_state = LOW;
        written_pin = -1;
        written_value = -1;
    }

    int pin_state = LOW;
    int written_pin = -1;
    int written_value = -1;
}

FakeSerial Serial;
