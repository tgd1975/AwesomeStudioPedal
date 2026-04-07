#pragma once

class IButtonController {
public:
    virtual ~IButtonController() = default;
    virtual void setup() = 0;
    virtual bool read() = 0;
};
