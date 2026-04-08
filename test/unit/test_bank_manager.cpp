#include "bank_manager.h"
#include "mock_led_controller.h"
#include "send.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Exactly;

// Minimal concrete Send subclass for testing
class FakeSend : public Send
{
public:
    FakeSend(IBleKeyboard* kb) : Send(kb) {}
    void send() override {}
};

class BankManagerTest : public ::testing::Test
{
protected:
    MockLEDController led1, led2, led3;
    BankManager* manager;

    void SetUp() override
    {
        EXPECT_CALL(led1, setState(_)).Times(::testing::AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(::testing::AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(::testing::AnyNumber());
        manager = new BankManager(led1, led2, led3);
    }

    void TearDown() override { delete manager; }
};

TEST_F(BankManagerTest, StartsAtBank0) { EXPECT_EQ(manager->getCurrentBank(), 0); }

TEST_F(BankManagerTest, SwitchBankWrapsAfterThree)
{
    EXPECT_CALL(led1, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3, setState(_)).Times(::testing::AnyNumber());
    manager->switchBank(); // -> 1
    manager->switchBank(); // -> 2
    manager->switchBank(); // -> 0
    EXPECT_EQ(manager->getCurrentBank(), 0);
}

TEST_F(BankManagerTest, AddAndGetActionReturnsCorrectAction)
{
    MockLEDController led1b, led2b, led3b;
    EXPECT_CALL(led1b, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led2b, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(led3b, setState(_)).Times(::testing::AnyNumber());
    BankManager bm(led1b, led2b, led3b);
    auto action = std::make_unique<FakeSend>(nullptr);
    FakeSend* rawPtr = action.get();
    bm.addAction(0, 2, std::move(action));
    EXPECT_EQ(bm.getAction(0, 2), rawPtr);
}

TEST_F(BankManagerTest, GetActionOutOfBoundsReturnsNullptr)
{
    EXPECT_EQ(manager->getAction(3, 0), nullptr);
    EXPECT_EQ(manager->getAction(0, 4), nullptr);
}

TEST_F(BankManagerTest, GetActionEmptySlotReturnsNullptr)
{
    EXPECT_EQ(manager->getAction(0, 0), nullptr);
}

TEST_F(BankManagerTest, ConstructorSetsBank0LEDs)
{
    MockLEDController l1, l2, l3;
    EXPECT_CALL(l1, setState(true)).Times(Exactly(1));
    EXPECT_CALL(l2, setState(false)).Times(Exactly(1));
    EXPECT_CALL(l3, setState(false)).Times(Exactly(1));
    BankManager bm(l1, l2, l3);
}

TEST_F(BankManagerTest, SwitchBankCallsUpdateLEDs)
{
    MockLEDController l1, l2, l3;
    // Constructor sets bank 0 (l1=true); switchBank goes 0->1 (l2=true)
    EXPECT_CALL(l1, setState(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(l2, setState(true)).Times(::testing::AtLeast(1));
    EXPECT_CALL(l2, setState(false)).Times(::testing::AnyNumber());
    EXPECT_CALL(l3, setState(_)).Times(::testing::AnyNumber());
    BankManager bm(l1, l2, l3);
    bm.switchBank();
}
