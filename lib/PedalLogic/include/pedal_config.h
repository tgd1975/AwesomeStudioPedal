#pragma once
#include "bank_manager.h"
#include "i_ble_keyboard.h"

/**
 * @brief Configures the button banks with default actions
 * 
 * Sets up the three button banks with predefined actions for the guitar pedal.
 * This function defines what each button does in each bank.
 * 
 * @param bankManager Reference to the BankManager instance
 * @param keyboard Pointer to the BLE keyboard interface
 */
void configureBanks(BankManager& bankManager, IBleKeyboard* keyboard);
