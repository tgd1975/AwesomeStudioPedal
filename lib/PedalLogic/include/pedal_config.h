#pragma once
#include "profile_manager.h"
#include "i_ble_keyboard.h"

/**
 * @brief Loads pedal profiles from file, falling back to DEFAULT_CONFIG on failure.
 *
 * @return true  if configuration was loaded successfully (from file or default JSON)
 * @return false if even the default JSON failed to parse (should never happen)
 */
bool configureProfiles(ProfileManager& profileManager, IBleKeyboard* keyboard);
