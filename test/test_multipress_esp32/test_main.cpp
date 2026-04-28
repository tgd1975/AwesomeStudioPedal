#include "builder_config.h"
#include "button.h"
#include <Arduino.h>
#include <unity.h>

// Uses button A from builder_config (GPIO 13 on NodeMCU-32S by default).
static constexpr uint8_t BTN_PIN = CONF_BUTTON_PINS[0];

static Button btn(BTN_PIN);

static void isr_handler() { btn.isr(); }

static void setup_button()
{
    btn.setup();
    btn.reset();
    attachInterrupt(BTN_PIN, isr_handler, CHANGE);
}

static void teardown_button() { detachInterrupt(BTN_PIN); }

static void countdown(int seconds)
{
    for (int i = seconds; i > 0; i--)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "  %ds remaining...", i);
        TEST_MESSAGE(buf);
        delay(1000);
    }
}

static void prompt(const char* msg, int seconds)
{
    TEST_MESSAGE("--------------------------------------------------");
    TEST_MESSAGE(msg);
    countdown(seconds);
}

// --- Tests ------------------------------------------------------------------

// Test 1: Short tap — press and release quickly (under 500 ms).
// Expects: event() fires; holdDurationMs() == 0 after release.
void test_short_press_fires_only_event()
{
    setup_button();
    prompt("ACTION: Press button A ONCE and release immediately", 5);

    bool gotEvent = false;
    bool gotDouble = false;
    unsigned long start = millis();
    while (millis() - start < 500)
    {
        if (btn.event())
            gotEvent = true;
        if (btn.doublePressEvent())
            gotDouble = true;
    }

    unsigned long hold = btn.holdDurationMs();
    teardown_button();

    TEST_ASSERT_TRUE_MESSAGE(gotEvent,
                             "Expected event() to fire — press the button during the countdown");
    TEST_ASSERT_FALSE_MESSAGE(gotDouble, "Double press detected — tap once only");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        0UL, hold, "holdDurationMs() must be 0 after release — release the button fully");
}

// Test 2: Long hold — press and hold for at least 1 second.
// Expects: holdDurationMs() >= 500 while held.
void test_long_press_reports_hold_duration()
{
    setup_button();
    prompt("ACTION: Press button A and HOLD — a second prompt will tell you when to release", 7);

    bool pressed = false;
    unsigned long waitStart = millis();
    while (millis() - waitStart < 3000)
    {
        if (btn.holdDurationMs() > 0)
        {
            pressed = true;
            break;
        }
    }

    unsigned long maxHold = 0;
    unsigned long sampleStart = millis();
    while (millis() - sampleStart < 800)
    {
        unsigned long h = btn.holdDurationMs();
        if (h > maxHold)
            maxHold = h;
    }

    prompt("ACTION: Release button A now", 3);

    teardown_button();

    TEST_ASSERT_TRUE_MESSAGE(pressed, "No press detected — press the button during the countdown");
    TEST_ASSERT_TRUE_MESSAGE(
        maxHold >= 500, "Hold duration too short — keep the button pressed for the full countdown");
}

// Test 3: Double press — two quick taps within 300 ms.
// Retries up to 3 times if the first attempt is too slow.
// Expects: doublePressEvent() fires; event() does NOT fire.
void test_double_press_suppresses_single()
{
    bool gotDouble = false;
    bool gotSingle = false;

    for (int attempt = 1; attempt <= 3 && ! gotDouble; attempt++)
    {
        char buf[64];
        snprintf(buf,
                 sizeof(buf),
                 "ACTION: Press button A TWICE — pause ~200 ms between presses (attempt %d/3)",
                 attempt);
        setup_button();
        btn.setDoublePressWindow(500); // widen window to 500 ms for easier human triggering
        prompt(buf, 5);

        unsigned long start = millis();
        while (millis() - start < 1000)
        {
            if (btn.doublePressEvent())
                gotDouble = true;
            if (btn.event())
                gotSingle = true;
        }

        teardown_button();

        if (! gotDouble && attempt < 3)
        {
            gotSingle = false; // reset for next attempt
            TEST_MESSAGE("  Not detected — try pressing faster. Retrying...");
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(
        gotDouble,
        "No double press detected after 3 attempts — press twice faster (both within 300 ms)");
    TEST_ASSERT_FALSE_MESSAGE(gotSingle,
                              "Single event also fired — the two presses were too far apart");
}

// --- Entry point ------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(2000);

    UNITY_BEGIN();

    TEST_MESSAGE("==================================================");
    TEST_MESSAGE("  INTERACTIVE MULTI-PRESS TEST — ACTION PLAN");
    TEST_MESSAGE("==================================================");
    TEST_MESSAGE("   1. Press button A ONCE and release         (5s)");
    TEST_MESSAGE("   2. Press and HOLD — release on 2nd prompt  (7s + 3s)");
    TEST_MESSAGE("   3. Press button A TWICE quickly             (5s, retries)");
    TEST_MESSAGE("==================================================");
    TEST_MESSAGE("  Perform each action DURING its countdown.");
    TEST_MESSAGE("  Starting in 5 seconds...");
    TEST_MESSAGE("==================================================");
    delay(5000);

    RUN_TEST(test_short_press_fires_only_event);
    RUN_TEST(test_long_press_reports_hold_duration);
    RUN_TEST(test_double_press_suppresses_single);

    UNITY_END();
}

void loop() {}
