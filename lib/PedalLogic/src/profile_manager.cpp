#include "profile_manager.h"

/**
 * @brief Constructs a ProfileManager and initializes LED indicators
 *
 * @param led1 Reference to profile 1 LED controller
 * @param led2 Reference to profile 2 LED controller
 * @param led3 Reference to profile 3 LED controller
 */
ProfileManager::ProfileManager(ILEDController& led1, ILEDController& led2, ILEDController& led3)
    : led1(led1), led2(led2), led3(led3)
{
    updateLEDs();
}

/**
 * @brief Adds a complete profile to a specific profile slot
 *
 * Performs bounds checking before storing the profile.
 *
 * @param profileIndex Profile slot index (0-2)
 * @param profile Unique pointer to the Profile to be stored
 */
void ProfileManager::addProfile(uint8_t profileIndex, std::unique_ptr<Profile> profile)
{
    if (profileIndex < NUM_PROFILES)
    {
        profileSlots[profileIndex] = std::move(profile);
    }
}

/**
 * @brief Gets the action associated with a profile and button
 *
 * Performs bounds checking and delegates to the Profile object.
 *
 * @param profileIndex Profile slot index (0-2)
 * @param button Button index (0-3)
 * @return Pointer to the Action, or nullptr if invalid or not set
 */
Action* ProfileManager::getAction(uint8_t profileIndex, uint8_t button) const
{
    if (profileIndex < NUM_PROFILES && profileSlots[profileIndex])
    {
        return profileSlots[profileIndex]->getAction(button);
    }
    return nullptr;
}

/**
 * @brief Gets the profile at a specific index
 *
 * Performs bounds checking before returning the profile.
 *
 * @param profileIndex Profile slot index (0-2)
 * @return Pointer to the Profile, or nullptr if invalid or not set
 */
const Profile* ProfileManager::getProfile(uint8_t profileIndex) const
{
    if (profileIndex < NUM_PROFILES && profileSlots[profileIndex])
    {
        return profileSlots[profileIndex].get();
    }
    return nullptr;
}

/**
 * @brief Cycles to the next profile and updates LED indicators
 *
 * Uses modulo arithmetic to wrap around from profile 2 back to profile 0.
 *
 * @return The new current profile index (0-2)
 */
uint8_t ProfileManager::switchProfile()
{
    currentProfile = (currentProfile + 1) % NUM_PROFILES;
    updateLEDs();
    return currentProfile;
}

/**
 * @brief Updates LED states to reflect the current profile
 *
 * Turns on the LED corresponding to the current profile and
 * turns off LEDs for other profiles.
 */
void ProfileManager::updateLEDs()
{
    led1.setState(currentProfile == 0);
    led2.setState(currentProfile == 1);
    led3.setState(currentProfile == 2);
}

/**
 * @brief Gets the currently active profile index
 *
 * @return Current profile index (0-2)
 */
uint8_t ProfileManager::getCurrentProfile() const { return currentProfile; }

/**
 * @brief Gets the name of a profile
 *
 * @param profileIndex Profile slot index (0-2)
 * @return The profile name, or empty string if no profile assigned
 */
const std::string& ProfileManager::getProfileName(uint8_t profileIndex) const
{
    if (profileIndex < NUM_PROFILES && profileSlots[profileIndex])
    {
        return profileSlots[profileIndex]->getName();
    }
    static const std::string emptyString;
    return emptyString;
}

const char* ProfileManager::getActionTypeString(Action::Type actionType)
{
    switch (actionType)
    {
        case Action::Type::SendString: return "SendString";
        case Action::Type::SendChar: return "SendChar";
        case Action::Type::SendKey: return "SendKey";
        case Action::Type::SendMediaKey: return "SendMediaKey";
        case Action::Type::SerialOutput: return "SerialOutput";
        case Action::Type::Delayed: return "Delayed";
        default: return "Unknown";
    }
}