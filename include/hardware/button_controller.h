#pragma once
#include <driver/gpio.h>
#include <Arduino.h>

class ButtonController {
public:
    ButtonController(gpio_num_t pin);
    
    void setup();
    bool read();
    
private:
    gpio_num_t pin;
};
