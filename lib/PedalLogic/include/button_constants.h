#pragma once
#include <cstdint>

/**
 * @brief Button helpers for pedal configuration
 *
 * Buttons are identified by letter (A, B, C, … Z) everywhere:
 * in JSON, in serial output, and in log lines.
 *
 * Index 0 = A, 1 = B, ..., 25 = Z.
 * The active count is set at runtime via hardwareConfig.numButtons.
 */
namespace Btn
{
    static constexpr uint8_t MAX = 26; /**< Maximum number of action buttons (A–Z) */

    // Named index constants for convenience (A=0, B=1, …)
    static constexpr uint8_t A = 0;
    static constexpr uint8_t B = 1;
    static constexpr uint8_t C = 2;
    static constexpr uint8_t D = 3;

    /**
     * @brief Write the letter name for a button index into buf.
     *
     * @param index  Button index (0 = A, 1 = B, ..., 25 = Z)
     * @param buf    Output buffer — must be at least 2 bytes
     */
    inline void name(uint8_t index, char* buf)
    {
        buf[0] = static_cast<char>('A' + index);
        buf[1] = '\0';
    }
} // namespace Btn
