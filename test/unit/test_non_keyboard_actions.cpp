#include "delayed_action.h"
#include "mock_ble_keyboard.h"
#include "mock_led_controller.h"
#include "non_send_action.h"
#include "null_logger.h"
#include "send_action.h"
#include "serial_action.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::AnyNumber;

namespace fake_time
{
    extern unsigned long value;
}

// ---------------------------------------------------------------------------
// SerialOutputAction
// ---------------------------------------------------------------------------

TEST(SerialOutputAction, ExecuteDoesNotCrash)
{
    SerialOutputAction action("hello pedal");
    EXPECT_NO_THROW(action.execute());
}

TEST(SerialOutputAction, GetTypeReturnsSerialOutput)
{
    SerialOutputAction action("msg");
    EXPECT_EQ(action.getType(), Action::Type::SerialOutput);
}

TEST(SerialOutputAction, IsNotASendAction)
{
    SerialOutputAction action("msg");
    EXPECT_FALSE(action.isSendAction());
}

// ---------------------------------------------------------------------------
// LEDBlinkAction
// ---------------------------------------------------------------------------

TEST(LEDBlinkAction, ExecuteBlinksOnce)
{
    MockLEDController led;
    EXPECT_CALL(led, setState(true)).Times(1);
    EXPECT_CALL(led, setState(false)).Times(1);
    LEDBlinkAction action(led, 1, 0);
    action.execute();
}

TEST(LEDBlinkAction, ExecuteBlinksMultipleTimes)
{
    MockLEDController led;
    EXPECT_CALL(led, setState(true)).Times(3);
    EXPECT_CALL(led, setState(false)).Times(3);
    LEDBlinkAction action(led, 3, 0);
    action.execute();
}

TEST(LEDBlinkAction, IsNotASendAction)
{
    MockLEDController led;
    LEDBlinkAction action(led);
    EXPECT_FALSE(action.isSendAction());
}

TEST(LEDBlinkAction, DefaultGetTypeReturnsUnknown)
{
    MockLEDController led;
    LEDBlinkAction action(led);
    EXPECT_EQ(action.getType(), Action::Type::Unknown);
}

TEST(LEDBlinkAction, GetDelayReturnsZeroByDefault)
{
    MockLEDController led;
    LEDBlinkAction action(led);
    EXPECT_EQ(action.getDelay(), 0U);
}

TEST(LEDBlinkAction, IsNotInProgressByDefault)
{
    MockLEDController led;
    LEDBlinkAction action(led);
    EXPECT_FALSE(action.isInProgress());
}

// ---------------------------------------------------------------------------
// DelayedAction
// ---------------------------------------------------------------------------

class DelayedActionTest : public ::testing::Test
{
protected:
    void SetUp() override { fake_time::value = 0; }
};

TEST_F(DelayedActionTest, FirstExecuteStartsTimerWithoutFiringInner)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write(::testing::An<uint8_t>())).Times(0);
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('a'));
    DelayedAction da(std::move(inner), 500);
    fake_time::value = 0;
    da.execute();
}

TEST_F(DelayedActionTest, SecondExecuteAfterDelayFiresInnerAction)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write(static_cast<uint8_t>('a'))).Times(1);
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('a'));
    DelayedAction da(std::move(inner), 500);
    fake_time::value = 0;
    da.execute();           // start timer
    fake_time::value = 600; // past delay
    da.execute();           // fires inner action
}

TEST_F(DelayedActionTest, SecondExecuteBeforeDelayDoesNotFireInner)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write(::testing::An<uint8_t>())).Times(0);
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('b'));
    DelayedAction da(std::move(inner), 500);
    fake_time::value = 0;
    da.execute();           // start timer
    fake_time::value = 100; // before delay
    da.execute();           // should not fire
}

