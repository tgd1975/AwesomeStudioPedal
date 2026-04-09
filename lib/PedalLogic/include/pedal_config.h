#pragma once
#include "profile_manager.h"
#include "i_ble_keyboard.h"

/**
 * @brief Configures the button profiles with default actions
 *
 * Sets up the three button profiles with predefined actions for the guitar pedal.
 * This function creates Profile objects and adds them to the ProfileManager.
 *
 * @param profileManager Reference to the ProfileManager instance
 * @param keyboard Pointer to the BLE keyboard interface
 */
void configureProfiles(ProfileManager& profileManager, IBleKeyboard* keyboard);
