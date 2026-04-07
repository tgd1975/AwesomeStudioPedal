#pragma once
#include "i_ble_keyboard.h"
#include <gmock/gmock.h>

class MockBleKeyboard : public IBleKeyboard {
public:
    MOCK_METHOD(void, begin, (), (override));
    MOCK_METHOD(bool, isConnected, (), (override));
    MOCK_METHOD(void, write, (uint8_t key), (override));
    MOCK_METHOD(void, write, (const MediaKeyReport key), (override));
    MOCK_METHOD(void, print, (const char* text), (override));
};
