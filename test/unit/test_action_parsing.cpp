#include "button_constants.h"
#include "config.h"
#include "config_loader.h"
#include "delayed_action.h"
#include "mock_ble_keyboard.h"
#include "mock_led_controller.h"
#include "profile_manager.h"
#include "send_action.h"
#include "serial_action.h"
#include <gtest/gtest.h>
#include <vector>

using ::testing::_;
using ::testing::AnyNumber;

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class ActionParsingTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EXPECT_CALL(led1, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(AnyNumber());
        pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
    }

    // Load a single-button config and return the parsed action for button A.
    Action* loadSingleAction(const char* type, const char* value, const char* extra = "")
    {
        std::string json = R"({"profiles":[{"name":"T","buttons":{"A":{"type":")";
        json += type;
        json += R"(","value":")";
        json += value;
        json += "\"";
        if (extra[0])
        {
            json += ",";
            json += extra;
        }
        json += "}}}]})";
        pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
        loader.loadFromString(*pm, &kb, json);
        return pm->getAction(0, Btn::A);
    }

    MockLEDController led1, led2, led3;
    MockBleKeyboard kb;
    std::unique_ptr<ProfileManager> pm;
    ConfigLoader loader;
};

// ---------------------------------------------------------------------------
// SendCharAction — named keys must resolve to non-null actions
// ---------------------------------------------------------------------------

// Arrow keys
TEST_F(ActionParsingTest, SendChar_KEY_LEFT_ARROW)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_LEFT_ARROW"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_RIGHT_ARROW)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_RIGHT_ARROW"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_UP_ARROW)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_UP_ARROW"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_DOWN_ARROW)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_DOWN_ARROW"), nullptr);
}

// Navigation keys used in real config
TEST_F(ActionParsingTest, SendChar_KEY_PAGE_UP)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_PAGE_UP"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_PAGE_DOWN)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_PAGE_DOWN"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_HOME)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_HOME"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_END)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_END"), nullptr);
}

// Control keys
TEST_F(ActionParsingTest, SendChar_KEY_TAB)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_TAB"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_RETURN)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_RETURN"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_ENTER_alias)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_ENTER"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_ESC)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_ESC"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_BACKSPACE)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_BACKSPACE"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_PRINTSCREEN)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_PRINTSCREEN"), nullptr);
}

// Modifier keys
TEST_F(ActionParsingTest, SendChar_KEY_LEFT_CTRL)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_LEFT_CTRL"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_LEFT_SHIFT)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_LEFT_SHIFT"), nullptr);
}

// Function keys — spot-check low, mid, and extended range
TEST_F(ActionParsingTest, SendChar_KEY_F1)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F1"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F9)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F9"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F12)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F12"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F13)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F13"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F14)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F14"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F15)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F15"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F16)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F16"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_KEY_F24)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "KEY_F24"), nullptr);
}

// Legacy names without KEY_ prefix
TEST_F(ActionParsingTest, SendChar_LegacyName_LEFT_ARROW)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "LEFT_ARROW"), nullptr);
}
TEST_F(ActionParsingTest, SendChar_LegacyName_F9)
{
    EXPECT_NE(loadSingleAction("SendCharAction", "F9"), nullptr);
}

// ---------------------------------------------------------------------------
// SendCharAction — single ASCII character fallback
// ---------------------------------------------------------------------------

TEST_F(ActionParsingTest, SendChar_SingleChar_OpenBracket)
{
    Action* a = loadSingleAction("SendCharAction", "[");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendChar);
}
TEST_F(ActionParsingTest, SendChar_SingleChar_CloseBracket)
{
    Action* a = loadSingleAction("SendCharAction", "]");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendChar);
}
TEST_F(ActionParsingTest, SendChar_SingleChar_Letter_c)
{
    Action* a = loadSingleAction("SendCharAction", "c");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendChar);
}
TEST_F(ActionParsingTest, SendChar_SingleChar_Space)
{
    Action* a = loadSingleAction("SendCharAction", " ");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendChar);
}

// An unknown multi-char key name must produce nullptr (not crash)
TEST_F(ActionParsingTest, SendChar_UnknownKeyName_ReturnsNullptr)
{
    EXPECT_EQ(loadSingleAction("SendCharAction", "KEY_NONEXISTENT_XYZ"), nullptr);
}

// ---------------------------------------------------------------------------
// SendMediaKeyAction — all names used in real config and their aliases
// ---------------------------------------------------------------------------

