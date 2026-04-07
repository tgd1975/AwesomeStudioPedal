#pragma once
#include <memory>
#include <array>
#include "send.h"
#include "i_led_controller.h"

class BankManager {
public:
    BankManager(ILEDController& led1, ILEDController& led2, ILEDController& led3);

    void addAction(uint8_t bank, uint8_t button, std::unique_ptr<Send> action);
    Send* getAction(uint8_t bank, uint8_t button);
    void switchBank();
    void updateLEDs();
    uint8_t getCurrentBank() const;

private:
    std::array<std::array<std::unique_ptr<Send>, 4>, 3> banks;
    uint8_t currentBank = 0;
    ILEDController& led1;
    ILEDController& led2;
    ILEDController& led3;
};
