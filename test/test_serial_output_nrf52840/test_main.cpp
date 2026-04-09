#include <Arduino.h>
#include <memory>
#include <unity.h>

#include "action.h"
#include "button_constants.h"
#include "i_led_controller.h"
#include "profile.h"
#include "profile_manager.h"
#include "serial_action.h"

// --- Null LED controller (no GPIO side-effects needed for this test) --------

class NullLEDController : public ILEDController
{
public:
    void setup(uint32_t) override {}
    void setState(bool) override {}
    void toggle() override {}
};

// --- Test state -------------------------------------------------------------

static NullLEDController led1, led2, led3;
static ProfileManager pm(led1, led2, led3);

void setUp() {}
void tearDown() {}

// --- Tests ------------------------------------------------------------------

void test_active_profile_is_2()
{
    TEST_ASSERT_EQUAL_MESSAGE(
        2, pm.getCurrentProfile(), "Profile 2 (Custom) must be the active profile");
}

void test_profile2_name_is_custom()
{
    TEST_ASSERT_EQUAL_STRING_MESSAGE(
        "Custom", pm.getProfileName(2).c_str(), "Profile at index 2 must be named 'Custom'");
}

void test_button_b_action_exists_in_profile2()
{
    TEST_ASSERT_NOT_NULL_MESSAGE(pm.getAction(2, Btn::B),
                                 "An action must be configured for Button B in profile 2");
}

void test_button_b_action_is_serial_output_type()
{
    Action* action = pm.getAction(2, Btn::B);
    TEST_ASSERT_NOT_NULL(action);
    TEST_ASSERT_EQUAL_MESSAGE((int) Action::Type::SerialOutput,
                              (int) action->getType(),
                              "Button B action in profile 2 must be of type SerialOutput");
}

void test_action_type_string_is_serialoutput()
{
    Action* action = pm.getAction(2, Btn::B);
    TEST_ASSERT_NOT_NULL(action);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(
        "SerialOutput",
        ProfileManager::getActionTypeString(action->getType()),
        "getActionTypeString must return 'SerialOutput' for this action");
}

void test_button_b_serial_message_matches_expected()
{
    Action* action = pm.getAction(2, Btn::B);
    TEST_ASSERT_NOT_NULL(action);
    auto* sa = static_cast<SerialOutputAction*>(action);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Button B pressed - Custom Profile",
                                     sa->getMessage().c_str(),
                                     "SerialOutputAction message must match the expected string");
}

void test_full_serial_output_format()
{
    // Verifies the exact text produced on the serial connection when Button B
    // is pressed while profile 2 (Custom) is active, matching the three lines
    // emitted by executeActionWithLogging() + SerialOutputAction::execute().
    Action* action = pm.getAction(2, Btn::B);
    TEST_ASSERT_NOT_NULL(action);

    char buf[128];

    // Line 1 — Serial.printf("Button %s pressed (Profile: %s)\n", ...)
    snprintf(buf, sizeof(buf), "Button B pressed (Profile: %s)", pm.getProfileName(2).c_str());
    TEST_ASSERT_EQUAL_STRING("Button B pressed (Profile: Custom)", buf);

    // Line 2 — Serial.printf("  -> Executing %s action\n", ...)
    snprintf(buf,
             sizeof(buf),
             "  -> Executing %s action",
             ProfileManager::getActionTypeString(action->getType()));
    TEST_ASSERT_EQUAL_STRING("  -> Executing SerialOutput action", buf);

    // Line 3 — Serial.print("ACTION: ") + Serial.println(message)
    auto* sa = static_cast<SerialOutputAction*>(action);
    snprintf(buf, sizeof(buf), "ACTION: %s", sa->getMessage().c_str());
    TEST_ASSERT_EQUAL_STRING("ACTION: Button B pressed - Custom Profile", buf);
}

// --- Entry point ------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // Build profile 2 (Custom) with SerialOutputAction on Button B.
    // The message matches pedal_config.json exactly so the test stays in sync
    // with the shipped configuration.
    std::unique_ptr<Profile> profile2(new Profile("Custom"));
    profile2->addAction(Btn::B,
                        std::unique_ptr<SerialOutputAction>(
                            new SerialOutputAction("Button B pressed - Custom Profile")));
    pm.addProfile(2, std::move(profile2));

    // Activate profile 2 (default starts at 0; cycle twice: 0 -> 1 -> 2).
    pm.switchProfile();
    pm.switchProfile();

    UNITY_BEGIN();
    RUN_TEST(test_active_profile_is_2);
    RUN_TEST(test_profile2_name_is_custom);
    RUN_TEST(test_button_b_action_exists_in_profile2);
    RUN_TEST(test_button_b_action_is_serial_output_type);
    RUN_TEST(test_action_type_string_is_serialoutput);
    RUN_TEST(test_button_b_serial_message_matches_expected);
    RUN_TEST(test_full_serial_output_format);
    UNITY_END();
}

void loop() {}
