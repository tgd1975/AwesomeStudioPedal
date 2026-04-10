#include "config_loader.h"
#include "mock_ble_keyboard.h"
#include "mock_led_controller.h"
#include "profile_manager.h"
#include <ArduinoJson.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string makeConfig(std::initializer_list<const char*> names)
{
    std::string s = "{\"profiles\":[";
    bool first = true;
    for (const char* name : names)
    {
        if (! first)
            s += ",";
        first = false;
        s += "{\"name\":\"";
        s += name;
        s += "\",\"buttons\":{\"A\":{\"type\":\"SendStringAction\",\"name\":\"X\",\"value\":\"x\"}}"
             "}";
    }
    s += "]}";
    return s;
}

static uint8_t countPopulated(const ProfileManager& pm)
{
    uint8_t n = 0;
    for (uint8_t i = 0; i < ProfileManager::MAX_PROFILES; i++)
        if (pm.getProfile(i))
            n++;
    return n;
}

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class ConfigLoaderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EXPECT_CALL(led1, setState(::testing::_)).Times(::testing::AnyNumber());
        EXPECT_CALL(led2, setState(::testing::_)).Times(::testing::AnyNumber());
        EXPECT_CALL(led3, setState(::testing::_)).Times(::testing::AnyNumber());
        profileManager =
            std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
        loader = std::make_unique<ConfigLoader>();
    }

    MockLEDController led1, led2, led3;
    std::unique_ptr<ProfileManager> profileManager;
    MockBleKeyboard keyboard;
    std::unique_ptr<ConfigLoader> loader;
};

// ---------------------------------------------------------------------------
// loadFromString — slot population
// ---------------------------------------------------------------------------

TEST_F(ConfigLoaderTest, Load0Profiles_AllSlotsEmpty)
{
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, "{\"profiles\":[]}"));
    EXPECT_EQ(countPopulated(*profileManager), 0);
    EXPECT_EQ(profileManager->getCurrentProfile(), 0);
}

TEST_F(ConfigLoaderTest, Load1Profile_Slot0PopulatedRest_Null)
{
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"Alpha"})));
    EXPECT_NE(profileManager->getProfile(0), nullptr);
    EXPECT_EQ(profileManager->getProfile(0)->getName(), "Alpha");
    for (uint8_t i = 1; i < ProfileManager::MAX_PROFILES; i++)
        EXPECT_EQ(profileManager->getProfile(i), nullptr) << "slot " << (int) i;
    EXPECT_EQ(profileManager->getCurrentProfile(), 0);
}

TEST_F(ConfigLoaderTest, Load5Profiles_Slots0to4Populated)
{
    ASSERT_TRUE(loader->loadFromString(
        *profileManager, &keyboard, makeConfig({"P0", "P1", "P2", "P3", "P4"})));
    for (uint8_t i = 0; i < 5; i++)
        EXPECT_NE(profileManager->getProfile(i), nullptr) << "slot " << (int) i;
    EXPECT_EQ(profileManager->getProfile(5), nullptr);
    EXPECT_EQ(profileManager->getProfile(6), nullptr);
}

TEST_F(ConfigLoaderTest, Load7Profiles_AllSlotsPopulated)
{
    ASSERT_TRUE(loader->loadFromString(
        *profileManager, &keyboard, makeConfig({"P0", "P1", "P2", "P3", "P4", "P5", "P6"})));
    EXPECT_EQ(countPopulated(*profileManager), 7);
}

TEST_F(ConfigLoaderTest, Load8Profiles_ClampedTo7)
{
    ASSERT_TRUE(loader->loadFromString(
        *profileManager,
        &keyboard,
        makeConfig({"P0", "P1", "P2", "P3", "P4", "P5", "P6", "P7_dropped"})));
    EXPECT_EQ(countPopulated(*profileManager), 7);
    // The 8th profile must not appear anywhere
    for (uint8_t i = 0; i < ProfileManager::MAX_PROFILES; i++)
    {
        if (profileManager->getProfile(i))
        {
            EXPECT_NE(profileManager->getProfile(i)->getName(), "P7_dropped");
        }
    }
}

TEST_F(ConfigLoaderTest, LoadResetsCurrentProfileTo0_EvenIfPreviouslyHigher)
{
    // Load 3 profiles and advance to profile 2
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"P0", "P1", "P2"})));
    EXPECT_CALL(led1, setState(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2, setState(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3, setState(::testing::_)).Times(::testing::AnyNumber());
    profileManager->switchProfile(); // -> 1
    profileManager->switchProfile(); // -> 2
    ASSERT_EQ(profileManager->getCurrentProfile(), 2);

    // Reload — currentProfile must reset
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"Q0", "Q1"})));
    EXPECT_EQ(profileManager->getCurrentProfile(), 0);
}

TEST_F(ConfigLoaderTest, LoadInvalidJson_ReturnsFalse)
{
    EXPECT_FALSE(loader->loadFromString(*profileManager, &keyboard, "not json"));
}

// ---------------------------------------------------------------------------
// mergeConfig
// ---------------------------------------------------------------------------

