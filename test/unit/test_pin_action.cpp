#include "arduino_shim.h"
#include "pin_action.h"
#include <gtest/gtest.h>

namespace fake_gpio
{
    extern int written_pin;
    extern int written_value;
}

class PinActionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        fake_gpio::written_pin = -1;
        fake_gpio::written_value = -1;
    }
};

// ---------------------------------------------------------------------------
// PinHigh
// ---------------------------------------------------------------------------

TEST_F(PinActionTest, PinHigh_ExecuteSetsHigh)
{
    PinAction action(Action::Type::PinHigh, 22);
    action.execute();
    EXPECT_EQ(fake_gpio::written_pin, 22);
    EXPECT_EQ(fake_gpio::written_value, HIGH);
}

TEST_F(PinActionTest, PinHigh_GetTypeCorrect)
{
    PinAction action(Action::Type::PinHigh, 22);
    EXPECT_EQ(action.getType(), Action::Type::PinHigh);
}

TEST_F(PinActionTest, PinHigh_ExecuteReleaseIsNoOp)
{
    PinAction action(Action::Type::PinHigh, 22);
    action.execute();
    fake_gpio::written_value = -1;
    action.executeRelease();
    EXPECT_EQ(fake_gpio::written_value, -1); // no write on release
}

// ---------------------------------------------------------------------------
// PinLow
// ---------------------------------------------------------------------------

TEST_F(PinActionTest, PinLow_ExecuteSetsLow)
{
    PinAction action(Action::Type::PinLow, 5);
    action.execute();
    EXPECT_EQ(fake_gpio::written_pin, 5);
    EXPECT_EQ(fake_gpio::written_value, LOW);
}

TEST_F(PinActionTest, PinLow_ExecuteReleaseIsNoOp)
{
    PinAction action(Action::Type::PinLow, 5);
    action.execute();
    fake_gpio::written_value = -1;
    action.executeRelease();
    EXPECT_EQ(fake_gpio::written_value, -1);
}

// ---------------------------------------------------------------------------
// PinToggle
// ---------------------------------------------------------------------------

TEST_F(PinActionTest, PinToggle_FirstExecuteSetsHigh)
{
    PinAction action(Action::Type::PinToggle, 10);
    action.execute();
    EXPECT_EQ(fake_gpio::written_value, HIGH);
}

TEST_F(PinActionTest, PinToggle_SecondExecuteSetsLow)
{
    PinAction action(Action::Type::PinToggle, 10);
    action.execute(); // HIGH
    action.execute(); // LOW
    EXPECT_EQ(fake_gpio::written_value, LOW);
}

TEST_F(PinActionTest, PinToggle_ThirdExecuteSetsHighAgain)
{
    PinAction action(Action::Type::PinToggle, 10);
    action.execute(); // HIGH
    action.execute(); // LOW
    action.execute(); // HIGH
    EXPECT_EQ(fake_gpio::written_value, HIGH);
}

// ---------------------------------------------------------------------------
// PinHighWhilePressed
// ---------------------------------------------------------------------------

TEST_F(PinActionTest, PinHighWhilePressed_ExecuteSetsHigh)
{
    PinAction action(Action::Type::PinHighWhilePressed, 13);
    action.execute();
    EXPECT_EQ(fake_gpio::written_pin, 13);
    EXPECT_EQ(fake_gpio::written_value, HIGH);
}

TEST_F(PinActionTest, PinHighWhilePressed_ExecuteReleaseSetsLow)
{
    PinAction action(Action::Type::PinHighWhilePressed, 13);
    action.execute();
    action.executeRelease();
    EXPECT_EQ(fake_gpio::written_pin, 13);
    EXPECT_EQ(fake_gpio::written_value, LOW);
}

// ---------------------------------------------------------------------------
// PinLowWhilePressed
// ---------------------------------------------------------------------------

TEST_F(PinActionTest, PinLowWhilePressed_ExecuteSetsLow)
{
    PinAction action(Action::Type::PinLowWhilePressed, 14);
    action.execute();
    EXPECT_EQ(fake_gpio::written_pin, 14);
    EXPECT_EQ(fake_gpio::written_value, LOW);
}

TEST_F(PinActionTest, PinLowWhilePressed_ExecuteReleaseSetsHigh)
{
    PinAction action(Action::Type::PinLowWhilePressed, 14);
    action.execute();
    action.executeRelease();
    EXPECT_EQ(fake_gpio::written_pin, 14);
    EXPECT_EQ(fake_gpio::written_value, HIGH);
}
