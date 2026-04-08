#pragma once
#include <memory>
#include <array>
#include "send.h"
#include "i_led_controller.h"

/**
 * @class BankManager
 * @brief Manages multiple banks of button configurations with LED feedback
 * 
 * This class handles the three configurable button banks, allowing different
 * actions to be assigned to each button in each bank. It also manages the
 * visual feedback through LED indicators to show the currently active bank.
 */
class BankManager {
public:
    static constexpr uint8_t NUM_BANKS   = 3; /**< Number of available button banks */
    static constexpr uint8_t NUM_BUTTONS = 4; /**< Number of buttons per bank */

    /**
     * @brief Constructs a BankManager with LED controllers
     * 
     * @param led1 LED controller for bank 1 indicator
     * @param led2 LED controller for bank 2 indicator  
     * @param led3 LED controller for bank 3 indicator
     */
    BankManager(ILEDController& led1, ILEDController& led2, ILEDController& led3);

    /**
     * @brief Adds an action to a specific bank and button
     * 
     * @param bank Bank index (0-2)
     * @param button Button index (0-3)
     * @param action Unique pointer to the action to be executed
     */
    void addAction(uint8_t bank, uint8_t button, std::unique_ptr<Send> action);
    
    /**
     * @brief Gets the action associated with a bank and button
     * 
     * @param bank Bank index (0-2)
     * @param button Button index (0-3)
     * @return Pointer to the Send action, or nullptr if none assigned
     */
    Send* getAction(uint8_t bank, uint8_t button);
    
    /**
     * @brief Switches to the next bank and updates LED indicators
     * 
     * @return Index of the new current bank (0-2)
     */
    uint8_t switchBank();
    
    /**
     * @brief Gets the currently active bank index
     * 
     * @return Current bank index (0-2)
     */
    uint8_t getCurrentBank() const;

private:
    /**
     * @brief Updates LED indicators to reflect current bank
     * 
     * Turns on the LED corresponding to the current bank and
     * turns off the LEDs for other banks.
     */
    void updateLEDs();
    
    std::array<std::array<std::unique_ptr<Send>, NUM_BUTTONS>, NUM_BANKS> banks; /**< 2D array storing actions for all banks/buttons */
    uint8_t currentBank = 0; /**< Currently active bank index */
    ILEDController& led1; /**< Reference to bank 1 LED controller */
    ILEDController& led2; /**< Reference to bank 2 LED controller */
    ILEDController& led3; /**< Reference to bank 3 LED controller */
};
