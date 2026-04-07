#pragma once
#include <driver/gpio.h>

struct HardwareConfig {
    // LED pins
    gpio_num_t ledBluetooth;
    gpio_num_t ledPower;
    gpio_num_t ledSelect1;
    gpio_num_t ledSelect2;
    gpio_num_t ledSelect3;
    
    // Button pins
    gpio_num_t buttonSelect;
    gpio_num_t buttonA;
    gpio_num_t buttonB;
    gpio_num_t buttonC;
    gpio_num_t buttonD;
};

extern const HardwareConfig hardwareConfig;
