#include <Arduino.h>
#include <memory>
#include <unity.h>

#include "action.h"
#include "button_constants.h"
#include "config_loader.h"
#include "delayed_action.h"
#include "i_led_controller.h"
#include "null_led_controller.h"
#include "profile.h"
#include "profile_manager.h"
#include "send_action.h"

// ---------------------------------------------------------------------------
// Shared state
// ---------------------------------------------------------------------------

static NullLEDController led1, led2, led3;
static ProfileManager pm(led1, led2, led3);
static ConfigLoader loader;

// Null keyboard — send actions are not exercised here
class NullKeyboard : public IBleKeyboard
{
public:
    bool isConnected() override { return false; }
    void print(const char*) override {}
    void write(uint8_t) override {}
    void write(const MediaKeyReport) override {}
    void begin() override {}
};
static NullKeyboard keyboard;

// Helper — count populated slots
static uint8_t countPopulated()
{
    uint8_t n = 0;
    for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++)
        if (pm.getProfile(i))
            n++;
    return n;
}

// Helper — build a minimal single-profile JSON
static std::string profileJson(const char* name)
{
    std::string s = "{\"profiles\":[{\"name\":\"";
    s += name;
    s += "\",\"buttons\":{\"A\":{\"type\":\"SendStringAction\",\"name\":\"X\",\"value\":\"x\"}}}]}";
    return s;
}

// Helper — build JSON with N profiles named P0..P(N-1)
static std::string nProfilesJson(uint8_t n)
{
    std::string s = "{\"profiles\":[";
    char name[8];
    for (uint8_t i = 0; i < n; i++)
    {
        if (i > 0)
            s += ",";
        snprintf(name, sizeof(name), "P%d", i);
        s += "{\"name\":\"";
        s += name;
        s += "\",\"buttons\":{\"A\":{\"type\":\"SendStringAction\",\"name\":\"X\",\"value\":\"x\"}}"
             "}";
    }
    s += "]}";
    return s;
}

// Reset the profile manager to a clean state before each test group
static void resetPM()
{
    for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++)
        pm.addProfile(i, nullptr);
    pm.resetToFirstProfile();
}

void setUp() {}
void tearDown() {}

// ---------------------------------------------------------------------------
// loadFromString — slot population counts
// ---------------------------------------------------------------------------

void test_load0_all_slots_empty()
{
    resetPM();
    bool ok = loader.loadFromString(pm, &keyboard, "{\"profiles\":[]}");
    TEST_ASSERT_TRUE_MESSAGE(ok, "loadFromString with 0 profiles must return true");
    TEST_ASSERT_EQUAL_MESSAGE(0, countPopulated(), "0 profiles loaded → all slots empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, pm.getCurrentProfile(), "currentProfile must be 0");
}

void test_load1_slot0_populated()
{
    resetPM();
    bool ok = loader.loadFromString(pm, &keyboard, profileJson("Alpha"));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL_MESSAGE(pm.getProfile(0), "Slot 0 must be populated");
    TEST_ASSERT_EQUAL_STRING("Alpha", pm.getProfile(0)->getName().c_str());
    TEST_ASSERT_EQUAL_MESSAGE(1, countPopulated(), "Only 1 slot must be populated");
    TEST_ASSERT_EQUAL_MESSAGE(0, pm.getCurrentProfile(), "currentProfile must be 0");
}

void test_load5_slots_0to4_populated()
{
    resetPM();
    bool ok = loader.loadFromString(pm, &keyboard, nProfilesJson(5));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_MESSAGE(5, countPopulated(), "5 slots must be populated");
    for (uint8_t i = 0; i < 5; i++)
        TEST_ASSERT_NOT_NULL_MESSAGE(pm.getProfile(i), "Slot must not be null");
    TEST_ASSERT_NULL_MESSAGE(pm.getProfile(5), "Slot 5 must be null");
    TEST_ASSERT_NULL_MESSAGE(pm.getProfile(6), "Slot 6 must be null");
}

void test_load7_all_slots_populated()
{
    resetPM();
    bool ok = loader.loadFromString(pm, &keyboard, nProfilesJson(7));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_MESSAGE(7, countPopulated(), "All 7 slots must be populated");
}

void test_load8_clamped_to7()
{
    resetPM();
    bool ok = loader.loadFromString(pm, &keyboard, nProfilesJson(8));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_MESSAGE(7, countPopulated(), "8 profiles must be clamped to 7");
}

void test_load_resets_current_profile_to_0()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, nProfilesJson(3));
    pm.switchProfile(); // -> 1
    pm.switchProfile(); // -> 2
    TEST_ASSERT_EQUAL_MESSAGE(2, pm.getCurrentProfile(), "Pre-condition: currentProfile == 2");
    loader.loadFromString(pm, &keyboard, nProfilesJson(2));
    TEST_ASSERT_EQUAL_MESSAGE(
        0, pm.getCurrentProfile(), "currentProfile must reset to 0 after load");
}

// ---------------------------------------------------------------------------
// mergeConfig
// ---------------------------------------------------------------------------

void test_merge_into_empty_lands_at_slot0()
{
    resetPM();
    loader.mergeConfig(pm, &keyboard, profileJson("New"));
    TEST_ASSERT_NOT_NULL(pm.getProfile(0));
    TEST_ASSERT_EQUAL_STRING("New", pm.getProfile(0)->getName().c_str());
    TEST_ASSERT_EQUAL_MESSAGE(1, countPopulated(), "Exactly 1 slot populated");
}

