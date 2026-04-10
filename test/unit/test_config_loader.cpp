#include "button_constants.h"
#include "config_loader.h"
#include "mock_led_controller.h"
#include "profile_manager.h"
#include "send_action.h"
#include "serial_action.h"
#include <gtest/gtest.h>
#include <vector>

// Note: File system tests (loadFromFile/saveToFile) are not tested in host environment
// since LittleFS is not available. Only JSON parsing and string-based loading is tested.

class MockKeyboard : public IBleKeyboard
{
public:
    void begin() override {}
    bool isConnected() override { return true; }
    void write(uint8_t key) override {}
    void write(const MediaKeyReport key) override {}
    void print(const char* text) override {}
};

class ConfigLoaderUnitTest : public ::testing::Test
{
protected:
    MockLEDController led1, led2, led3;
    ProfileManager profileManager{std::vector<ILEDController*>{&led1, &led2, &led3}};
    MockKeyboard keyboard;
    ConfigLoader configLoader;
};

TEST_F(ConfigLoaderUnitTest, LoadFromValidJsonString)
{
    std::string validJson = R"json({
        "profiles": [{
            "name": "TestProfile",
            "buttons": {
                "A": {"type": "SendStringAction", "value": "Test"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, validJson));
    EXPECT_EQ(profileManager.getProfileName(0), "TestProfile");
    EXPECT_NE(profileManager.getAction(0, Btn::A), nullptr);
}

TEST_F(ConfigLoaderUnitTest, LoadFromInvalidJsonReturnsFalse)
{
    std::string invalidJson = "not valid json";
    EXPECT_FALSE(configLoader.loadFromString(profileManager, &keyboard, invalidJson));
}

TEST_F(ConfigLoaderUnitTest, GetDefaultConfigReturnsValidJson)
{
    const char* defaultConfig = configLoader.getDefaultConfig();
    EXPECT_FALSE(defaultConfig == nullptr);
    EXPECT_NE(std::string(defaultConfig).find("profiles"), std::string::npos);
}

TEST_F(ConfigLoaderUnitTest, UnknownActionTypeReturnsNullptr)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "UnknownAction", "value": "test"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    EXPECT_EQ(profileManager.getAction(0, Btn::A), nullptr);
}

TEST_F(ConfigLoaderUnitTest, DelayedActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {
                    "type": "DelayedAction",
                    "delayMs": 1000,
                    "action": {"type": "SendStringAction", "value": "Delayed"}
                }
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getDelay(), 1000u);
}

TEST_F(ConfigLoaderUnitTest, SerialOutputActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "SerialOutputAction", "value": "Debug message"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_FALSE(action->isSendAction());
}
