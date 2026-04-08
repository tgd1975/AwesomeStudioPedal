#include "config.h"

/**
 * @brief Hardware configuration for ESP32 development board
 * 
 * Defines the GPIO pin assignments for the Awesome Guitar Pedal hardware.
 * These pin assignments are specific to the NodeMCU-32S development board.
 */
const HardwareConfig hardwareConfig = {
    GPIO_NUM_26,   // ledBluetooth - Bluetooth status LED
    GPIO_NUM_25,   // ledPower      - Power indicator LED  
    GPIO_NUM_5,    // ledSelect1    - Bank 1 selection LED
    GPIO_NUM_18,   // ledSelect2    - Bank 2 selection LED
    GPIO_NUM_19,   // ledSelect3    - Bank 3 selection LED
    GPIO_NUM_21,   // buttonSelect  - Bank selection button
    GPIO_NUM_13,   // buttonA       - Action button A
    GPIO_NUM_12,   // buttonB       - Action button B
    GPIO_NUM_27,   // buttonC       - Action button C
    GPIO_NUM_14    // buttonD       - Action button D
};
