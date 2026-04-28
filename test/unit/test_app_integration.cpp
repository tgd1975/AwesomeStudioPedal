// End-to-end PedalApp integration tests on host.
//
// These exercise the full BlePedalApp::setup() / loop() flow using
// HostPedalApp + MockBleKeyboard + per-pin fake_gpio. They validate
// the value the EPIC-020 HAL refactor unlocked: hardware-independent
// integration testing of pedal behaviour.
//
// Each test constructs a fresh HostPedalApp because BlePedalApp's
// global ISR pointer (g_blePedalApp) gets set in the constructor —
// test fixtures must scope the app's lifetime to a single TEST_F.

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "config.h"
#include "host_pedal_app.h"
#include "mock_ble_keyboard.h"

#include "arduino_shim.h" // fake_gpio, fake_time

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::StrEq;

namespace
{
    // The default config (loaded when /profiles.json is absent on host) has
    // four buttons:  A=SendString(" "), B=SendMediaKey(STOP),
    // C=SendChar(LEFT_ARROW), D=SendChar(RIGHT_ARROW). Tests below assert on
    // those specific outputs.

    class AppIntegrationTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            fake_gpio::reset();
            fake_time::value = 0;

            // Release every button (active-low: HIGH = released).
            for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
                fake_gpio::setPinState(hardwareConfig.buttonPins[i], HIGH);
            fake_gpio::setPinState(hardwareConfig.buttonSelect, HIGH);
        }

        // Simulate a press-and-release sequence on action button index `btn`.
        // Advances fake_time enough to clear the debounce window between
        // edges, then past the double-press window so event() reports a
        // single-press event.
        void pressAndRelease(BlePedalApp& app, uint8_t btn)
        {
            const uint8_t pin = hardwareConfig.buttonPins[btn];

            // Advance past Button's 100 ms debounce window so the first edge
            // is honoured (lastDebounceTime starts at 0).
            fake_time::value += 200;
            fake_gpio::setPinState(pin, LOW);
            app.onActionButtonInterrupt(btn);

            fake_time::value += 150;
            fake_gpio::setPinState(pin, HIGH);
            app.onActionButtonInterrupt(btn);

            // Past the 300 ms double-press window so event() fires.
            fake_time::value += 400;
        }
    };

    TEST_F(AppIntegrationTest, SetupRunsCleanly)
    {
        MockBleKeyboard mockKb;
        EXPECT_CALL(mockKb, begin()).Times(1);
        EXPECT_CALL(mockKb, isConnected()).Times(AnyNumber());

        HostPedalApp app(&mockKb);
        app.setup();
        SUCCEED();
    }

    TEST_F(AppIntegrationTest, ButtonAPress_EmitsSpaceViaSendString)
    {
        MockBleKeyboard mockKb;
        EXPECT_CALL(mockKb, begin()).Times(1);
        EXPECT_CALL(mockKb, isConnected()).Times(AnyNumber());
        EXPECT_CALL(mockKb, print(StrEq(" "))).Times(1);

        HostPedalApp app(&mockKb);
        app.setup();

        pressAndRelease(app, 0); // button A
        app.loop();
    }

    TEST_F(AppIntegrationTest, ButtonCPress_EmitsLeftArrowViaWrite)
    {
        // SendCharAction "LEFT_ARROW" is HID keycode 0x50 (80) on Arduino.
        // We only assert that *some* write() happens with a non-zero key — the
        // exact code can drift if the key map changes; this test cares about
        // the dispatch path, not the keycode constant.
        MockBleKeyboard mockKb;
        EXPECT_CALL(mockKb, begin()).Times(1);
        EXPECT_CALL(mockKb, isConnected()).Times(AnyNumber());
        EXPECT_CALL(mockKb, write(::testing::A<uint8_t>())).Times(AtLeast(1));

        HostPedalApp app(&mockKb);
        app.setup();

        pressAndRelease(app, 2); // button C
        app.loop();
    }

    TEST_F(AppIntegrationTest, SelectButtonPress_DoesNotEmitAction)
    {
        // Select cycles the profile index; it must not produce any keyboard
        // output. Use StrictMock-style explicit zero expectations so any
        // accidental dispatch fails the test.
        MockBleKeyboard mockKb;
        EXPECT_CALL(mockKb, begin()).Times(1);
        EXPECT_CALL(mockKb, isConnected()).Times(AnyNumber());
        EXPECT_CALL(mockKb, write(::testing::A<uint8_t>())).Times(0);
        EXPECT_CALL(mockKb, print(_)).Times(0);

        HostPedalApp app(&mockKb);
        app.setup();

        // Press + release the SELECT button (index = numButtons in the
        // dispatcher's handler table; the BlePedalApp routes via
        // onSelectButtonInterrupt rather than the action-button fan-out).
        const uint8_t selectPin = hardwareConfig.buttonSelect;

        fake_time::value += 50;
        fake_gpio::setPinState(selectPin, LOW);
        app.onSelectButtonInterrupt();

        fake_time::value += 50;
        fake_gpio::setPinState(selectPin, HIGH);
        app.onSelectButtonInterrupt();

        fake_time::value += 300;
        app.loop();
    }
} // namespace
