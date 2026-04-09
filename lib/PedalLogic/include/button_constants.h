#pragma once
#include <cstdint>

/**
 * @brief Button constants for pedal configuration
 *
 * These constants represent the physical buttons on the guitar pedal:
 * - BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D: Action buttons (0-3)
 * - BUTTON_BANK: Bank selection button (4)
 */
namespace Btn
{
    static constexpr uint8_t A = 0;    /**< Button A (index 0) */
    static constexpr uint8_t B = 1;    /**< Button B (index 1) */
    static constexpr uint8_t C = 2;    /**< Button C (index 2) */
    static constexpr uint8_t D = 3;    /**< Button D (index 3) */
    static constexpr uint8_t BANK = 4; /**< Bank selection button (index 4) */
}