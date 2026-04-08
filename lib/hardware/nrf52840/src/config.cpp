#include "config.h"

/**
 * @brief Hardware configuration for Adafruit Feather nRF52840 Express
 *
 * Defines the GPIO pin assignments for the Awesome Guitar Pedal hardware
 * on the nRF52840 development board.
 *
 * Pin mapping (Feather nRF52840 Express):
 *  - D5  = 5   (ledBluetooth)
 *  - D6  = 6   (ledPower)
 *  - D9  = 9   (ledSelect1)
 *  - D10 = 10  (ledSelect2)
 *  - D11 = 11  (ledSelect3)
 *  - D12 = 12  (buttonSelect)
 *  - A0  = 14  (buttonA)
 *  - A1  = 15  (buttonB)
 *  - A2  = 16  (buttonC)
 *  - A3  = 17  (buttonD)
 */
const HardwareConfig hardwareConfig = {
    5,   // ledBluetooth - Bluetooth status LED
    6,   // ledPower      - Power indicator LED
    9,   // ledSelect1    - Bank 1 selection LED
    10,  // ledSelect2    - Bank 2 selection LED
    11,  // ledSelect3    - Bank 3 selection LED
    12,  // buttonSelect  - Bank selection button
    14,  // buttonA       - Action button A (A0)
    15,  // buttonB       - Action button B (A1)
    16,  // buttonC       - Action button C (A2)
    17   // buttonD       - Action button D (A3)
};