void test_merge_into5_lands_at_slot5()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, nProfilesJson(5));
    loader.mergeConfig(pm, &keyboard, profileJson("P5"));
    TEST_ASSERT_NOT_NULL(pm.getProfile(5));
    TEST_ASSERT_EQUAL_STRING("P5", pm.getProfile(5)->getName().c_str());
    TEST_ASSERT_EQUAL_MESSAGE(6, countPopulated(), "6 slots populated after merge");
}

void test_merge_into_full_drops_overflow()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, nProfilesJson(7));
    loader.mergeConfig(pm, &keyboard, profileJson("Overflow"));
    TEST_ASSERT_EQUAL_MESSAGE(
        7, countPopulated(), "Full manager: overflow profile must be dropped");
}

void test_merge_skips_duplicate_name()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, profileJson("Dup"));
    loader.mergeConfig(pm, &keyboard, profileJson("Dup"));
    TEST_ASSERT_EQUAL_MESSAGE(1, countPopulated(), "Duplicate name must be skipped");
}

// ---------------------------------------------------------------------------
// replaceProfile
// ---------------------------------------------------------------------------

void test_replace_slot0()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, profileJson("Old"));
    loader.replaceProfile(pm, &keyboard, 0, profileJson("New"));
    TEST_ASSERT_NOT_NULL(pm.getProfile(0));
    TEST_ASSERT_EQUAL_STRING("New", pm.getProfile(0)->getName().c_str());
}

void test_replace_slot5()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, nProfilesJson(6));
    loader.replaceProfile(pm, &keyboard, 5, profileJson("P5new"));
    TEST_ASSERT_EQUAL_STRING("P5new", pm.getProfile(5)->getName().c_str());
    TEST_ASSERT_EQUAL_MESSAGE(0, pm.getCurrentProfile(), "currentProfile unchanged");
}

void test_replace_empty_slot_populates_it()
{
    resetPM();
    bool ok = loader.replaceProfile(pm, &keyboard, 3, profileJson("Fresh"));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_NOT_NULL(pm.getProfile(3));
    TEST_ASSERT_EQUAL_STRING("Fresh", pm.getProfile(3)->getName().c_str());
}

void test_replace_invalid_index_returns_false()
{
    resetPM();
    bool ok = loader.replaceProfile(pm, &keyboard, 7, profileJson("X"));
    TEST_ASSERT_FALSE_MESSAGE(ok, "Index 7 (>= NUM_PROFILES) must return false");
}

// ---------------------------------------------------------------------------
// LED encoding visual check (human-verified)
// ---------------------------------------------------------------------------

void test_led_encoding_profile0_visual()
{
    resetPM();
    // Re-create with real LEDControllers on the actual GPIO pins for visual check.
    // Using NullLEDController here — tester observes physical LEDs separately.
    TEST_MESSAGE("LED CHECK: Profile 0 → LED1 ON, LED2 OFF, LED3 OFF");
    TEST_ASSERT_EQUAL(0, pm.getCurrentProfile());
}

void test_led_encoding_profile3_visual()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, nProfilesJson(4));
    pm.switchProfile(); // 0->1
    pm.switchProfile(); // 1->2
    pm.switchProfile(); // 2->3
    TEST_MESSAGE("LED CHECK: Profile 3 → LED1 ON, LED2 ON, LED3 OFF");
    TEST_ASSERT_EQUAL(3, pm.getCurrentProfile());
}

void test_led_encoding_profile6_visual()
{
    resetPM();
    loader.loadFromString(pm, &keyboard, nProfilesJson(7));
    for (uint8_t i = 0; i < 6; i++)
        pm.switchProfile();
    TEST_MESSAGE("LED CHECK: Profile 6 → LED1 ON, LED2 ON, LED3 ON");
    TEST_ASSERT_EQUAL(6, pm.getCurrentProfile());
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(2000);

    UNITY_BEGIN();

    TEST_MESSAGE("=== loadFromString ===");
    RUN_TEST(test_load0_all_slots_empty);
    RUN_TEST(test_load1_slot0_populated);
    RUN_TEST(test_load5_slots_0to4_populated);
    RUN_TEST(test_load7_all_slots_populated);
    RUN_TEST(test_load8_clamped_to7);
    RUN_TEST(test_load_resets_current_profile_to_0);

    TEST_MESSAGE("=== mergeConfig ===");
    RUN_TEST(test_merge_into_empty_lands_at_slot0);
    RUN_TEST(test_merge_into5_lands_at_slot5);
    RUN_TEST(test_merge_into_full_drops_overflow);
    RUN_TEST(test_merge_skips_duplicate_name);

    TEST_MESSAGE("=== replaceProfile ===");
    RUN_TEST(test_replace_slot0);
    RUN_TEST(test_replace_slot5);
    RUN_TEST(test_replace_empty_slot_populates_it);
    RUN_TEST(test_replace_invalid_index_returns_false);

    TEST_MESSAGE("=== LED encoding (visual) ===");
    RUN_TEST(test_led_encoding_profile0_visual);
    RUN_TEST(test_led_encoding_profile3_visual);
    RUN_TEST(test_led_encoding_profile6_visual);

    UNITY_END();
}

void loop() {}