TEST_F(ActionParsingTest, SendMediaKey_MEDIA_STOP)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_STOP");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
TEST_F(ActionParsingTest, SendMediaKey_MEDIA_PLAY_PAUSE)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_PLAY_PAUSE");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
TEST_F(ActionParsingTest, SendMediaKey_MEDIA_NEXT_TRACK)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_NEXT_TRACK");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
TEST_F(ActionParsingTest, SendMediaKey_MEDIA_PREVIOUS_TRACK)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_PREVIOUS_TRACK");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
TEST_F(ActionParsingTest, SendMediaKey_MEDIA_MUTE)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_MUTE");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
TEST_F(ActionParsingTest, SendMediaKey_MEDIA_VOLUME_UP)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_VOLUME_UP");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
TEST_F(ActionParsingTest, SendMediaKey_MEDIA_VOLUME_DOWN)
{
    Action* a = loadSingleAction("SendMediaKeyAction", "MEDIA_VOLUME_DOWN");
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::SendMediaKey);
}
// KEY_MEDIA_* prefix aliases
TEST_F(ActionParsingTest, SendMediaKey_KEY_MEDIA_PLAY_PAUSE_alias)
{
    EXPECT_NE(loadSingleAction("SendMediaKeyAction", "KEY_MEDIA_PLAY_PAUSE"), nullptr);
}
TEST_F(ActionParsingTest, SendMediaKey_KEY_MEDIA_STOP_alias)
{
    EXPECT_NE(loadSingleAction("SendMediaKeyAction", "KEY_MEDIA_STOP"), nullptr);
}
// Short volume aliases used in real config (profile 02 Pixel Camera Remote)
TEST_F(ActionParsingTest, SendMediaKey_KEY_VOLUME_UP_alias)
{
    EXPECT_NE(loadSingleAction("SendMediaKeyAction", "KEY_VOLUME_UP"), nullptr);
}
TEST_F(ActionParsingTest, SendMediaKey_KEY_VOLUME_DOWN_alias)
{
    EXPECT_NE(loadSingleAction("SendMediaKeyAction", "KEY_VOLUME_DOWN"), nullptr);
}

// ---------------------------------------------------------------------------
// DelayedAction with inner SendMediaKeyAction
// (mirrors profile "02 Pixel Camera Remote", button A)
// ---------------------------------------------------------------------------

TEST_F(ActionParsingTest, DelayedAction_WithInner_SendMediaKey)
{
    const char* json = R"({
        "profiles":[{"name":"T","buttons":{"A":{
            "type":"DelayedAction",
            "delayMs":3000,
            "action":{"type":"SendMediaKeyAction","value":"KEY_VOLUME_UP"}
        }}}]})";
    pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
    loader.loadFromString(*pm, &kb, json);
    Action* a = pm->getAction(0, Btn::A);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::Delayed);
    EXPECT_EQ(a->getDelay(), 3000u);
}

// ---------------------------------------------------------------------------
// Action names are preserved
// ---------------------------------------------------------------------------

TEST_F(ActionParsingTest, ActionName_IsPreserved)
{
    const char* json = R"({
        "profiles":[{"name":"T","buttons":{"A":{
            "type":"SendCharAction","name":"Next Page","value":"KEY_PAGE_DOWN"
        }}}]})";
    pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
    loader.loadFromString(*pm, &kb, json);
    Action* a = pm->getAction(0, Btn::A);
    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(a->hasName());
    EXPECT_EQ(a->getName(), "Next Page");
}

// ---------------------------------------------------------------------------
// Full real pedal_config.json — all 7 profiles, all 4 buttons non-null
//
// This test catches two classes of bugs at once:
//   1. DynamicJsonDocument too small  → profiles silently truncated
//   2. Missing key names in KEY_TABLE → actions silently dropped
// ---------------------------------------------------------------------------

