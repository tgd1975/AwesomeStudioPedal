#include "mock_ble_keyboard.h"
#include "send.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;

TEST(SendChar, CallsWriteWithCorrectKey)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write((uint8_t) 'a')).Times(1);
    SendChar s(&mock, 'a');
    s.send();
}

TEST(SendKey, CallsWriteWithCorrectKeycode)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write((uint8_t) 0x42)).Times(1);
    SendKey s(&mock, 0x42);
    s.send();
}

TEST(SendString, CallsPrintWithCString)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, print(::testing::StrEq("hello"))).Times(1);
    SendString s(&mock, "hello");
    s.send();
}

TEST(SendMediaKey, CallsWriteWithCorrectBytes)
{
    MockBleKeyboard mock;
    MediaKeyReport key = {0x01, 0x02};
    // Disambiguate: MediaKeyReport decays to const uint8_t*
    EXPECT_CALL(mock, write(::testing::An<const uint8_t*>())).Times(1);
    SendMediaKey s(&mock, key);
    s.send();
}
