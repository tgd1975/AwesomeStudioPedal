#include <gtest/gtest.h>
#include "../test/mock/mock_led_controller.h"

TEST(LEDControllerTest, SetupAndSetState) {
    MockLEDController mockLED(GPIO_NUM_26);
    
    EXPECT_CALL(mockLED, setup(0))
        .Times(1);
    
    EXPECT_CALL(mockLED, setState(true))
        .Times(1);
    
    mockLED.setup(0);
    mockLED.setState(true);
}

TEST(LEDControllerTest, ToggleFunctionality) {
    MockLEDController mockLED(GPIO_NUM_5);
    
    EXPECT_CALL(mockLED, setup(1))
        .Times(1);
    
    EXPECT_CALL(mockLED, setState(false))
        .Times(1);
    
    EXPECT_CALL(mockLED, toggle())
        .Times(1);
    
    mockLED.setup(1);
    mockLED.setState(false);
    mockLED.toggle();
}
