#pragma once
#include "i_led_controller.h"
#include <driver/gpio.h>
#include <Arduino.h>

class LEDController : public ILEDController {
public:
    LEDController(gpio_num_t pin);

    virtual void setup(uint32_t initialState = 0) override;
    virtual void setState(bool state) override;
    virtual void toggle() override;

private:
    gpio_num_t pin;
    bool currentState = false;
};
