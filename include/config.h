#pragma once
#include <driver/gpio.h>

/**
 * @struct HardwareConfig
 * @brief Contains GPIO pin assignments for all hardware components
 * 
 * This structure defines the mapping between logical components
 * (LEDs, buttons) and physical GPIO pins on the ESP32.
 */
struct HardwareConfig {
    // LED pins
    gpio_num_t ledBluetooth; /**< Bluetooth status LED pin */
    gpio_num_t ledPower;     /**< Power indicator LED pin */
    gpio_num_t ledSelect1;   /**< Bank 1 selection LED pin */
    gpio_num_t ledSelect2;   /**< Bank 2 selection LED pin */
    gpio_num_t ledSelect3;   /**< Bank 3 selection LED pin */
    
    // Button pins
    gpio_num_t buttonSelect; /**< Bank selection button pin */
    gpio_num_t buttonA;      /**< Action button A pin */
    gpio_num_t buttonB;      /**< Action button B pin */
    gpio_num_t buttonC;      /**< Action button C pin */
    gpio_num_t buttonD;      /**< Action button D pin */
};

/**
 * @brief Global hardware configuration instance
 * 
 * Contains the actual GPIO pin assignments used throughout the application.
 */
extern const HardwareConfig hardwareConfig;
