#pragma once
#include "i_led_controller.h"
#include <gmock/gmock.h>

class MockLEDController : public ILEDController
{
public:
    MOCK_METHOD(void, setup,      (uint32_t initialState), (override));
    MOCK_METHOD(void, setState,   (bool state),            (override));
    MOCK_METHOD(void, toggle,     (),                      (override));
    MOCK_METHOD(void, startBlink, (uint32_t intervalMs, int16_t count), (override));
    MOCK_METHOD(void, stopBlink,  (),                      (override));
    MOCK_METHOD(void, update,     (uint32_t now),          (override));
    MOCK_METHOD(bool, isBlinking, (),                      (const, override));
};
