#include <Arduino.h>
#include <unity.h>
#include "button.h"
#include "config.h"

// --- Helpers ----------------------------------------------------------------

static Button* btn = nullptr;
static uint8_t btn_pin = 0;
static const char* btn_name = "";

static void isr_handler() {
    btn->isr();
}

static void setup_button(Button& b, uint8_t pin, const char* name) {
    btn = &b;
    btn_pin = pin;
    btn_name = name;
    b.setup();
    b.reset();
    attachInterrupt(pin, isr_handler, CHANGE);
}

static void teardown_button(uint8_t pin) {
    detachInterrupt(pin);
}

// Count events fired within a window (ms) after prompt
static int count_events(Button& b, unsigned long window_ms) {
    int count = 0;
    unsigned long start = millis();
    while (millis() - start < window_ms) {
        if (b.event()) count++;
    }
    return count;
}

static void prompt(const char* msg) {
    Serial.println();
    Serial.print(">>> ");
    Serial.println(msg);
    delay(100); // flush
}

// --- Buttons under test -----------------------------------------------------

static Button btnA(GPIO_NUM_13);
static Button btnB(GPIO_NUM_12);
static Button btnC(GPIO_NUM_27);
static Button btnD(GPIO_NUM_14);
static Button btnSel(GPIO_NUM_21);

// --- Tests ------------------------------------------------------------------

void test_single_press_fires_one_event() {
    prompt("Press button A ONCE, then wait 3s...");
    setup_button(btnA, GPIO_NUM_13, "A");
    delay(3000);
    int n = count_events(btnA, 500);
    teardown_button(GPIO_NUM_13);
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Expected exactly 1 event for single press");
}

void test_no_event_before_press() {
    prompt("Do NOT press any button for 2s...");
    setup_button(btnA, GPIO_NUM_13, "A");
    int n = count_events(btnA, 2000);
    teardown_button(GPIO_NUM_13);
    TEST_ASSERT_EQUAL_MESSAGE(0, n, "Expected 0 events with no press");
}

void test_double_press_fires_two_events() {
    prompt("Press button A TWICE (with pause between), then wait 3s...");
    setup_button(btnA, GPIO_NUM_13, "A");
    delay(5000);
    int n = count_events(btnA, 500);
    teardown_button(GPIO_NUM_13);
    TEST_ASSERT_EQUAL_MESSAGE(2, n, "Expected exactly 2 events for double press");
}

void test_hold_fires_one_event() {
    prompt("Press and HOLD button A for 2s, then release...");
    setup_button(btnA, GPIO_NUM_13, "A");
    delay(3000);
    int n = count_events(btnA, 500);
    teardown_button(GPIO_NUM_13);
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Expected exactly 1 event when holding button");
}

void test_release_does_not_fire_event() {
    prompt("Press button A once, release slowly. Wait 3s...");
    setup_button(btnA, GPIO_NUM_13, "A");
    delay(3000);
    // drain the press event
    count_events(btnA, 200);
    // now count any events that fire in the next 1s (should be 0 — release)
    int n = count_events(btnA, 1000);
    teardown_button(GPIO_NUM_13);
    TEST_ASSERT_EQUAL_MESSAGE(0, n, "Expected 0 events on release");
}

void test_reset_clears_stale_event() {
    prompt("Press button A once, then wait 2s...");
    setup_button(btnA, GPIO_NUM_13, "A");
    delay(2000);
    btnA.reset();
    int n = count_events(btnA, 500);
    teardown_button(GPIO_NUM_13);
    TEST_ASSERT_EQUAL_MESSAGE(0, n, "Expected 0 events after reset()");
}

void test_button_b_single_press() {
    prompt("Press button B ONCE, then wait 3s...");
    setup_button(btnB, GPIO_NUM_12, "B");
    delay(3000);
    int n = count_events(btnB, 500);
    teardown_button(GPIO_NUM_12);
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Button B: expected 1 event");
}

void test_button_c_single_press() {
    prompt("Press button C ONCE, then wait 3s...");
    setup_button(btnC, GPIO_NUM_27, "C");
    delay(3000);
    int n = count_events(btnC, 500);
    teardown_button(GPIO_NUM_27);
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Button C: expected 1 event");
}

void test_button_d_single_press() {
    prompt("Press button D ONCE, then wait 3s...");
    setup_button(btnD, GPIO_NUM_14, "D");
    delay(3000);
    int n = count_events(btnD, 500);
    teardown_button(GPIO_NUM_14);
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "Button D: expected 1 event");
}

void test_button_select_single_press() {
    prompt("Press SELECT button ONCE, then wait 3s...");
    setup_button(btnSel, GPIO_NUM_21, "SELECT");
    delay(3000);
    int n = count_events(btnSel, 500);
    teardown_button(GPIO_NUM_21);
    TEST_ASSERT_EQUAL_MESSAGE(1, n, "SELECT: expected 1 event");
}

void test_two_buttons_sequential() {
    prompt("Press button A, then button B (with pause between). Wait 6s...");
    setup_button(btnA, GPIO_NUM_13, "A");
    setup_button(btnB, GPIO_NUM_12, "B");
    delay(6000);
    int na = count_events(btnA, 300);
    int nb = count_events(btnB, 300);
    teardown_button(GPIO_NUM_13);
    teardown_button(GPIO_NUM_12);
    TEST_ASSERT_EQUAL_MESSAGE(1, na, "Button A: expected 1 event");
    TEST_ASSERT_EQUAL_MESSAGE(1, nb, "Button B: expected 1 event");
}

// --- Entry point ------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_no_event_before_press);
    RUN_TEST(test_single_press_fires_one_event);
    RUN_TEST(test_double_press_fires_two_events);
    RUN_TEST(test_hold_fires_one_event);
    RUN_TEST(test_release_does_not_fire_event);
    RUN_TEST(test_reset_clears_stale_event);
    RUN_TEST(test_button_b_single_press);
    RUN_TEST(test_button_c_single_press);
    RUN_TEST(test_button_d_single_press);
    RUN_TEST(test_button_select_single_press);
    RUN_TEST(test_two_buttons_sequential);

    UNITY_END();
}

void loop() {}
