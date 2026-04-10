#include "action.h"
#include "button_constants.h"
#include "mock_led_controller.h"
#include "profile.h"
#include "profile_manager.h"
#include "send_action.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Exactly;

// ---------------------------------------------------------------------------
// Minimal concrete Action for testing
// ---------------------------------------------------------------------------

class FakeAction : public Action
{
public:
    void execute() override {}
    Action::Type getType() const override { return Action::Type::SendString; }
};

// ---------------------------------------------------------------------------
// Helper — build a profile with a single FakeAction on button A
// ---------------------------------------------------------------------------

static std::unique_ptr<Profile> makeProfile(const char* name)
{
    auto p = std::make_unique<Profile>(name);
    p->addAction(0 /*A*/, std::make_unique<FakeAction>());
    return p;
}

// ---------------------------------------------------------------------------
// Fixture — all LED calls allowed by default; individual tests tighten where needed
// ---------------------------------------------------------------------------

class ProfileManagerTest : public ::testing::Test
{
protected:
    MockLEDController led1, led2, led3;
    ProfileManager* manager = nullptr;

    void SetUp() override
    {
        allowAllLEDCalls(led1);
        allowAllLEDCalls(led2);
        allowAllLEDCalls(led3);
        manager = new ProfileManager({&led1, &led2, &led3});
    }

    void TearDown() override { delete manager; }

    static void allowAllLEDCalls(MockLEDController& led)
    {
        EXPECT_CALL(led, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led, update(_)).Times(AnyNumber());
        EXPECT_CALL(led, isBlinking()).Times(AnyNumber());
    }

    // Populate slots 0..n-1 with dummy profiles
    void populateSlots(uint8_t n)
    {
        char name[8];
        for (uint8_t i = 0; i < n && i < ProfileManager::MAX_PROFILES; i++)
        {
            snprintf(name, sizeof(name), "P%d", i);
            manager->addProfile(i, makeProfile(name));
        }
    }

    // Advance to a specific profile index by repeatedly calling switchProfile,
    // then drain the blink so updateLEDs() runs with the correct state
    void switchTo(uint8_t target)
    {
        while (manager->getCurrentProfile() != target)
            manager->switchProfile();
        manager->update(1000); // complete post-switch blink
    }
};

// ---------------------------------------------------------------------------
// Basic state
// ---------------------------------------------------------------------------

TEST_F(ProfileManagerTest, StartsAtProfile0) { EXPECT_EQ(manager->getCurrentProfile(), 0); }

TEST_F(ProfileManagerTest, SwitchProfileWrapsAfterSeven)
{
    // With no profiles loaded every switch skips all empty slots and stays at 0
    for (int i = 0; i < 7; i++)
        manager->switchProfile();
    EXPECT_EQ(manager->getCurrentProfile(), 0);
}

TEST_F(ProfileManagerTest, AddAndGetProfileReturnsCorrectAction)
{
    auto profile = std::make_unique<Profile>("Test");
    auto action = std::make_unique<FakeAction>();
    Action* rawPtr = action.get();
    profile->addAction(2 /*C*/, std::move(action));
    manager->addProfile(1, std::move(profile));
    EXPECT_EQ(manager->getAction(1, 2 /*C*/), rawPtr);
}

TEST_F(ProfileManagerTest, GetActionOutOfBoundsReturnsNullptr)
{
    EXPECT_EQ(manager->getAction(ProfileManager::MAX_PROFILES, 0 /*A*/), nullptr);
    EXPECT_EQ(manager->getAction(0, Profile::MAX_BUTTONS), nullptr); // out of button range
}

TEST_F(ProfileManagerTest, GetActionEmptySlotReturnsNullptr)
{
    EXPECT_EQ(manager->getAction(0, 0 /*A*/), nullptr);
}

