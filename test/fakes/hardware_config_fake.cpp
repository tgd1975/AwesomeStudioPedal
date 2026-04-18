#include "config.h"

/**
 * @brief Fake hardwareConfig for host unit tests.
 *
 * Matches the default build: 7 profiles, 3 select LEDs, 4 buttons (A-D).
 * Tests that exercise specific pin values should not rely on these numbers;
 * this fake exists only to satisfy the linker.
 */
HardwareConfig hardwareConfig = {
    7,                    // numProfiles
    3,                    // numSelectLeds
    4,                    // numButtons
    26,                   // ledBluetooth
    25,                   // ledPower
    {5, 18, 19, 0, 0, 0}, // ledSelect
    21,                   // buttonSelect
    {13, 12, 27, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // buttonPins
};
