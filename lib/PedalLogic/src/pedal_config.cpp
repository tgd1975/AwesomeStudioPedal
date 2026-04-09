#include "pedal_config.h"
#include "profile.h"
#include "send_action.h"
#include "action.h"
#include "serial_action.h"
#include "button_constants.h"
#include "config_loader.h"
#include <memory>

/**
 * @brief Configures the pedal profiles by loading from configuration file
 *
 * Attempts to load configuration from "pedal_config.json" file.
 * If the file doesn't exist or is invalid, loads default configuration.
 *
 * @param profileManager Reference to the ProfileManager instance
 * @param keyboard Pointer to the BLE keyboard interface
 */
void configureProfiles(ProfileManager& profileManager, IBleKeyboard* keyboard)
{
    ConfigLoader configLoader;
    
    // Try to load from file first
    if (!configLoader.loadFromFile(profileManager, keyboard, "/pedal_config.json")) {
        // If file loading fails, load default configuration
        configLoader.loadFromString(profileManager, keyboard, configLoader.getDefaultConfig());
        
        // Save the default configuration to file for future use
        configLoader.saveToFile(profileManager, "/pedal_config.json");
    }
}