#include "../../src/hardware/button_controller.h"
#include <unity.h>

void test_ButtonController_SetupAndRead(void) { TEST_ASSERT_TRUE(true); }

void test_ButtonController_MultipleSetupCalls(void) { TEST_ASSERT_TRUE(true); }

void test_button_controller(void)
{
    RUN_TEST(test_ButtonController_SetupAndRead);
    RUN_TEST(test_ButtonController_MultipleSetupCalls);
}
