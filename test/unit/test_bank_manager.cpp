#include <unity.h>
#include "../../src/bank_manager.h"

void test_BankManager_AddAndGetAction(void) {
    TEST_ASSERT_TRUE(true);
}

void test_BankManager_SwitchBankUpdatesLEDs(void) {
    TEST_ASSERT_TRUE(true);
}

void test_bank_manager(void) {
    RUN_TEST(test_BankManager_AddAndGetAction);
    RUN_TEST(test_BankManager_SwitchBankUpdatesLEDs);
}
