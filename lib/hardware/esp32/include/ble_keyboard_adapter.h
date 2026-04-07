#pragma once
#include <BleKeyboard.h>
#include "i_ble_keyboard.h"

class BleKeyboardAdapter : public IBleKeyboard {
    BleKeyboard& kb;
public:
    explicit BleKeyboardAdapter(BleKeyboard& kb) : kb(kb) {}
    void begin() override { kb.begin(); }
    bool isConnected() override { return kb.isConnected(); }
    void write(uint8_t key) override { kb.write(key); }
    void write(const MediaKeyReport key) override { kb.write(key); }
    void print(const char* text) override { kb.print(text); }
};
