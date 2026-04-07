#pragma once
#include "../src/hardware/led_controller.h"
#include <gmock/gmock.h>

class MockLEDController : public LEDController {
public:
    MockLEDController(gpio_num_t pin) : LEDController(pin) {}
    
    MOCK_METHOD(void, setup, (uint32_t initialState), (override));
    MOCK_METHOD(void, setState, (bool state), (override));
    MOCK_METHOD(void, toggle, (), (override));
};
