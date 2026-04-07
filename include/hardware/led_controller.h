#pragma once
#include <driver/gpio.h>
#include <Arduino.h>

class LEDController {
public:
    LEDController(gpio_num_t pin);
    
    void setup(uint32_t initialState = 0);
    void setState(bool state);
    void toggle();
    
private:
    gpio_num_t pin;
    bool currentState = false;
};
