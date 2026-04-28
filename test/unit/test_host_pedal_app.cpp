#include <gtest/gtest.h>

#include "host_pedal_app.h"
#include "mock_ble_keyboard.h"

// Smoke test: HostPedalApp constructs cleanly with an injected
// MockBleKeyboard. The four pure-virtual hooks resolve to no-ops
// (platformSetup/platformLoop) or in-memory store (save/loadProfile).
// Richer end-to-end scenarios live in test_app_integration.cpp
// (TASK-302).
TEST(HostPedalAppTest, ConstructsWithInjectedMockKeyboard)
{
    MockBleKeyboard mockKb;
    HostPedalApp app(&mockKb);
    SUCCEED();
}
