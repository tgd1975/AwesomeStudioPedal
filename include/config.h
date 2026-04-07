#pragma once
#include <driver/gpio.h>

struct HardwareConfig {
    // LED pins
    gpio_num_t ledBluetooth = GPIO_NUM_26;
    gpio_num_t ledPower = GPIO_NUM_25;
    gpio_num_t ledSelect1 = GPIO_NUM_5;
    gpio_num_t ledSelect2 = GPIO_NUM_18;
    gpio_num_t ledSelect3 = GPIO_NUM_19;
    
    // Button pins
    gpio_num_t buttonSelect = GPIO_NUM_21;
    gpio_num_t buttonA = GPIO_NUM_13;
    gpio_num_t buttonB = GPIO_NUM_12;
    gpio_num_t buttonC = GPIO_NUM_27;
    gpio_num_t buttonD = GPIO_NUM_14;
};

extern const HardwareConfig hardwareConfig;EOF