TEST_F(ProfileManagerTest, GetProfileNameReturnsCorrectName)
{
    manager->addProfile(0, makeProfile("TestProfile"));
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

// ---------------------------------------------------------------------------
// LED encoding — verify exact setState calls after blink completes
// ---------------------------------------------------------------------------

// Simple LED spy that records the last setState value
class SpyLEDController : public ILEDController
{
public:
    bool lastState = false;
    void setup(uint32_t) override {}
    void setState(bool s) override { lastState = s; }
    void toggle() override { lastState = ! lastState; }
    void startBlink(uint32_t, int16_t) override {}
    void stopBlink() override {}
    void update(uint32_t) override {}
    bool isBlinking() const override { return false; }
};

// 3 LEDs, 7 profiles (binary mode) — expected pattern for each profile index
// Profile 0→binary 1: 001, 1→010, 2→011, 3→100, 4→101, 5→110, 6→111
static void assertLEDPattern(uint8_t profileIndex, uint8_t pattern)
{
    SpyLEDController l1, l2, l3;
    ProfileManager pm({&l1, &l2, &l3});

    // Populate all slots up to and including the target
    char name[8];
    for (uint8_t i = 0; i <= profileIndex; i++)
    {
        snprintf(name, sizeof(name), "P%d", i);
        pm.addProfile(i, makeProfile(name));
    }

    // Advance to target profile
    while (pm.getCurrentProfile() != profileIndex)
        pm.switchProfile();

    // Drive updateLEDs: for profile 0 use resetToFirstProfile(),
    // for others complete the post-switch blink via update().
    if (profileIndex == 0)
    {
        pm.resetToFirstProfile();
    }
    else
    {
        pm.update(1);    // initialise blink: sets blinkStartTime=1, turns all LEDs on
        pm.update(5000); // far enough ahead to complete all half-cycles → calls updateLEDs
    }

    EXPECT_EQ(l1.lastState, (pattern & 0b001) != 0)
        << "LED1 wrong for profile " << (int) profileIndex;
    EXPECT_EQ(l2.lastState, (pattern & 0b010) != 0)
        << "LED2 wrong for profile " << (int) profileIndex;
    EXPECT_EQ(l3.lastState, (pattern & 0b100) != 0)
        << "LED3 wrong for profile " << (int) profileIndex;
}

// hardwareConfig fake: 7 profiles, 3 select LEDs → binary mode
// Binary encoding: 1-based profile number encoded in binary
// Profile 0 → bits=1 → 001 (LED1)
// Profile 1 → bits=2 → 010 (LED2)
// Profile 2 → bits=3 → 011 (LED1+LED2)
// Profile 3 → bits=4 → 100 (LED3)
// Profile 4 → bits=5 → 101 (LED1+LED3)
// Profile 5 → bits=6 → 110 (LED2+LED3)
// Profile 6 → bits=7 → 111 (LED1+LED2+LED3)
TEST_F(ProfileManagerTest, LEDEncoding_Profile0_LED1Only)     { assertLEDPattern(0, 0b001); }
TEST_F(ProfileManagerTest, LEDEncoding_Profile1_LED2Only)     { assertLEDPattern(1, 0b010); }
TEST_F(ProfileManagerTest, LEDEncoding_Profile2_LED1andLED2)  { assertLEDPattern(2, 0b011); }
TEST_F(ProfileManagerTest, LEDEncoding_Profile3_LED3Only)     { assertLEDPattern(3, 0b100); }
TEST_F(ProfileManagerTest, LEDEncoding_Profile4_LED1andLED3)  { assertLEDPattern(4, 0b101); }
TEST_F(ProfileManagerTest, LEDEncoding_Profile5_LED2andLED3)  { assertLEDPattern(5, 0b110); }
TEST_F(ProfileManagerTest, LEDEncoding_Profile6_AllLEDs)      { assertLEDPattern(6, 0b111); }

// ---------------------------------------------------------------------------
// switchProfile skips empty slots
// ---------------------------------------------------------------------------

TEST_F(ProfileManagerTest, SwitchSkipsEmptySlots_LandsOnNextPopulated)
{
    // Populate only slots 0 and 3
    manager->addProfile(0, makeProfile("P0"));
    manager->addProfile(3, makeProfile("P3"));

    manager->switchProfile(); // should skip 1, 2 and land on 3
    manager->update(1);
    manager->update(5000);

    EXPECT_EQ(manager->getCurrentProfile(), 3);
}

TEST_F(ProfileManagerTest, SwitchWrapsAroundToFirstPopulated)
{
    manager->addProfile(0, makeProfile("P0"));
    manager->addProfile(3, makeProfile("P3"));

    manager->switchProfile(); // 0 -> 3
    manager->update(1);
    manager->update(5000);
    manager->switchProfile(); // 3 -> wraps back to 0
    manager->update(5001);
    manager->update(10000);

    EXPECT_EQ(manager->getCurrentProfile(), 0);
}

// ---------------------------------------------------------------------------
// resetToFirstProfile
// ---------------------------------------------------------------------------

TEST_F(ProfileManagerTest, ResetToFirstProfile_LandsOnFirstPopulated)
{
    manager->addProfile(2, makeProfile("P2"));
    manager->addProfile(5, makeProfile("P5"));
    manager->resetToFirstProfile();
    EXPECT_EQ(manager->getCurrentProfile(), 2);
}

TEST_F(ProfileManagerTest, ResetToFirstProfile_AllEmpty_LandsOn0)
{
    manager->addProfile(0, makeProfile("P0"));
    manager->addProfile(0, nullptr); // clear it
    manager->resetToFirstProfile();
    EXPECT_EQ(manager->getCurrentProfile(), 0);
}

// ---------------------------------------------------------------------------
// Constructor sets correct initial LED state (profile 0, binary mode: bits=1 → LED1 on)
// ---------------------------------------------------------------------------

TEST_F(ProfileManagerTest, ConstructorSetsProfile0LEDs)
{
    MockLEDController l1, l2, l3;
    EXPECT_CALL(l1, setState(true)).Times(Exactly(1));
    EXPECT_CALL(l2, setState(false)).Times(Exactly(1));
    EXPECT_CALL(l3, setState(false)).Times(Exactly(1));
    ProfileManager bm({&l1, &l2, &l3});
}

// ---------------------------------------------------------------------------
// Post-switch blink: LED state immediately after switch vs after update()
// ---------------------------------------------------------------------------

TEST_F(ProfileManagerTest, PostSwitchBlink_AllLEDsOnImmediately)
{
    MockLEDController l1, l2, l3;
    EXPECT_CALL(l1, setState(_)).Times(AnyNumber());
    EXPECT_CALL(l2, setState(_)).Times(AnyNumber());
    EXPECT_CALL(l3, setState(_)).Times(AnyNumber());
    ProfileManager pm({&l1, &l2, &l3});
    pm.addProfile(0, makeProfile("P0"));
    pm.addProfile(1, makeProfile("P1"));

    pm.switchProfile();

    EXPECT_CALL(l1, setState(true)).Times(::testing::AtLeast(1));
    EXPECT_CALL(l2, setState(true)).Times(::testing::AtLeast(1));
    EXPECT_CALL(l3, setState(true)).Times(::testing::AtLeast(1));
    pm.update(1);
}

TEST_F(ProfileManagerTest, SwitchProfileCallsUpdateLEDs)
{
    MockLEDController l1, l2, l3;
    // Constructor at profile 0 (binary: bits=1 → LED1 on).
    // switchProfile goes 0->1 (binary: bits=2 → LED2 on, LED1/LED3 off).
    EXPECT_CALL(l1, setState(_)).Times(AnyNumber());
    EXPECT_CALL(l2, setState(true)).Times(::testing::AtLeast(1));
    EXPECT_CALL(l2, setState(false)).Times(AnyNumber());
    EXPECT_CALL(l3, setState(_)).Times(AnyNumber());
    ProfileManager bm({&l1, &l2, &l3});
    bm.addProfile(0, makeProfile("P0"));
    bm.addProfile(1, makeProfile("P1"));
    bm.switchProfile();
    bm.update(1);
    bm.update(5000);
}
