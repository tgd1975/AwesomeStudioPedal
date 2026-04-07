#include <gtest/gtest.h>
#include <memory>
#include "../test/mock/mock_ble_keyboard.h"
#include "../src/bank_manager.h"
#include "../src/hardware/led_controller.h"

class BankManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockBleKeyboard = new MockBleKeyboard("Test", "Test");
        mockLED1 = new MockLEDController(GPIO_NUM_5);
        mockLED2 = new MockLEDController(GPIO_NUM_18);
        mockLED3 = new MockLEDController(GPIO_NUM_19);
        bankManager = new BankManager(*mockLED1, *mockLED2, *mockLED3);
    }
    
    void TearDown() override {
        delete mockBleKeyboard;
        delete mockLED1;
        delete mockLED2;
        delete mockLED3;
        delete bankManager;
    }
    
    MockBleKeyboard* mockBleKeyboard;
    MockLEDController* mockLED1;
    MockLEDController* mockLED2;
    MockLEDController* mockLED3;
    BankManager* bankManager;
};

TEST_F(BankManagerTest, AddAndGetAction) {
    auto mockAction = std::unique_ptr<Send>(new SendString(mockBleKeyboard, "Test"));
    
    bankManager->addAction(0, 0, std::move(mockAction));
    
    Send* retrieved = bankManager->getAction(0, 0);
    
    ASSERT_NE(retrieved, nullptr);
}

TEST_F(BankManagerTest, SwitchBankUpdatesLEDs) {
    EXPECT_CALL(*mockLED1, setState(true)).Times(1);
    EXPECT_CALL(*mockLED2, setState(false)).Times(1);
    EXPECT_CALL(*mockLED3, setState(false)).Times(1);
    
    bankManager->switchBank(); // Bank 0 -> Bank 1
    
    EXPECT_CALL(*mockLED1, setState(false)).Times(1);
    EXPECT_CALL(*mockLED2, setState(true)).Times(1);
    EXPECT_CALL(*mockLED3, setState(false)).Times(1);
    
    bankManager->switchBank(); // Bank 1 -> Bank 2
}
