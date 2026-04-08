#include "arduino_shim.h"
#include "button.h"
#include <gtest/gtest.h>

class ButtonTest : public ::testing::Test
{
protected:
    void SetUp() override { fake_time::value = 0; }
};

TEST_F(ButtonTest, EventReturnsFalseInitially)
{
    Button btn(5);
    EXPECT_FALSE(btn.event());
}

TEST_F(ButtonTest, AfterIsrEventReturnsTrue)
{
    Button btn(5);
    fake_time::value = 1000; // past debounce
    btn.isr();
    EXPECT_TRUE(btn.event());
}

TEST_F(ButtonTest, EventClearsFlag)
{
    Button btn(5);
    fake_time::value = 1000;
    btn.isr();
    EXPECT_TRUE(btn.event());
    EXPECT_FALSE(btn.event()); // flag should be cleared
}

TEST_F(ButtonTest, SecondIsrWithinDebounceWindowIsIgnored)
{
    Button btn(5);
    fake_time::value = 1000;
    btn.isr();               // accepted
    btn.event();             // consume the first press
    fake_time::value = 1100; // within 500ms debounce
    btn.isr();               // should be ignored
    EXPECT_FALSE(btn.event());
}

TEST_F(ButtonTest, IsrAfterDebounceDelayIsAccepted)
{
    Button btn(5);
    fake_time::value = 1000;
    btn.isr();               // first press
    btn.event();             // consume the first press
    fake_time::value = 1600; // 600ms later, past 500ms debounce
    btn.isr();               // second press
    EXPECT_TRUE(btn.event());
}
