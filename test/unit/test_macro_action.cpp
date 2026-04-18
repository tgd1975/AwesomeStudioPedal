#include "arduino_shim.h"
#include "button_constants.h"
#include "config_loader.h"
#include "delayed_action.h"
#include "macro_action.h"
#include "mock_ble_keyboard.h"
#include "mock_led_controller.h"
#include "profile_manager.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using ::testing::_;
using ::testing::AnyNumber;

// ---------------------------------------------------------------------------
// Minimal hand-written test double
// ---------------------------------------------------------------------------

class StubAction : public Action
{
public:
    explicit StubAction(bool staysInProgress = false) : staysInProgress_(staysInProgress) {}

    void execute() override { executeCount_++; }
    void executeRelease() override { releaseCount_++; }
    bool isInProgress() const override { return staysInProgress_; }
    Action::Type getType() const override { return Action::Type::SerialOutput; }

    void setInProgress(bool v) { staysInProgress_ = v; }

    int executeCount_ = 0;
    int releaseCount_ = 0;

private:
    bool staysInProgress_;
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static MacroAction::Step makeStep(std::unique_ptr<Action> a)
{
    MacroAction::Step s;
    s.push_back(std::move(a));
    return s;
}

static MacroAction::Step makeStep2(std::unique_ptr<Action> a, std::unique_ptr<Action> b)
{
    MacroAction::Step s;
    s.push_back(std::move(a));
    s.push_back(std::move(b));
    return s;
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(MacroActionTest, GetType_ReturnsMacro)
{
    MacroAction m;
    EXPECT_EQ(m.getType(), Action::Type::Macro);
}

TEST(MacroActionTest, EmptyMacro_NotInProgress)
{
    MacroAction m;
    EXPECT_FALSE(m.isInProgress());
}

TEST(MacroActionTest, EmptyMacro_Execute_NoEffect)
{
    MacroAction m;
    m.execute();
    EXPECT_FALSE(m.isInProgress());
}

TEST(MacroActionTest, SingleStep_SingleAction_FiresOnExecute)
{
    auto* stub = new StubAction(false);
    MacroAction m;
    m.addStep(makeStep(std::unique_ptr<Action>(stub)));

    m.execute();

    EXPECT_EQ(stub->executeCount_, 1);
    // Synchronous action completes immediately — macro is done
    EXPECT_FALSE(m.isInProgress());
}

TEST(MacroActionTest, SingleStep_TwoParallelActions_BothFire)
{
    auto* a = new StubAction(false);
    auto* b = new StubAction(false);
    MacroAction m;
    m.addStep(makeStep2(std::unique_ptr<Action>(a), std::unique_ptr<Action>(b)));

    m.execute();

    EXPECT_EQ(a->executeCount_, 1);
    EXPECT_EQ(b->executeCount_, 1);
    EXPECT_FALSE(m.isInProgress());
}

TEST(MacroActionTest, TwoStepSequential_Step2NotFiredUntilStep1Done)
{
    auto* step1 = new StubAction(true); // stays in-progress
    auto* step2 = new StubAction(false);

    MacroAction m;
    m.addStep(makeStep(std::unique_ptr<Action>(step1)));
    m.addStep(makeStep(std::unique_ptr<Action>(step2)));

    m.execute();

    // Step 1 fired but not done yet
    EXPECT_EQ(step1->executeCount_, 1);
    EXPECT_EQ(step2->executeCount_, 0);
    EXPECT_TRUE(m.isInProgress());

    // Simulate step 1 completing
    step1->setInProgress(false);
    m.update();

    // Now step 2 should have fired and macro is done
    EXPECT_EQ(step2->executeCount_, 1);
    EXPECT_FALSE(m.isInProgress());
}

TEST(MacroActionTest, Update_WhileNotRunning_IsNoop)
{
    MacroAction m;
    m.update(); // must not crash
    EXPECT_FALSE(m.isInProgress());
}

TEST(MacroActionTest, IsInProgress_TrueWhileRunning)
{
    auto* step1 = new StubAction(true);
    MacroAction m;
    m.addStep(makeStep(std::unique_ptr<Action>(step1)));
    m.execute();
    EXPECT_TRUE(m.isInProgress());
}

TEST(MacroActionTest, ExecuteRelease_PropagatestoCurrentStep)
{
    auto* step1 = new StubAction(true);
    MacroAction m;
    m.addStep(makeStep(std::unique_ptr<Action>(step1)));
    m.execute();
    m.executeRelease();
    EXPECT_EQ(step1->releaseCount_, 1);
}

TEST(MacroActionTest, SynchronousSteps_AdvanceImmediately)
{
    auto* s1 = new StubAction(false);
    auto* s2 = new StubAction(false);
    auto* s3 = new StubAction(false);
    MacroAction m;
    m.addStep(makeStep(std::unique_ptr<Action>(s1)));
    m.addStep(makeStep(std::unique_ptr<Action>(s2)));
    m.addStep(makeStep(std::unique_ptr<Action>(s3)));

    m.execute();

    EXPECT_EQ(s1->executeCount_, 1);
    EXPECT_EQ(s2->executeCount_, 1);
    EXPECT_EQ(s3->executeCount_, 1);
    EXPECT_FALSE(m.isInProgress());
}

// ---------------------------------------------------------------------------
// Delayed step: macro waits until delay elapses
// ---------------------------------------------------------------------------

TEST(MacroActionTest, DelayedStep_DoesNotAdvanceBeforeTimeout)
{
    fake_time::value = 0;

    auto inner = std::make_unique<StubAction>(false);
    StubAction* innerPtr = inner.get();
    auto delayed = std::make_unique<DelayedAction>(std::move(inner), 500);

    auto* step2 = new StubAction(false);

    MacroAction m;
    m.addStep(makeStep(std::move(delayed)));
    m.addStep(makeStep(std::unique_ptr<Action>(step2)));

    m.execute(); // fires delayed action step (starts timer)
    EXPECT_TRUE(m.isInProgress());
    EXPECT_EQ(step2->executeCount_, 0);

    // Advance time but not past threshold
    fake_time::value = 300;
    m.update();
    EXPECT_EQ(step2->executeCount_, 0);
    EXPECT_TRUE(m.isInProgress());

    // Call execute() on the in-progress delayed action (as main loop does), then update macro
    fake_time::value = 600;
    // The delayed action's isInProgress drops to false after execute fires inner action
    Action* delayedActionPtr = nullptr; // we exercise via macro's update below

    // Manually drive the DelayedAction to completion the same way main.cpp does
    // (polling execute while isInProgress)
    // We need to get the first step's action — exercise through macro directly.
    // Instead, create a fresh scenario where we wrap in macro and poll properly.
    (void) innerPtr;
    (void) delayedActionPtr;
}

TEST(MacroActionTest, DelayedStep_AdvancesAfterTimeout)
{
    fake_time::value = 0;

    auto inner = std::make_unique<StubAction>(false);
    auto delayed = std::make_unique<DelayedAction>(std::move(inner), 500);

    auto* step2 = new StubAction(false);

    MacroAction m;
    m.addStep(makeStep(std::move(delayed)));
    m.addStep(makeStep(std::unique_ptr<Action>(step2)));

    m.execute(); // starts the delay timer on the DelayedAction
    EXPECT_TRUE(m.isInProgress());

    // Advance past delay threshold; update() polls the DelayedAction and advances macro
    fake_time::value = 600;
    m.update(); // polls DelayedAction::execute() (fires inner, clears started), then advances

    EXPECT_EQ(step2->executeCount_, 1);
    EXPECT_FALSE(m.isInProgress());
}

// ---------------------------------------------------------------------------
// Config loader round-trip
// ---------------------------------------------------------------------------

class MacroConfigTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EXPECT_CALL(led1, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(AnyNumber());
        pm = std::make_unique<ProfileManager>(std::vector<ILEDController*>{&led1, &led2, &led3});
    }

    MockLEDController led1, led2, led3;
    MockBleKeyboard kb;
    std::unique_ptr<ProfileManager> pm;
    ConfigLoader loader;
};

TEST_F(MacroConfigTest, ThreeStepMacro_ParsesCorrectType)
{
    const char* json = R"({
        "profiles":[{"name":"T","buttons":{"A":{
            "type":"MacroAction",
            "steps":[
                [{"type":"PinHighAction","pin":27}],
                [{"type":"SerialOutputAction","value":"step2"},{"type":"SerialOutputAction","value":"step2b"}],
                [{"type":"PinLowAction","pin":27}]
            ]
        }}}]})";
    loader.loadFromString(*pm, &kb, json);
    Action* a = pm->getAction(0, Btn::A);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::Macro);
}

TEST_F(MacroConfigTest, MacroWithEmptySteps_ParsesAndDoesNotCrash)
{
    const char* json =
        R"({"profiles":[{"name":"T","buttons":{"A":{"type":"MacroAction","steps":[]}}}]})";
    loader.loadFromString(*pm, &kb, json);
    Action* a = pm->getAction(0, Btn::A);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::Macro);
    a->execute(); // must not crash
}

TEST_F(MacroConfigTest, MacroWithMissingSteps_ParsesAndDoesNotCrash)
{
    const char* json =
        R"({"profiles":[{"name":"T","buttons":{"A":{"type":"MacroAction"}}}]})";
    loader.loadFromString(*pm, &kb, json);
    Action* a = pm->getAction(0, Btn::A);
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a->getType(), Action::Type::Macro);
    a->execute();
}
