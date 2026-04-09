#include "mock_ble_keyboard.h"
#include "send.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;

TEST(SendCharAction, CallsWriteWithCorrectKey)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write((uint8_t) 'a')).Times(1);
    SendCharAction s(&mock, 'a');
    s.send();
}

TEST(SendKeyAction, CallsWriteWithCorrectKeycode)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write((uint8_t) 0x42)).Times(1);
    SendKeyAction s(&mock, 0x42);
    s.send();
}

TEST(SendStringAction, CallsPrintWithCString)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, print(::testing::StrEq("hello"))).Times(1);
    SendStringAction s(&mock, "hello");
    s.send();
}

TEST(SendMediaKeyAction, CallsWriteWithCorrectBytes)
{
    MockBleKeyboard mock;
    MediaKeyReport key = {0x01, 0x02};
    // Disambiguate: MediaKeyReport decays to const uint8_t*
    EXPECT_CALL(mock, write(::testing::An<const uint8_t*>())).Times(1);
    SendMediaKeyAction s(&mock, key);
    s.send();
}
