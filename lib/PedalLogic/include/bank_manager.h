#pragma once
#include <memory>
#include <array>
#include "send.h"
#include "i_led_controller.h"

class BankManager {
public:
    static constexpr uint8_t NUM_BANKS   = 3;
    static constexpr uint8_t NUM_BUTTONS = 4;

    BankManager(ILEDController& led1, ILEDController& led2, ILEDController& led3);

    void addAction(uint8_t bank, uint8_t button, std::unique_ptr<Send> action);
    Send* getAction(uint8_t bank, uint8_t button);
    uint8_t switchBank();
    uint8_t getCurrentBank() const;

private:
    void updateLEDs();
    std::array<std::array<std::unique_ptr<Send>, NUM_BUTTONS>, NUM_BANKS> banks;
    uint8_t currentBank = 0;
    ILEDController& led1;
    ILEDController& led2;
    ILEDController& led3;
};
