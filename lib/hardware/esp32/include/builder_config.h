#pragma once
#include <cstdint>

// ===================================================================
// BUILDER CONFIGURATION — ESP32 (NodeMCU-32S)
// Edit these values to match your hardware wiring, then rebuild.
// Invalid combinations will produce a build error with a description.
// ===================================================================

// Number of profiles this build uses
static constexpr uint8_t CONF_NUM_PROFILES    = 7;

// Profile-select LEDs wired. Maximum profiles = 2^n − 1.
//   1 LED  → up to  1 profile
//   2 LEDs → up to  3 profiles
//   3 LEDs → up to  7 profiles
//   4 LEDs → up to 15 profiles
//   5 LEDs → up to 31 profiles
//   6 LEDs → up to 63 profiles
static constexpr uint8_t CONF_NUM_SELECT_LEDS = 3;

// Action buttons wired (A through Z, max 26)
static constexpr uint8_t CONF_NUM_BUTTONS     = 4;

// GPIO pin assignments
static constexpr uint8_t CONF_LED_BLUETOOTH   = 26;
static constexpr uint8_t CONF_LED_POWER       = 25;
static constexpr uint8_t CONF_LED_SELECT[]    = { 5, 18, 19 }; // CONF_NUM_SELECT_LEDS entries
static constexpr uint8_t CONF_BUTTON_SELECT   = 21;
static constexpr uint8_t CONF_BUTTON_PINS[]   = {              // CONF_NUM_BUTTONS entries
    13, // A
    12, // B
    27, // C
    14  // D
};

// ===================================================================
// VALIDATION — build fails here if your configuration is invalid.
// Read the message to understand what needs to change above.
// ===================================================================

static_assert(CONF_NUM_SELECT_LEDS >= 1 && CONF_NUM_SELECT_LEDS <= 6,
    "CONF_NUM_SELECT_LEDS must be between 1 and 6.");

static_assert(CONF_NUM_PROFILES >= 1 &&
              CONF_NUM_PROFILES <= (1u << CONF_NUM_SELECT_LEDS) - 1u,
    "CONF_NUM_PROFILES exceeds what your select LEDs can display. "
    "Either reduce CONF_NUM_PROFILES or wire more select LEDs (CONF_NUM_SELECT_LEDS).");

static_assert(CONF_NUM_BUTTONS >= 1 && CONF_NUM_BUTTONS <= 26,
    "CONF_NUM_BUTTONS must be between 1 and 26 (A-Z).");
