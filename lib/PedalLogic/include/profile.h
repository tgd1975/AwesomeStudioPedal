#pragma once
#include "action.h"
#include <array>
#include <memory>
#include <string>

/**
 * @class Profile
 * @brief Represents a single button configuration profile
 *
 * A Profile contains a name and an array of actions, one for each button.
 * This provides better encapsulation than the previous 2D array approach.
 */
class Profile
{
public:
    static constexpr uint8_t MAX_BUTTONS = 26; /**< Maximum buttons per profile (A–Z) */

    /**
     * @brief Constructs a Profile with a name
     *
     * @param name The name of this profile
     */
    explicit Profile(std::string name);

    /**
     * @brief Adds an action to a specific button in this profile
     *
     * @param button Button index (0..MAX_BUTTONS-1)
     * @param action Unique pointer to the action to be executed
     */
    void addAction(uint8_t button, std::unique_ptr<Action> action);

    /**
     * @brief Gets the action associated with a button in this profile
     *
     * @param button Button index (0..MAX_BUTTONS-1)
     * @return Pointer to the Action, or nullptr if none assigned
     */
    Action* getAction(uint8_t button) const;

    /**
     * @brief Gets the name of this profile
     *
     * @return The profile name
     */
    const std::string& getName() const;

    /**
     * @brief Gets the description of this profile
     *
     * @return The profile description
     */
    const std::string& getDescription() const;

    /**
     * @brief Sets the description of this profile
     *
     * @param description The profile description
     */
    void setDescription(const std::string& description);

private:
    std::string name;        /**< The name of this profile */
    std::string description; /**< The description of this profile */
    std::array<std::unique_ptr<Action>, MAX_BUTTONS> actions; /**< Actions for each button */
};