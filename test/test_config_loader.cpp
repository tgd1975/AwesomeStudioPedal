#include "config_loader.h"
#include "profile_manager.h"
#include "mock_ble_keyboard.h"
#include "mock_led_controller.h"
#include <gtest/gtest.h>
#include <string>
#include <ArduinoJson.h>

class ConfigLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        led1 = std::make_unique<MockLEDController>();
        led2 = std::make_unique<MockLEDController>();
        led3 = std::make_unique<MockLEDController>();
        profileManager = std::make_unique<ProfileManager>(*led1, *led2, *led3);
        keyboard = std::make_unique<MockBleKeyboard>();
        loader = std::make_unique<ConfigLoader>();
    }

    std::unique_ptr<MockLEDController> led1, led2, led3;
    std::unique_ptr<ProfileManager> profileManager;
    std::unique_ptr<MockBleKeyboard> keyboard;
    std::unique_ptr<ConfigLoader> loader;
};

TEST_F(ConfigLoaderTest, MergeConfigAddsNewProfile) {
    // Start with empty profile manager
    ASSERT_EQ(profileManager->getProfile(0), nullptr);
    ASSERT_EQ(profileManager->getProfile(1), nullptr);
    ASSERT_EQ(profileManager->getProfile(2), nullptr);

    // Merge a config with one profile
    std::string newConfig = 
        "{\n"
        "  \"profiles\": [\n"
        "    {\n"
        "      \"name\": \"TestProfile\",\n"
        "      \"buttons\": {\n"
        "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Test\", \"value\": \"test\"}\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n";

    bool result = loader->mergeConfig(*profileManager, keyboard.get(), newConfig);
    ASSERT_TRUE(result);

    // Check that the profile was added to the first empty slot
    ASSERT_NE(profileManager->getProfile(0), nullptr);
    ASSERT_EQ(profileManager->getProfile(0)->getName(), "TestProfile");
    ASSERT_EQ(profileManager->getProfile(1), nullptr);
    ASSERT_EQ(profileManager->getProfile(2), nullptr);
}

TEST_F(ConfigLoaderTest, MergeConfigSkipsDuplicateNames) {
    // Start with one profile already loaded
    std::string initialConfig = 
        "{\n"
        "  \"profiles\": [\n"
        "    {\n"
        "      \"name\": \"ExistingProfile\",\n"
        "      \"buttons\": {\n"
        "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Test\", \"value\": \"test\"}\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n";

    bool loadResult = loader->loadFromString(*profileManager, keyboard.get(), initialConfig);
    ASSERT_TRUE(loadResult);

    // Try to merge a config with a profile that has the same name
    std::string duplicateConfig = 
        "{\n"
        "  \"profiles\": [\n"
        "    {\n"
        "      \"name\": \"ExistingProfile\",\n"
        "      \"buttons\": {\n"
        "        \"B\": {\"type\": \"SendStringAction\", \"name\": \"New\", \"value\": \"new\"}\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n";

    bool mergeResult = loader->mergeConfig(*profileManager, keyboard.get(), duplicateConfig);
    ASSERT_TRUE(mergeResult);

    // The existing profile should remain unchanged (no duplicate should be added)
    ASSERT_NE(profileManager->getProfile(0), nullptr);
    ASSERT_EQ(profileManager->getProfile(0)->getName(), "ExistingProfile");
    ASSERT_EQ(profileManager->getProfile(1), nullptr);
    ASSERT_EQ(profileManager->getProfile(2), nullptr);
}

TEST_F(ConfigLoaderTest, ReplaceProfileUpdatesSpecificSlot) {
    // Start with one profile
    std::string initialConfig = 
        "{\n"
        "  \"profiles\": [\n"
        "    {\n"
        "      \"name\": \"OldProfile\",\n"
        "      \"buttons\": {\n"
        "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Old\", \"value\": \"old\"}\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n";

    bool loadResult = loader->loadFromString(*profileManager, keyboard.get(), initialConfig);
    ASSERT_TRUE(loadResult);

    // Replace the profile at index 0
    std::string replacementConfig = 
        "{\n"
        "  \"profiles\": [\n"
        "    {\n"
        "      \"name\": \"NewProfile\",\n"
        "      \"buttons\": {\n"
        "        \"B\": {\"type\": \"SendStringAction\", \"name\": \"New\", \"value\": \"new\"}\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n";

    bool replaceResult = loader->replaceProfile(*profileManager, keyboard.get(), 0, replacementConfig);
    ASSERT_TRUE(replaceResult);

    // Check that the profile was replaced
    ASSERT_NE(profileManager->getProfile(0), nullptr);
    ASSERT_EQ(profileManager->getProfile(0)->getName(), "NewProfile");
}

TEST_F(ConfigLoaderTest, ReplaceProfileInvalidIndexReturnsFalse) {
    std::string config = 
        "{\n"
        "  \"profiles\": [\n"
        "    {\n"
        "      \"name\": \"TestProfile\",\n"
        "      \"buttons\": {\n"
        "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Test\", \"value\": \"test\"}\n"
        "      }\n"
        "    }\n"
        "  ]\n"
        "}\n";

    // Try to replace at an invalid index (>= NUM_PROFILES which is 7)
    bool result = loader->replaceProfile(*profileManager, keyboard.get(), 7, config);
    ASSERT_FALSE(result);
}