static const char* REAL_PEDAL_CONFIG = R"json({
  "profiles": [
    {
      "name": "01 Score Navigator",
      "description": "Designed for musicians reading digital sheet music on a tablet or notebook.",
      "buttons": {
        "A": { "type": "SendCharAction", "name": "Prev Page", "value": "KEY_PAGE_UP" },
        "B": { "type": "SendCharAction", "name": "Next Page", "value": "KEY_PAGE_DOWN" },
        "C": { "type": "SendCharAction", "name": "First Page", "value": "KEY_HOME" },
        "D": { "type": "SendCharAction", "name": "Last Page", "value": "KEY_END" }
      }
    },
    {
      "name": "02 Pixel Camera Remote",
      "description": "Hands-free video and photo control for Google Pixel smartphones.",
      "buttons": {
        "A": {
          "type": "DelayedAction",
          "name": "Delayed Shutter",
          "delayMs": 3000,
          "action": { "type": "SendMediaKeyAction", "value": "KEY_VOLUME_UP" }
        },
        "B": { "type": "SendMediaKeyAction", "name": "Instant Shutter", "value": "KEY_VOLUME_UP" },
        "C": { "type": "SendCharAction", "name": "Switch Mode", "value": "KEY_TAB" },
        "D": { "type": "SendCharAction", "name": "Gallery Preview", "value": "KEY_ENTER" }
      }
    },
    {
      "name": "03 VLC Mobile Controller",
      "description": "Optimized for the VLC app on smartphones.",
      "buttons": {
        "A": { "type": "SendMediaKeyAction", "name": "Play/Pause", "value": "MEDIA_PLAY_PAUSE" },
        "B": { "type": "SendMediaKeyAction", "name": "Stop", "value": "MEDIA_STOP" },
        "C": { "type": "SendCharAction", "name": "Slow Down", "value": "[" },
        "D": { "type": "SendCharAction", "name": "Speed Up", "value": "]" }
      }
    },
    {
      "name": "04 OBS Stream Deck",
      "description": "A foot-operated switcher for OBS Studio on a PC/Mac.",
      "buttons": {
        "A": { "type": "SendCharAction", "name": "Intro Scene", "value": "KEY_F13" },
        "B": { "type": "SendCharAction", "name": "Main Scene", "value": "KEY_F14" },
        "C": { "type": "SendCharAction", "name": "Mute Mic", "value": "KEY_F15" },
        "D": { "type": "SendCharAction", "name": "Start Stream", "value": "KEY_F16" }
      }
    },
    {
      "name": "05 DAW Looper (Ableton)",
      "description": "General purpose recording and looping profile for DAWs.",
      "buttons": {
        "A": { "type": "SendCharAction", "name": "Record", "value": "F9" },
        "B": { "type": "SendCharAction", "name": "Play/Stop", "value": " " },
        "C": { "type": "SendStringAction", "name": "Undo Take", "value": "ctrl+z" },
        "D": { "type": "SendCharAction", "name": "Metronome", "value": "c" }
      }
    },
    {
      "name": "06 Social & Comms",
      "description": "A productivity profile for interacting with Discord, Slack, or Zoom.",
      "buttons": {
        "A": { "type": "SendStringAction", "name": "Quick Msg", "value": "Starting now, see you in the chat!" },
        "B": { "type": "SendStringAction", "name": "Mute App", "value": "ctrl+shift+m" },
        "C": { "type": "SendStringAction", "name": "Deafen", "value": "ctrl+shift+d" },
        "D": { "type": "SendCharAction", "name": "Screenshot", "value": "KEY_PRINTSCREEN" }
      }
    },
    {
      "name": "07 System Debug",
      "description": "Technical health check profile for the AwesomeStudioPedal.",
      "buttons": {
        "A": { "type": "SerialOutputAction", "name": "Ping Pedal", "value": "AwesomeStudioPedal: Profile 7 Active - All systems nominal." },
        "B": { "type": "SendStringAction", "name": "Version Info", "value": "v1.2-Extended-Func" },
        "C": { "type": "SendCharAction", "name": "Nav Up", "value": "KEY_UP_ARROW" },
        "D": { "type": "SendCharAction", "name": "Nav Down", "value": "KEY_DOWN_ARROW" }
      }
    }
  ]
})json";

static const char* EXPECTED_PROFILE_NAMES[7] = {
    "01 Score Navigator",
    "02 Pixel Camera Remote",
    "03 VLC Mobile Controller",
    "04 OBS Stream Deck",
    "05 DAW Looper (Ableton)",
    "06 Social & Comms",
    "07 System Debug",
};

class RealConfigTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EXPECT_CALL(led1, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(AnyNumber());
        pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
        ok = loader.loadFromString(*pm, &kb, REAL_PEDAL_CONFIG);
    }

    MockLEDController led1, led2, led3;
    MockBleKeyboard kb;
    std::unique_ptr<ProfileManager> pm;
    ConfigLoader loader;
    bool ok = false;
};

