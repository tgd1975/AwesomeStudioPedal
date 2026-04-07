#pragma once
#include "i_button_controller.h"
#include <driver/gpio.h>
#include <Arduino.h>

class ButtonController : public IButtonController {
public:
    ButtonController(gpio_num_t pin);

    virtual void setup() override;
    virtual bool read() override;

private:
    gpio_num_t pin;
};
