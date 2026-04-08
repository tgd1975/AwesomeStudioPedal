#pragma once
#include <cstdint>

using MediaKeyReport = uint8_t[2];

// Key constants: define only when BleKeyboard.h has not been included,
// so that firmware TUs that include both headers don't get redefinitions.
#ifndef ESP32_BLE_KEYBOARD_H
const uint8_t KEY_UP_ARROW    = 0xDA;
const uint8_t KEY_DOWN_ARROW  = 0xD9;
const uint8_t KEY_LEFT_ARROW  = 0xD8;
const uint8_t KEY_RIGHT_ARROW = 0xD7;

static constexpr uint8_t KEY_MEDIA_STOP[2] = {4, 0};
#endif

class IBleKeyboard {
public:
    virtual ~IBleKeyboard() = default;
    virtual void begin() = 0;
    virtual bool isConnected() = 0;
    virtual void write(uint8_t key) = 0;
    virtual void write(const MediaKeyReport key) = 0;
    virtual void print(const char* text) = 0;
};
