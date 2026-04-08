#pragma once
#include <cstdint>

/**
 * @struct HardwareConfig
 * @brief Platform-independent hardware pin configuration
 *
 * Uses uint8_t for pin numbers so this header compiles on any Arduino
 * target without pulling in ESP-IDF or platform-specific headers.
 * Each hardware package provides its own config.cpp with the concrete
 * pin assignments for that board.
 */
struct HardwareConfig
{
    // LED pins
    uint8_t ledBluetooth; /**< Bluetooth status LED pin */
    uint8_t ledPower;     /**< Power indicator LED pin */
    uint8_t ledSelect1;   /**< Bank 1 selection LED pin */
    uint8_t ledSelect2;   /**< Bank 2 selection LED pin */
    uint8_t ledSelect3;   /**< Bank 3 selection LED pin */

    // Button pins
    uint8_t buttonSelect; /**< Bank selection button pin */
    uint8_t buttonA;      /**< Action button A pin */
    uint8_t buttonB;      /**< Action button B pin */
    uint8_t buttonC;      /**< Action button C pin */
    uint8_t buttonD;      /**< Action button D pin */
};

/**
 * @brief Global hardware configuration instance
 *
 * Defined in the platform-specific config.cpp inside each hardware package.
 */
extern const HardwareConfig hardwareConfig;
