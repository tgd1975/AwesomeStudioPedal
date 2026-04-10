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
 *
 * Runtime active counts (numProfiles, numSelectLeds, numButtons) control
 * loop bounds throughout the firmware. The compile-time ceilings in
 * ProfileManager and Profile determine array sizes only.
 */
struct HardwareConfig
{
    uint8_t numProfiles;   /**< Number of active profiles (1..MAX_PROFILES) */
    uint8_t numSelectLeds; /**< Profile-select LEDs wired; caps profiles at 2^n − 1 */
    uint8_t numButtons;    /**< Action buttons wired (1..26, A–Z) */

    // LED pins
    uint8_t ledBluetooth; /**< Bluetooth status LED pin */
    uint8_t ledPower;     /**< Power indicator LED pin */
    uint8_t ledSelect[6]; /**< Profile-select LED pins (up to 6) */

    // Button pins
    uint8_t buttonSelect;   /**< Profile-cycle button pin */
    uint8_t buttonPins[26]; /**< Action button pins: index 0 = A, 1 = B, ..., 25 = Z */
};

/**
 * @brief Global hardware configuration instance
 *
 * Defined in the platform-specific config.cpp inside each hardware package.
 */
extern const HardwareConfig hardwareConfig;
