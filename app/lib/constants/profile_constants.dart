// Maximum number of profiles the pedal can address. Tied to the 3-bit
// LED selector on the current hardware (LED1, LED2, LED3 → 2³−1 = 7
// non-zero patterns, since pattern 0 / "all LEDs off" is reserved for
// "no profile selected"). Mirrored in data/profiles.schema.json as
// `maxItems: 7` on the profiles array.
//
// The firmware itself (lib/PedalLogic/include/profile_manager.h) caps
// at MAX_PROFILES=63 (6-bit ceiling), but no current board exposes
// more than 3 select LEDs, so the user-facing cap is 7. Bump this if
// a future hardware variant adds a 4th select LED.
const int kMaxProfiles = 7;
