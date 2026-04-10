#pragma once
#include <BleKeyboard.h>
#include <gmock/gmock.h>

class MockBleKeyboard : public BleKeyboard
{
public:
    MockBleKeyboard(const char* name, const char* manufacturer) : BleKeyboard(name, manufacturer) {}

    MOCK_METHOD(void, press, (uint8_t key), (override));
    MOCK_METHOD(void, release, (uint8_t key), (override));
    MOCK_METHOD(void, print, (const char* text), (override));
    MOCK_METHOD(void, write, (uint8_t mediaKey), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(void, begin, (), (override));
};
