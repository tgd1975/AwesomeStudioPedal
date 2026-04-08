#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <cstdio>

using boolean = bool;
using String = std::string;

// ESP-IDF type stub used by Button and the ESP32 hardware drivers
typedef int gpio_num_t;

// GPIO stubs — no-ops in host builds; only needed so driver sources compile
#define INPUT_PULLUP 2
inline void gpio_pad_select_gpio(gpio_num_t) {}
inline void pinMode(uint8_t, uint8_t) {}

namespace fake_time {
    extern unsigned long value;
}

inline unsigned long millis() {
    return fake_time::value;
}

struct FakeSerial {
    template<typename Fmt, typename... Args>
    void printf(Fmt, Args...) {}
} extern Serial;
