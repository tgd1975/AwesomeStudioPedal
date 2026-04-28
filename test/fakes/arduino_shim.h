#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

using boolean = bool;
using String = std::string;

// ESP-IDF type stub used by Button and the ESP32 hardware drivers
typedef int gpio_num_t;

// GPIO stubs — no-ops in host builds; only needed so driver sources compile
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define CHANGE 1
inline void gpio_pad_select_gpio(gpio_num_t) {}
inline void pinMode(uint8_t, uint8_t) {}

namespace fake_gpio
{
    // Per-pin state. setPinState/getPinState are the new API; tests with
    // multi-button scenarios should use them. The legacy `pin_state` global
    // continues to work as an alias for pin 0 — older tests need no change.
    void setPinState(uint8_t pin, int state);
    int getPinState(uint8_t pin);
    void reset(); // clear all per-pin state + last_written_*

    extern int pin_state; // legacy fallback: returned by digitalRead for any pin not explicitly set
                          // via setPinState / digitalWrite
    extern int written_pin;   // last pin passed to digitalWrite() / gpio_set_level
    extern int written_value; // last value passed to digitalWrite() / gpio_set_level
}

// ESP-IDF gpio_set_direction / gpio_set_level shims
constexpr int GPIO_MODE_OUTPUT = 1;
inline void gpio_set_direction(gpio_num_t, int) {}
inline void gpio_set_level(gpio_num_t pin, int level)
{
    fake_gpio::written_pin = static_cast<int>(pin);
    fake_gpio::written_value = level;
    fake_gpio::setPinState(static_cast<uint8_t>(pin), level);
}

inline int digitalRead(uint8_t pin) { return fake_gpio::getPinState(pin); }
inline void digitalWrite(uint8_t pin, uint8_t val)
{
    fake_gpio::written_pin = static_cast<int>(pin);
    fake_gpio::written_value = static_cast<int>(val);
    fake_gpio::setPinState(pin, static_cast<int>(val));
}

namespace fake_time
{
    extern unsigned long value;
}

inline unsigned long millis() { return fake_time::value; }
inline void delay(uint32_t) {}

// Interrupt API — no-op shims so on-device code compiles in host builds
inline void attachInterrupt(uint8_t, void (*)(), int) {}
inline void detachInterrupt(uint8_t) {}

// Serial-like stub. Sinks output to std::cout so host test logs preserve the
// device-side chatter without requiring per-call site guards.
struct FakeSerial
{
    void begin(unsigned long) {}
    void print(const char* s) { std::cout << s; }
    void print(const std::string& s) { std::cout << s; }
    void println() { std::cout << '\n'; }
    void println(const char* s) { std::cout << s << '\n'; }
    void println(const std::string& s) { std::cout << s << '\n'; }

    template <typename... Args> int printf(const char* fmt, Args... args)
    {
        return std::printf(fmt, args...);
    }
} extern Serial;
