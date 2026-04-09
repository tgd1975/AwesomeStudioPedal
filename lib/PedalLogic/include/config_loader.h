#pragma once
#include "profile_manager.h"
#include "action.h"
#include "button_constants.h"
#include "i_ble_keyboard.h"
#include <string>
#ifndef HOST_TEST_BUILD
#include <ArduinoJson.h>
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
     * @brief Gets the default configuration as JSON string
     * 
     * @return JSON string containing default configuration
     */
    const char* getDefaultConfig() const { return DEFAULT_CONFIG; }
    
    /**
     * @brief Converts button name ("A", "B", "C", "D") to button index
     * 
     * @param buttonName Button name as string
     * @return Button index (0-3) or 255 if invalid
     */
    static uint8_t getButtonIndex(const char* buttonName);

private:
#ifndef HOST_TEST_BUILD
    std::unique_ptr<Action> createActionFromJson(const JsonObject& actionJson, IBleKeyboard* keyboard);
    JsonObject actionToJson(const Action* action) const;
#endif
};