TEST_F(RealConfigTest, LoadReturnsTrue) { EXPECT_TRUE(ok); }

TEST_F(RealConfigTest, All7ProfileSlotsPopulated)
{
    for (uint8_t i = 0; i < hardwareConfig.numProfiles; i++)
        EXPECT_NE(pm->getProfile(i), nullptr) << "profile slot " << (int) i << " is null";
}

TEST_F(RealConfigTest, ProfileNamesMatchConfig)
{
    for (uint8_t i = 0; i < hardwareConfig.numProfiles; i++)
    {
        const Profile* p = pm->getProfile(i);
        ASSERT_NE(p, nullptr) << "slot " << (int) i;
        EXPECT_EQ(p->getName(), EXPECTED_PROFILE_NAMES[i]) << "slot " << (int) i;
    }
}

TEST_F(RealConfigTest, Every_Profile_Has_4_NonNull_Actions)
{
    for (uint8_t i = 0; i < hardwareConfig.numProfiles; i++)
    {
        for (uint8_t b = 0; b < hardwareConfig.numButtons; b++)
        {
            EXPECT_NE(pm->getAction(i, b), nullptr)
                << "profile " << EXPECTED_PROFILE_NAMES[i] << " button " << (int) b << " is null";
        }
    }
}

// Spot-check action types for each profile
TEST_F(RealConfigTest, Profile0_AllSendChar)
{
    for (uint8_t b = 0; b < hardwareConfig.numButtons; b++)
        EXPECT_EQ(pm->getAction(0, b)->getType(), Action::Type::SendChar) << "button " << (int) b;
}

TEST_F(RealConfigTest, Profile1_ButtonA_IsDelayed)
{
    EXPECT_EQ(pm->getAction(1, Btn::A)->getType(), Action::Type::Delayed);
}

TEST_F(RealConfigTest, Profile1_ButtonB_IsSendMediaKey)
{
    EXPECT_EQ(pm->getAction(1, Btn::B)->getType(), Action::Type::SendMediaKey);
}

TEST_F(RealConfigTest, Profile2_ButtonA_IsSendMediaKey)
{
    EXPECT_EQ(pm->getAction(2, Btn::A)->getType(), Action::Type::SendMediaKey);
}

TEST_F(RealConfigTest, Profile2_ButtonsC_D_AreSendChar)
{
    // "[" and "]" — single-char fallback
    EXPECT_EQ(pm->getAction(2, Btn::C)->getType(), Action::Type::SendChar);
    EXPECT_EQ(pm->getAction(2, Btn::D)->getType(), Action::Type::SendChar);
}

TEST_F(RealConfigTest, Profile3_AllSendChar_Fkeys)
{
    for (uint8_t b = 0; b < hardwareConfig.numButtons; b++)
        EXPECT_EQ(pm->getAction(3, b)->getType(), Action::Type::SendChar) << "button " << (int) b;
}

TEST_F(RealConfigTest, Profile6_ButtonA_IsSerialOutput)
{
    EXPECT_EQ(pm->getAction(6, Btn::A)->getType(), Action::Type::SerialOutput);
}

// ---------------------------------------------------------------------------
// Regression: DynamicJsonDocument size
// A JSON with long description strings that pushes total size above 2048 bytes
// must still load all profiles correctly.
// ---------------------------------------------------------------------------

TEST_F(ActionParsingTest, LargeJson_AllProfilesLoaded)
{
    // Build a JSON with 7 profiles where each description is 200 chars,
    // pushing total size well above 2048 bytes.
    std::string longDesc(200, 'x');
    std::string json = "{\"profiles\":[";
    for (int i = 0; i < 7; i++)
    {
        if (i > 0)
            json += ",";
        json += "{\"name\":\"P";
        json += std::to_string(i);
        json += "\",\"description\":\"";
        json += longDesc;
        json += "\",\"buttons\":{\"A\":{\"type\":\"SendCharAction\",\"value\":\"KEY_TAB\"}}}";
    }
    json += "]}";

    ASSERT_GT(json.size(), 2048u) << "pre-condition: JSON must exceed old 2048-byte limit";

    pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
    bool result = loader.loadFromString(*pm, &kb, json);
    EXPECT_TRUE(result);
    for (uint8_t i = 0; i < hardwareConfig.numProfiles; i++)
        EXPECT_NE(pm->getProfile(i), nullptr) << "profile slot " << (int) i << " is null";
}