TEST_F(ConfigLoaderTest, MergeInto0Existing_AddsAtSlot0)
{
    ASSERT_TRUE(loader->mergeConfig(*profileManager, &keyboard, makeConfig({"New"})));
    EXPECT_NE(profileManager->getProfile(0), nullptr);
    EXPECT_EQ(profileManager->getProfile(0)->getName(), "New");
    EXPECT_EQ(countPopulated(*profileManager), 1);
}

TEST_F(ConfigLoaderTest, MergeInto5Existing_AddsAtSlot5)
{
    ASSERT_TRUE(loader->loadFromString(
        *profileManager, &keyboard, makeConfig({"P0", "P1", "P2", "P3", "P4"})));
    ASSERT_TRUE(loader->mergeConfig(*profileManager, &keyboard, makeConfig({"P5"})));
    EXPECT_NE(profileManager->getProfile(5), nullptr);
    EXPECT_EQ(profileManager->getProfile(5)->getName(), "P5");
    EXPECT_EQ(countPopulated(*profileManager), 6);
}

TEST_F(ConfigLoaderTest, MergeIntoFullManager_ExtraProfileDropped)
{
    ASSERT_TRUE(loader->loadFromString(
        *profileManager, &keyboard, makeConfig({"P0", "P1", "P2", "P3", "P4", "P5", "P6"})));
    ASSERT_TRUE(loader->mergeConfig(*profileManager, &keyboard, makeConfig({"Overflow"})));
    EXPECT_EQ(countPopulated(*profileManager), 7);
    for (uint8_t i = 0; i < ProfileManager::MAX_PROFILES; i++)
    {
        if (profileManager->getProfile(i))
        {
            EXPECT_NE(profileManager->getProfile(i)->getName(), "Overflow");
        }
    }
}

TEST_F(ConfigLoaderTest, MergeSkipsDuplicateName)
{
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"Existing"})));
    ASSERT_TRUE(loader->mergeConfig(*profileManager, &keyboard, makeConfig({"Existing"})));
    // Still only one slot populated
    EXPECT_EQ(countPopulated(*profileManager), 1);
}

TEST_F(ConfigLoaderTest, MergeDoesNotAffectCurrentProfile)
{
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"P0", "P1", "P2"})));
    EXPECT_CALL(led1, setState(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2, setState(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3, setState(::testing::_)).Times(::testing::AnyNumber());
    profileManager->switchProfile(); // -> 1
    ASSERT_EQ(profileManager->getCurrentProfile(), 1);
    ASSERT_TRUE(loader->mergeConfig(*profileManager, &keyboard, makeConfig({"New"})));
    EXPECT_EQ(profileManager->getCurrentProfile(), 1);
}

// ---------------------------------------------------------------------------
// replaceProfile
// ---------------------------------------------------------------------------

TEST_F(ConfigLoaderTest, ReplaceSlot0_UpdatesName)
{
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"Old"})));
    ASSERT_TRUE(loader->replaceProfile(*profileManager, &keyboard, 0, makeConfig({"New"})));
    EXPECT_EQ(profileManager->getProfile(0)->getName(), "New");
}

TEST_F(ConfigLoaderTest, ReplaceSlot5_NonActive)
{
    ASSERT_TRUE(loader->loadFromString(
        *profileManager, &keyboard, makeConfig({"P0", "P1", "P2", "P3", "P4", "P5"})));
    ASSERT_TRUE(loader->replaceProfile(*profileManager, &keyboard, 5, makeConfig({"P5new"})));
    EXPECT_EQ(profileManager->getProfile(5)->getName(), "P5new");
    EXPECT_EQ(profileManager->getCurrentProfile(), 0);
}

TEST_F(ConfigLoaderTest, ReplaceEmptySlot_PopulatesIt)
{
    // Start with no profiles; slot 3 is empty
    ASSERT_TRUE(loader->replaceProfile(*profileManager, &keyboard, 3, makeConfig({"Fresh"})));
    EXPECT_NE(profileManager->getProfile(3), nullptr);
    EXPECT_EQ(profileManager->getProfile(3)->getName(), "Fresh");
}

TEST_F(ConfigLoaderTest, ReplaceActiveSlot_CurrentProfileUnchanged)
{
    ASSERT_TRUE(loader->loadFromString(*profileManager, &keyboard, makeConfig({"P0", "P1", "P2"})));
    EXPECT_CALL(led1, setState(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2, setState(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3, setState(::testing::_)).Times(::testing::AnyNumber());
    profileManager->switchProfile(); // -> 1
    ASSERT_EQ(profileManager->getCurrentProfile(), 1);
    ASSERT_TRUE(loader->replaceProfile(*profileManager, &keyboard, 1, makeConfig({"P1new"})));
    EXPECT_EQ(profileManager->getCurrentProfile(), 1);
    EXPECT_EQ(profileManager->getProfile(1)->getName(), "P1new");
}

TEST_F(ConfigLoaderTest, ReplaceInvalidIndex7_ReturnsFalse)
{
    EXPECT_FALSE(loader->replaceProfile(*profileManager, &keyboard, 7, makeConfig({"X"})));
}

TEST_F(ConfigLoaderTest, ReplaceInvalidIndex255_ReturnsFalse)
{
    EXPECT_FALSE(loader->replaceProfile(*profileManager, &keyboard, 255, makeConfig({"X"})));
}
