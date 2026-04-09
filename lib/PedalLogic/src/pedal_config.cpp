#include "pedal_config.h"
#include "action.h"
#include "button_constants.h"
#include "config_loader.h"
#include "profile.h"
#include "send_action.h"
#include "serial_action.h"
#include <memory>

/**
 * @brief Load profiles from file; fall back to DEFAULT_CONFIG on failure.
 *
 * Returns false only if DEFAULT_CONFIG itself fails to parse — which indicates
 * a programming error and should never happen at runtime.
 */
bool configureProfiles(ProfileManager& profileManager, IBleKeyboard* keyboard)
{
    ConfigLoader configLoader;

    if (configLoader.loadFromFile(profileManager, keyboard, "/pedal_config.json"))
    {
        return true;
    }

    // File missing or invalid — load the hardcoded default
    if (! configLoader.loadFromString(profileManager, keyboard, configLoader.getDefaultConfig()))
    {
        // DEFAULT_CONFIG failed to parse: this is a compile-time bug, not a runtime error
        return false;
    }

    // Persist the default so future boots load from file
    configLoader.saveToFile(profileManager, "/pedal_config.json");
    return true;
}
