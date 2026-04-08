#include "bank_manager.h"

BankManager::BankManager(ILEDController& led1, ILEDController& led2, ILEDController& led3)
    : led1(led1), led2(led2), led3(led3) {}

void BankManager::addAction(uint8_t bank, uint8_t button, std::unique_ptr<Send> action) {
    if (bank < 3 && button < 4) {
        banks[bank][button] = std::move(action);
    }
}

Send* BankManager::getAction(uint8_t bank, uint8_t button) {
    if (bank < 3 && button < 4 && banks[bank][button]) {
        return banks[bank][button].get();
    }
    return nullptr;
}

uint8_t BankManager::switchBank() {
    currentBank = (currentBank + 1) % 3;
    updateLEDs();
    return currentBank;
}

void BankManager::updateLEDs() {
    led1.setState(currentBank == 0);
    led2.setState(currentBank == 1);
    led3.setState(currentBank == 2);
}

uint8_t BankManager::getCurrentBank() const {
    return currentBank;
}
