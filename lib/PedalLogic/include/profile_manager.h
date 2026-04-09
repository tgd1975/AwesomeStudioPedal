#pragma once
#include "i_led_controller.h"
#include "profile.h"
#include <array>
#include <memory>

/**
 * @class ProfileManager
 * @brief Manages up to 7 profiles with LED feedback
 *
 * LED encoding:
 *   - Profiles 1–3: one LED on exclusively (same as before)
 *   - Profiles 4–7: binary encoding across 3 LEDs
 *     bits = profileIndex + 1  →  LED1 = bit0, LED2 = bit1, LED3 = bit2
 *
 * After a profile switch all three LEDs blink 3 times, then return to the
 * encoding for the new profile.  Drive this by calling update() every loop.
 */
class ProfileManager
{
public:
    static constexpr uint8_t NUM_PROFILES = 7;

    ProfileManager(ILEDController& led1, ILEDController& led2, ILEDController& led3);

    void addProfile(uint8_t profileIndex, std::unique_ptr<Profile> profile);
    Action* getAction(uint8_t profileIndex, uint8_t button) const;
    const Profile* getProfile(uint8_t profileIndex) const;

    /**
     * @brief Advance to the next populated profile slot and trigger blink feedback
     *
     * Skips empty slots and wraps around.  Triggers a 3-blink sequence on all
     * LEDs; the LEDs return to the profile encoding after the sequence completes.
     *
     * @return New current profile index
     */
    uint8_t switchProfile();

    uint8_t getCurrentProfile() const { return currentProfile; }
    const std::string& getProfileName(uint8_t profileIndex) const;

    /**
     * @brief Reset to the first populated slot (or slot 0 if all empty)
     *
     * Called after loadFromString clears and repopulates all slots so that
     * currentProfile is never left pointing at an empty slot.
     */
    void resetToFirstProfile();

    /**
     * @brief Returns true if any DelayedAction across all profiles is currently running
     */
    bool hasActiveDelayedAction() const;

    /**
     * @brief Drive timed LED behaviour — call every loop iteration
     *
     * @param now Current time in milliseconds (millis())
     */
    void update(uint32_t now);

    static const char* getActionTypeString(Action::Type actionType);

private:
    void updateLEDs();

    std::array<std::unique_ptr<Profile>, NUM_PROFILES> profileSlots;
    uint8_t currentProfile = 0;

    ILEDController& led1;
    ILEDController& led2;
    ILEDController& led3;

    // Post-switch blink state
    bool postSwitchBlink = false;
    uint32_t blinkStartTime = 0;
    uint8_t blinkPhase = 0;                         // counts half-cycles (on/off)
    static constexpr uint8_t BLINK_COUNT = 3;       // full on/off cycles
    static constexpr uint32_t BLINK_INTERVAL = 150; // ms per half-cycle
};
