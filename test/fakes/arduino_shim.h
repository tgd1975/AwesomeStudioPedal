#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

using boolean = bool;
using String = std::string;

// ESP-IDF type stub used by Button and the ESP32 hardware drivers
typedef int gpio_num_t;

// GPIO stubs — no-ops in host builds; only needed so driver sources compile
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
inline void gpio_pad_select_gpio(gpio_num_t) {}
inline void pinMode(uint8_t, uint8_t) {}

namespace fake_gpio
{
    extern int pin_state;     // LOW = button pressed (active-low), HIGH = released
    extern int written_pin;   // last pin passed to digitalWrite()
    extern int written_value; // last value passed to digitalWrite()
}

inline int digitalRead(uint8_t) { return fake_gpio::pin_state; }
inline void digitalWrite(uint8_t pin, uint8_t val)
{
    fake_gpio::written_pin = static_cast<int>(pin);
    fake_gpio::written_value = static_cast<int>(val);
}

namespace fake_time
{
    extern unsigned long value;
}

inline unsigned long millis() { return fake_time::value; }

struct FakeSerial
{
    template <typename Fmt, typename... Args> void printf(Fmt, Args...) {}
} extern Serial;
