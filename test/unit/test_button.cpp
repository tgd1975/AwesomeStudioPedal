#include "arduino_shim.h"
#include "button.h"
#include <gtest/gtest.h>

class ButtonTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        fake_time::value = 0;
        fake_gpio::pin_state = LOW; // default: button pressed (active-low)
    }
};

// Helper: simulate a press ISR (pin LOW)
static void press(Button& btn)
{
    fake_gpio::pin_state = LOW;
    btn.isr();
}

// Helper: simulate a release ISR (pin HIGH)
static void release(Button& btn)
{
    fake_gpio::pin_state = HIGH;
    btn.isr();
}

TEST_F(ButtonTest, EventReturnsFalseInitially)
{
    Button btn(5);
    EXPECT_FALSE(btn.event());
}

TEST_F(ButtonTest, PressIsAccepted)
{
    Button btn(5);
    fake_time::value = 200; // 200ms past lastDebounceTime=0, well over 100ms debounce
    press(btn);
    EXPECT_TRUE(btn.event());
}

TEST_F(ButtonTest, EventClearsAfterSingleConsumption)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn);
    EXPECT_TRUE(btn.event());
    EXPECT_FALSE(btn.event());
}

TEST_F(ButtonTest, BounceDuringPressSideIsIgnored)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn); // accepted
    fake_time::value = 205;
    press(btn); // bounce — blocked by awaitingRelease, not debounce
    EXPECT_EQ(btn.pressCount, 1);
}

TEST_F(ButtonTest, SecondPressWithinDebounceWindowAfterReleaseIsIgnored)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn); // accepted, lastDebounceTime=200
    btn.event();
    fake_time::value = 310;
    release(btn); // 110ms after press — debounce elapsed, lastDebounceTime=310
    fake_time::value = 380;
    press(btn); // 70ms after release — within 100ms debounce window → rejected
    EXPECT_FALSE(btn.event());
}

TEST_F(ButtonTest, SecondPressAfterDebounceWindowIsAccepted)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn); // accepted, lastDebounceTime=200
    btn.event();
    fake_time::value = 310;
    release(btn); // 110ms after press — debounce elapsed, lastDebounceTime=310
    fake_time::value = 420;
    press(btn); // 110ms after release — debounce elapsed → accepted
    EXPECT_TRUE(btn.event());
}

// THE CRITICAL CASE: hold > debounceDelay, then release with bounce
TEST_F(ButtonTest, ReleaseBounceAfterLongHoldDoesNotFireSpuriousEvent)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn); // accepted at t=200
    EXPECT_EQ(btn.pressCount, 1);

    // hold for 400ms (well over 100ms debounce delay)
    fake_time::value = 600;
    release(btn); // HIGH edge: awaitingRelease=false, lastDebounceTime=600

    // contact bounce on LOW during release — must be rejected
    fake_time::value = 660;
    press(btn);                   // 60ms after release — within 100ms window → rejected
    EXPECT_EQ(btn.pressCount, 1); // still only 1, no spurious event
}

TEST_F(ButtonTest, ReleaseEdgeAloneDoesNotFireEvent)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn);
    btn.event(); // consume the press
    fake_time::value = 300;
    release(btn); // release should produce no event
    EXPECT_FALSE(btn.event());
}

// Both HIGH and LOW edges require debounce. Alternating bounce after release:
// the first HIGH is accepted (lastDebounceTime=300), subsequent HIGH bounces within
// 100ms are rejected. LOW bounces within 100ms of the accepted HIGH are also rejected.
TEST_F(ButtonTest, AlternatingBouncesAfterReleaseDoNotFireSpuriousEvent)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn);
    btn.event(); // consume, lastDebounceTime=200

    // genuine release 200ms after press — debounce elapsed
    fake_time::value = 400;
    release(btn); // HIGH accepted: lastDebounceTime=400
    fake_time::value = 410;
    press(btn); // LOW bounce: 10ms < 100 → rejected
    fake_time::value = 420;
    release(btn); // HIGH bounce: 20ms < 100 → rejected (debounce on HIGH too)
    fake_time::value = 430;
    press(btn); // LOW bounce: 30ms < 100 → rejected
    EXPECT_EQ(btn.pressCount, 0);

    // 101ms after the accepted HIGH (t=400) — genuine new press
    fake_time::value = 501;
    press(btn); // LOW: 501-400=101ms > 100 → accepted
    EXPECT_EQ(btn.pressCount, 1);
}

// A noise spike HIGH during a held press must not prematurely clear awaitingRelease.
// Without debouncing the HIGH edge, the LOW bounce after the spike would be rejected
// and the button would appear unresponsive.
TEST_F(ButtonTest, NoiseHighDuringHoldDoesNotClearAwaitingRelease)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn); // accepted, awaitingRelease=true, lastDebounceTime=200
    EXPECT_EQ(btn.pressCount, 1);

    // noise spike HIGH at t=210 — within 100ms debounce of press → rejected
    fake_time::value = 210;
    release(btn); // awaitingRelease=true but isDebounced(210): 10ms < 100 → ignored
    EXPECT_TRUE(btn.awaitingRelease); // still waiting for real release

    // LOW bounce at t=215 — awaitingRelease still true, so ignored
    fake_time::value = 215;
    press(btn);
    EXPECT_EQ(btn.pressCount, 1); // no double count

    // genuine release 200ms after press
    fake_time::value = 400;
    release(btn); // 400-200=200ms > 100 → accepted
    EXPECT_FALSE(btn.awaitingRelease);
}

// Race: ISR fires on falling edge but digitalRead sees HIGH (pin bounced back before read).
// awaitingRelease=false, so HIGH is ignored entirely — no timer update.
TEST_F(ButtonTest, SpuriousHighEdgeBeforePressDoesNotBlockEvent)
{
    Button btn(5);
    fake_time::value = 200;
    release(btn); // HIGH with awaitingRelease=false → ignored, no timer update
    fake_time::value = 210;
    press(btn); // LOW 10ms later — lastDebounceTime still 0, so 210ms > 100 → accepted
    EXPECT_EQ(btn.pressCount, 1);
}

TEST_F(ButtonTest, ResetClearsPendingEventsAndDebounceTimer)
{
    Button btn(5);
    fake_time::value = 200;
    press(btn);
    btn.reset();
    EXPECT_FALSE(btn.event());
    EXPECT_EQ(btn.lastDebounceTime, 0UL);
}
