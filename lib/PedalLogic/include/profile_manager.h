#pragma once
#include "i_led_controller.h"
#include "profile.h"
#include <array>
#include <memory>

/**
 * @class ProfileManager
 * @brief Manages multiple profiles of button configurations with LED feedback
 *
 * This class handles the three configurable button profiles, allowing different
 * Profile objects to be assigned to each profile slot. It also manages the
 * visual feedback through LED indicators to show the currently active profile.
 */
class ProfileManager
{
public:
    static constexpr uint8_t NUM_PROFILES = 3;   /**< Number of available button profiles */

    /**
     * @brief Constructs a ProfileManager with LED controllers
     *
     * @param led1 LED controller for profile 1 indicator
     * @param led2 LED controller for profile 2 indicator
     * @param led3 LED controller for profile 3 indicator
     */
    ProfileManager(ILEDController& led1, ILEDController& led2, ILEDController& led3);

    /**
     * @brief Adds a complete profile to a specific profile slot
     *
     * @param profileIndex Profile slot index (0-2)
     * @param profile Unique pointer to the Profile to be stored
     */
    void addProfile(uint8_t profileIndex, std::unique_ptr<Profile> profile);

    /**
     * @brief Gets the action associated with a profile and button
     *
     * @param profileIndex Profile slot index (0-2)
     * @param button Button index (0-3)
     * @return Pointer to the Action, or nullptr if none assigned
     */
    Action* getAction(uint8_t profileIndex, uint8_t button) const;

    /**
     * @brief Gets the profile at a specific index
     *
     * @param profileIndex Profile slot index (0-2)
     * @return Pointer to the Profile, or nullptr if none assigned
     */
    const Profile* getProfile(uint8_t profileIndex) const;

    /**
     * @brief Switches to the next profile and updates LED indicators
     *
     * @return Index of the new current profile (0-2)
     */
    uint8_t switchProfile();

    /**
     * @brief Gets the currently active profile index
     *
     * @return Current profile index (0-2)
     */
    uint8_t getCurrentProfile() const;

    /**
     * @brief Gets the name of a profile
     *
     * @param profileIndex Profile slot index (0-2)
     * @return The profile name, or empty string if no profile assigned
     */
    const std::string& getProfileName(uint8_t profileIndex) const;

    /**
     * @brief Gets a string representation of an action type
     *
     * @param actionType The action type to convert to string
     * @return String representation of the action type
     */
    static const char* getActionTypeString(Action::Type actionType);

private:
    /**
     * @brief Updates LED indicators to reflect current profile
     *
     * Turns on the LED corresponding to the current profile and
     * turns off the LEDs for other profiles.
     */
    void updateLEDs();

    std::array<std::unique_ptr<Profile>, NUM_PROFILES>
        profileSlots;           /**< Array storing profiles for all profile slots */
    uint8_t currentProfile = 0; /**< Currently active profile index */
    ILEDController& led1;    /**< Reference to profile 1 LED controller */
    ILEDController& led2;    /**< Reference to profile 2 LED controller */
    ILEDController& led3;    /**< Reference to profile 3 LED controller */
};
