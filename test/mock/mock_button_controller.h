#pragma once
#include "../src/hardware/button_controller.h"
#include <gmock/gmock.h>

class MockButtonController : public ButtonController {
public:
    MockButtonController(gpio_num_t pin) : ButtonController(pin) {}
    
    MOCK_METHOD(void, setup, (), (override));
    MOCK_METHOD(bool, read, (), (override));
};
