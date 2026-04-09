#pragma once
#include "profile_manager.h"
#include "action.h"
#include "button_constants.h"
#include "i_ble_keyboard.h"
#include <string>
#ifndef HOST_TEST_BUILD
#include <ArduinoJson.h>
#else
#include <ArduinoJson.h>
// String class is provided by arduino_shim.h for host testing
#endif

/**
 * @class ConfigLoader
 * @brief Loads pedal configuration from JSON files
 * 
 * Responsible for parsing JSON configuration files and creating the appropriate
 * Action objects to populate the ProfileManager.
 */
class ConfigLoader
{
private:
    static const char* DEFAULT_CONFIG;

public:
    /**
     * @brief Loads configuration from a JSON file
     * 
     * @param profileManager Reference to ProfileManager to populate
     * @param keyboard Pointer to BLE keyboard interface for send actions
     * @param configPath Path to the JSON configuration file
     * @return true if loading succeeded, false otherwise
     */
    bool loadFromFile(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& configPath);
    
    /**
     * @brief Loads configuration from a JSON string
     * 
     * @param profileManager Reference to ProfileManager to populate
     * @param keyboard Pointer to BLE keyboard interface for send actions
     * @param jsonConfig JSON configuration string
     * @return true if loading succeeded, false otherwise
     */
    bool loadFromString(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig);
    
    /**
     * @brief Saves current configuration to a JSON file
     * 
     * @param profileManager Reference to ProfileManager containing current configuration
     * @param configPath Path to save the JSON configuration file
     * @return true if saving succeeded, false otherwise
     */
    bool saveToFile(const ProfileManager& profileManager, const std::string& configPath);

    /**
     * @brief Merges profiles from a JSON config into the current configuration
     * 
     * Adds profiles from the provided JSON config to empty slots in the current configuration.
     * Existing profiles are preserved unless they have the same name as incoming profiles.
     * 
     * @param profileManager Reference to ProfileManager to update
     * @param keyboard Pointer to BLE keyboard interface for send actions
     * @param jsonConfig JSON configuration string containing profiles to merge
     * @return true if merging succeeded, false otherwise
     */
    bool mergeConfig(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig);

    /**
     * @brief Replaces a specific profile in the current configuration
     * 
     * Replaces the profile at the specified index with a new profile from JSON config.
     * 
     * @param profileManager Reference to ProfileManager to update
     * @param keyboard Pointer to BLE keyboard interface for send actions
     * @param profileIndex Index of profile to replace (0-2)
     * @param jsonConfig JSON configuration string containing the replacement profile
     * @return true if replacement succeeded, false otherwise
     */
    bool replaceProfile(ProfileManager& profileManager, IBleKeyboard* keyboard, uint8_t profileIndex, const std::string& jsonConfig);
    
    /**
     * @brief Gets the default configuration as JSON string
     * 
     * @return JSON string containing default configuration
     */
    const char* getDefaultConfig() const { return DEFAULT_CONFIG; }

    /**
     * @brief Output debug messages (platform-independent)
     * 
     * @param message Message to output
     */
    static void debugOutput(const char* message);
    
    /**
     * @brief Output debug messages with prefix (platform-independent)
     * 
     * @param prefix Prefix for the message
     * @param message Message to output
     */
    static void debugOutput(const char* prefix, const char* message);

    /**
     * @brief Converts button name ("A", "B", "C", "D") to button index
     * 
     * @param buttonName Button name as string
     * @return Button index (0-3) or 255 if invalid
     */
    static uint8_t getButtonIndex(const char* buttonName);

private:
    std::unique_ptr<Action> createActionFromJson(const ArduinoJson::JsonObject& actionJson, IBleKeyboard* keyboard);
    ArduinoJson::JsonObject actionToJson(const Action* action) const;
};