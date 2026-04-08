#include "bank_manager.h"

/**
 * @brief Constructs a BankManager and initializes LED indicators
 * 
 * @param led1 Reference to bank 1 LED controller
 * @param led2 Reference to bank 2 LED controller
 * @param led3 Reference to bank 3 LED controller
 */
BankManager::BankManager(ILEDController& led1, ILEDController& led2, ILEDController& led3)
    : led1(led1), led2(led2), led3(led3) {
    updateLEDs();
}

/**
 * @brief Adds an action to the specified bank and button
 * 
 * Performs bounds checking before storing the action.
 * 
 * @param bank Bank index (0-2)
 * @param button Button index (0-3)
 * @param action Unique pointer to the action to store
 */
void BankManager::addAction(uint8_t bank, uint8_t button, std::unique_ptr<Send> action) {
    if (bank < NUM_BANKS && button < NUM_BUTTONS) {
        banks[bank][button] = std::move(action);
    }
}

/**
 * @brief Retrieves the action for the specified bank and button
 * 
 * Performs bounds checking and null checking before returning.
 * 
 * @param bank Bank index (0-2)
 * @param button Button index (0-3)
 * @return Pointer to the Send action, or nullptr if invalid or not set
 */
Send* BankManager::getAction(uint8_t bank, uint8_t button) {
    if (bank < NUM_BANKS && button < NUM_BUTTONS && banks[bank][button]) {
        return banks[bank][button].get();
    }
    return nullptr;
}

/**
 * @brief Cycles to the next bank and updates LED indicators
 * 
 * Uses modulo arithmetic to wrap around from bank 2 back to bank 0.
 * 
 * @return The new current bank index (0-2)
 */
uint8_t BankManager::switchBank() {
    currentBank = (currentBank + 1) % NUM_BANKS;
    updateLEDs();
    return currentBank;
}

/**
 * @brief Updates LED states to reflect the current bank
 * 
 * Turns on the LED corresponding to the current bank and
 * turns off LEDs for other banks.
 */
void BankManager::updateLEDs() {
    led1.setState(currentBank == 0);
    led2.setState(currentBank == 1);
    led3.setState(currentBank == 2);
}

/**
 * @brief Gets the currently active bank index
 * 
 * @return Current bank index (0-2)
 */
uint8_t BankManager::getCurrentBank() const {
    return currentBank;
}
