#pragma once
#include <cstdint>

using MediaKeyReport = uint8_t[2];

class IBleKeyboard {
public:
    virtual ~IBleKeyboard() = default;
    virtual void begin() = 0;
    virtual bool isConnected() = 0;
    virtual void write(uint8_t key) = 0;
    virtual void write(const MediaKeyReport key) = 0;
    virtual void print(const char* text) = 0;
};
