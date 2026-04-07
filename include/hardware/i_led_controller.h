#pragma once
#include <cstdint>

class ILEDController {
public:
    virtual ~ILEDController() = default;
    virtual void setup(uint32_t initialState = 0) = 0;
    virtual void setState(bool state) = 0;
    virtual void toggle() = 0;
};
