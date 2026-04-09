#include "config_loader.h"
#include "profile_manager.h"
#include "mock_led_controller.h"
#include "send.h"
#include "serial_action.h"
#include "button_constants.h"
#include <gtest/gtest.h>

// Note: File system tests (loadFromFile/saveToFile) are not tested in host environment
// since LittleFS is not available. Only JSON parsing and string-based loading is tested.

class MockKeyboard : public IBleKeyboard {
public:
    void begin() override {}
    bool isConnected() override { return true; }
    void write(uint8_t key) override {}
    void write(const MediaKeyReport key) override {}
    void print(const char* text) override {}
};

class ConfigLoaderTest : public ::testing::Test {
protected:
    MockLEDController led1, led2, led3;
    ProfileManager profileManager{led1, led2, led3};
    MockKeyboard keyboard;
    ConfigLoader configLoader;
};

TEST_F(ConfigLoaderTest, LoadFromValidJsonString) {
    std::string validJson = R"({
        "profiles": [{
            "name": "TestProfile",
            "buttons": {
                "A": {"type": "SendStringAction", "value": "Test"}
            }
        }]
    });
    
    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, validJson));
    EXPECT_EQ(profileManager.getProfileName(0), "TestProfile");
    EXPECT_NE(profileManager.getAction(0, Button::A), nullptr);
}

TEST_F(ConfigLoaderTest, LoadFromInvalidJsonReturnsFalse) {
    std::string invalidJson = "not valid json";
    EXPECT_FALSE(configLoader.loadFromString(profileManager, &keyboard, invalidJson));
}

TEST_F(ConfigLoaderTest, GetDefaultConfigReturnsValidJson) {
    const char* defaultConfig = configLoader.getDefaultConfig();
    EXPECT_FALSE(defaultConfig == nullptr);
    EXPECT_NE(std::string(defaultConfig).find("profiles"), std::string::npos);
}

TEST_F(ConfigLoaderTest, UnknownActionTypeReturnsNullptr) {
    std::string jsonWithUnknownType = R"({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "UnknownAction", "value": "test"}
            }
        }]
    });
    
    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, jsonWithUnknownType));
    EXPECT_EQ(profileManager.getAction(0, Button::A), nullptr);
}

TEST_F(ConfigLoaderTest, DelayedActionParsing) {
    std::string jsonWithDelayedAction = R"({
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
    });
    
    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, jsonWithDelayedAction));
    Action* action = profileManager.getAction(0, Button::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getDelay(), 1000);
    EXPECT_TRUE(action->isSendAction()); // DelayedAction should delegate to inner action
}

TEST_F(ConfigLoaderTest, SerialOutputActionParsing) {
    std::string jsonWithSerialAction = R"({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "SerialOutputAction", "value": "Debug message"}
            }
        }]
    });
    
    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, jsonWithSerialAction));
    Action* action = profileManager.getAction(0, Button::A);
    EXPECT_NE(action, nullptr);
    EXPECT_FALSE(action->isSendAction()); // SerialOutputAction is not a send action
}