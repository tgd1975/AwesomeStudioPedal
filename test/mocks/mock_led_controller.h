#pragma once
#include "hardware/i_led_controller.h"
#include <gmock/gmock.h>

class MockLEDController : public ILEDController {
public:
    MOCK_METHOD(void, setup, (uint32_t initialState), (override));
    MOCK_METHOD(void, setState, (bool state), (override));
    MOCK_METHOD(void, toggle, (), (override));
};
