#include <gtest/gtest.h>
#include "../test/mock/mock_button_controller.h"

TEST(ButtonControllerTest, SetupAndRead) {
    MockButtonController mockButton(GPIO_NUM_13);
    
    EXPECT_CALL(mockButton, setup())
        .Times(1);
    
    EXPECT_CALL(mockButton, read())
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(false));
    
    mockButton.setup();
    bool firstRead = mockButton.read();
    bool secondRead = mockButton.read();
    
    EXPECT_TRUE(firstRead);
    EXPECT_FALSE(secondRead);
}

TEST(ButtonControllerTest, MultipleSetupCalls) {
    MockButtonController mockButton(GPIO_NUM_13);
    
    EXPECT_CALL(mockButton, setup())
        .Times(2);
    
    mockButton.setup();
    mockButton.setup(); // Should be idempotent
}
