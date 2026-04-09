#include "profile_manager.h"
#include "profile.h"
#include "mock_led_controller.h"
#include "action.h"
#include "send.h"
#include "button_constants.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;

// Minimal concrete Action subclass for testing
class FakeAction : public Action
{
public:
    void execute() override {}
    Action::Type getType() const override { return Action::Type::SendString; }
};

class ProfileManagerTest : public ::testing::Test
{
protected:
    MockLEDController led1, led2, led3;
    ProfileManager* manager;

    void SetUp() override
    {
        EXPECT_CALL(led1, setState(_)).Times(::testing::AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(::testing::AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(::testing::AnyNumber());
        manager = new ProfileManager(led1, led2, led3);
    }

    void TearDown() override { delete manager; }
};

TEST_F(ProfileManagerTest, StartsAtProfile0) { EXPECT_EQ(manager->getCurrentProfile(), 0); }

TEST_F(ProfileManagerTest, SwitchProfileWrapsAfterThree)
{
    EXPECT_CALL(led1, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3, setState(_)).Times(::testing::AnyNumber());
    manager->switchProfile(); // -> 1
    manager->switchProfile(); // -> 2
    manager->switchProfile(); // -> 0
    EXPECT_EQ(manager->getCurrentProfile(), 0);
}

TEST_F(ProfileManagerTest, AddAndGetProfileReturnsCorrectAction)
{
    MockLEDController led1b, led2b, led3b;
    EXPECT_CALL(led1b, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2b, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3b, setState(_)).Times(::testing::AnyNumber());
    ProfileManager bm(led1b, led2b, led3b);
    
    auto profile = std::make_unique<Profile>("Test");
    auto action = std::make_unique<FakeAction>();
    Action* rawPtr = action.get();
    profile->addAction(Button::C, std::move(action));
    bm.addProfile(1, std::move(profile));
    
    EXPECT_EQ(bm.getAction(1, Button::C), rawPtr);
}

TEST_F(ProfileManagerTest, GetActionOutOfBoundsReturnsNullptr)
{
    EXPECT_EQ(manager->getAction(3, Button::A), nullptr);
    EXPECT_EQ(manager->getAction(0, Button::BANK), nullptr);
}

TEST_F(ProfileManagerTest, GetActionEmptySlotReturnsNullptr)
{
    EXPECT_EQ(manager->getAction(0, Button::A), nullptr);
}

TEST_F(ProfileManagerTest, ConstructorSetsProfile0LEDs)
{
    MockLEDController l1, l2, l3;
    EXPECT_CALL(l1, setState(true)).Times(Exactly(1));
    EXPECT_CALL(l2, setState(false)).Times(Exactly(1));
    EXPECT_CALL(l3, setState(false)).Times(Exactly(1));
    ProfileManager bm(l1, l2, l3);
}

TEST_F(ProfileManagerTest, SwitchProfileCallsUpdateLEDs)
{
    MockLEDController l1, l2, l3;
    // Constructor sets profile 0 (l1=true); switchProfile goes 0->1 (l2=true)
    EXPECT_CALL(l1, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(l2, setState(true)).Times(::testing::AtLeast(1));
    EXPECT_CALL(l2, setState(false)).Times(::testing::AnyNumber());
    EXPECT_CALL(l3, setState(_)).Times(::testing::AnyNumber());
    ProfileManager bm(l1, l2, l3);
    bm.switchProfile();
}

TEST_F(ProfileManagerTest, GetProfileNameReturnsCorrectName)
{
    auto profile = std::make_unique<Profile>("TestProfile");
    manager->addProfile(0, std::move(profile));
    EXPECT_EQ(manager->getProfileName(0), "TestProfile");
}

TEST_F(ProfileManagerTest, GetProfileNameEmptySlotReturnsEmptyString)
{
    EXPECT_EQ(manager->getProfileName(1), "");
}

TEST_F(ProfileManagerTest, GetActionTypeStringReturnsCorrectStrings)
{
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::SendString), "SendString");
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::SendChar), "SendChar");
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::SendKey), "SendKey");
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::SendMediaKey), "SendMediaKey");
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::SerialOutput), "SerialOutput");
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::Delayed), "Delayed");
    EXPECT_STREQ(ProfileManager::getActionTypeString(Action::Type::Unknown), "Unknown");
}