TEST_F(DelayedActionTest, UpdateReturnsFalseWhenNotStarted)
{
    auto inner = std::make_unique<SendKeyAction>(nullptr, static_cast<uint8_t>('c'));
    DelayedAction da(std::move(inner), 500);
    EXPECT_FALSE(da.update(1000));
}

TEST_F(DelayedActionTest, UpdateReturnsTrueAfterDelay)
{
    MockBleKeyboard mock;
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('d'));
    DelayedAction da(std::move(inner), 500);
    fake_time::value = 0;
    da.execute();
    EXPECT_TRUE(da.update(600));
}

TEST_F(DelayedActionTest, UpdateReturnsFalseBeforeDelay)
{
    MockBleKeyboard mock;
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('e'));
    DelayedAction da(std::move(inner), 500);
    fake_time::value = 0;
    da.execute();
    EXPECT_FALSE(da.update(100));
}

TEST_F(DelayedActionTest, IsInProgressAfterFirstExecute)
{
    MockBleKeyboard mock;
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('f'));
    DelayedAction da(std::move(inner), 500);
    EXPECT_FALSE(da.isInProgress());
    fake_time::value = 0;
    da.execute();
    EXPECT_TRUE(da.isInProgress());
}

TEST_F(DelayedActionTest, GetTypeReturnsDelayed)
{
    auto inner = std::make_unique<SendKeyAction>(nullptr, static_cast<uint8_t>('g'));
    DelayedAction da(std::move(inner), 500);
    EXPECT_EQ(da.getType(), Action::Type::Delayed);
}

TEST_F(DelayedActionTest, GetDelayReturnsConfiguredValue)
{
    auto inner = std::make_unique<SendKeyAction>(nullptr, static_cast<uint8_t>('h'));
    DelayedAction da(std::move(inner), 1234);
    EXPECT_EQ(da.getDelay(), 1234U);
}

TEST_F(DelayedActionTest, IsSendActionMirrorsInnerAction)
{
    MockBleKeyboard mock;
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('i'));
    DelayedAction da(std::move(inner), 100);
    EXPECT_TRUE(da.isSendAction());
}

// End-to-end: press (execute) → poll via isInProgress+execute → inner fires on elapsed
TEST_F(DelayedActionTest, PollLoop_FiresInnerActionWhenDelayElapsed)
{
    MockBleKeyboard mock;
    EXPECT_CALL(mock, write(static_cast<uint8_t>('z'))).Times(1);
    auto inner = std::make_unique<SendKeyAction>(&mock, static_cast<uint8_t>('z'));
    DelayedAction da(std::move(inner), 500);

    // Simulate button press: starts timer
    fake_time::value = 0;
    da.execute();
    EXPECT_TRUE(da.isInProgress());

    // Poll loop at t=200: not elapsed yet
    fake_time::value = 200;
    if (da.isInProgress())
        da.execute();
    EXPECT_TRUE(da.isInProgress());

    // Poll loop at t=600: delay elapsed — inner fires and resets
    fake_time::value = 600;
    if (da.isInProgress())
        da.execute();
    EXPECT_FALSE(da.isInProgress());
}

TEST(DelayedActionTypeName, AllTypesReturnCorrectString)
{
    using T = Action::Type;
    EXPECT_STREQ(DelayedAction::getTypeName(T::SendString), "SendStringAction");
    EXPECT_STREQ(DelayedAction::getTypeName(T::SendChar), "SendCharAction");
    EXPECT_STREQ(DelayedAction::getTypeName(T::SendKey), "SendKeyAction");
    EXPECT_STREQ(DelayedAction::getTypeName(T::SendMediaKey), "SendMediaKeyAction");
    EXPECT_STREQ(DelayedAction::getTypeName(T::SerialOutput), "SerialOutputAction");
    EXPECT_STREQ(DelayedAction::getTypeName(T::Delayed), "DelayedAction");
    EXPECT_STREQ(DelayedAction::getTypeName(T::Unknown), "UnknownAction");
}
