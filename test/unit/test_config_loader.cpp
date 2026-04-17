#include "button_constants.h"
#include "config_loader.h"
#include "file_system.h"
#include "mock_led_controller.h"
#include "null_logger.h"
#include "pin_action.h"
#include "profile_manager.h"
#include "send_action.h"
#include "serial_action.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using ::testing::_;
using ::testing::AnyNumber;

// ---------------------------------------------------------------------------
// Fake filesystem for saveToFile tests
// ---------------------------------------------------------------------------

class FakeFileSystem : public IFileSystem
{
public:
    std::string lastWrittenPath;
    std::string lastWrittenContent;
    bool writeResult = true;

    bool exists(const char*) override { return false; }
    bool readFile(const char*, std::string&) override { return false; }
    bool writeFile(const char* path, const std::string& content) override
    {
        lastWrittenPath = path;
        lastWrittenContent = content;
        return writeResult;
    }
};

class MockKeyboard : public IBleKeyboard
{
public:
    void begin() override {}
    bool isConnected() override { return true; }
    void write(uint8_t key) override { (void) key; }
    void write(const MediaKeyReport key) override { (void) key; }
    void print(const char* text) override { (void) text; }
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

TEST_F(ConfigLoaderUnitTest, SendKeyActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "SendKeyAction", "value": "KEY_F1"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), Action::Type::SendKey);
}

TEST_F(ConfigLoaderUnitTest, SendKeyActionUnknownValueReturnsNullptr)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "SendKeyAction", "value": "NOT_A_KEY"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    EXPECT_EQ(profileManager.getAction(0, Btn::A), nullptr);
}

TEST_F(ConfigLoaderUnitTest, PinHighActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "PinHighAction", "pin": 5}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), Action::Type::PinHigh);
}

TEST_F(ConfigLoaderUnitTest, PinLowActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "PinLowAction", "pin": 6}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), Action::Type::PinLow);
}

TEST_F(ConfigLoaderUnitTest, PinToggleActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "PinToggleAction", "pin": 7}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), Action::Type::PinToggle);
}

TEST_F(ConfigLoaderUnitTest, PinHighWhilePressedActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "PinHighWhilePressedAction", "pin": 8}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), Action::Type::PinHighWhilePressed);
}

TEST_F(ConfigLoaderUnitTest, PinLowWhilePressedActionParsing)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "PinLowWhilePressedAction", "pin": 9}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    Action* action = profileManager.getAction(0, Btn::A);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), Action::Type::PinLowWhilePressed);
}

TEST_F(ConfigLoaderUnitTest, PinActionMissingPinFieldReturnsNullptr)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "PinHighAction"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    EXPECT_EQ(profileManager.getAction(0, Btn::A), nullptr);
}

TEST_F(ConfigLoaderUnitTest, DelayedActionMissingInnerActionReturnsNullptr)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "DelayedAction", "delayMs": 500}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    EXPECT_EQ(profileManager.getAction(0, Btn::A), nullptr);
}

TEST_F(ConfigLoaderUnitTest, SendMediaKeyUnknownValueReturnsNullptr)
{
    std::string json = R"json({
        "profiles": [{
            "name": "Test",
            "buttons": {
                "A": {"type": "SendMediaKeyAction", "value": "NOT_A_MEDIA_KEY"}
            }
        }]
    })json";

    EXPECT_TRUE(configLoader.loadFromString(profileManager, &keyboard, json));
    EXPECT_EQ(profileManager.getAction(0, Btn::A), nullptr);
}

// ---------------------------------------------------------------------------
// saveToFile tests — use injection constructor with FakeFileSystem
// ---------------------------------------------------------------------------

class ConfigLoaderSaveTest : public ::testing::Test
{
protected:
    FakeFileSystem fs;
    NullLogger logger;
    ConfigLoader loader{&fs, &logger};
    MockLEDController led1, led2, led3;
    MockKeyboard keyboard;
    ProfileManager pm{std::vector<ILEDController*>{&led1, &led2, &led3}};

    void SetUp() override
    {
        EXPECT_CALL(led1, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led1, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led2, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led3, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led1, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led2, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led3, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led1, update(_)).Times(AnyNumber());
        EXPECT_CALL(led2, update(_)).Times(AnyNumber());
        EXPECT_CALL(led3, update(_)).Times(AnyNumber());
        EXPECT_CALL(led1, isBlinking()).Times(AnyNumber());
        EXPECT_CALL(led2, isBlinking()).Times(AnyNumber());
        EXPECT_CALL(led3, isBlinking()).Times(AnyNumber());
    }
};

TEST_F(ConfigLoaderSaveTest, SaveToFileWritesJsonToCorrectPath)
{
    loader.loadFromString(pm, &keyboard, R"json({
        "profiles":[{"name":"MyProfile","buttons":{"A":{"type":"SendStringAction","value":"hi"}}}]
    })json");

    EXPECT_TRUE(loader.saveToFile(pm, "/config.json"));
    EXPECT_EQ(fs.lastWrittenPath, "/config.json");
    EXPECT_NE(fs.lastWrittenContent.find("MyProfile"), std::string::npos);
}

TEST_F(ConfigLoaderSaveTest, SaveToFileReturnsFalseOnWriteFailure)
{
    fs.writeResult = false;
    loader.loadFromString(pm, &keyboard, R"json({
        "profiles":[{"name":"P","buttons":{}}]
    })json");
    EXPECT_FALSE(loader.saveToFile(pm, "/config.json"));
}

TEST_F(ConfigLoaderSaveTest, SaveToFileSerializesAllActionTypes)
{
    loader.loadFromString(pm, &keyboard, R"json({
        "profiles":[{
            "name":"Multi",
            "buttons":{
                "A":{"type":"SendStringAction","value":"hello"},
                "B":{"type":"SendKeyAction","value":"KEY_A"},
                "C":{"type":"SerialOutputAction","value":"ping"},
                "D":{"type":"SendMediaKeyAction","value":"MEDIA_STOP"}
            }
        }]
    })json");

    EXPECT_TRUE(loader.saveToFile(pm, "/multi.json"));
    EXPECT_NE(fs.lastWrittenContent.find("SendStringAction"), std::string::npos);
}

TEST_F(ConfigLoaderSaveTest, SaveToFileWithEmptyProfileManager)
{
    EXPECT_TRUE(loader.saveToFile(pm, "/empty.json"));
    EXPECT_FALSE(fs.lastWrittenContent.empty());
}
