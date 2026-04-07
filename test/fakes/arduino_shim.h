#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <cstdio>

using boolean = bool;
using String = std::string;
typedef int gpio_num_t;

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
