#pragma once
#include <memory>
#include <array>
#include "send.h"
#include "hardware/led_controller.h"

class BankManager {
public:
    BankManager(LEDController& led1, LEDController& led2, LEDController& led3);
    
    void addAction(uint8_t bank, uint8_t button, std::unique_ptr<Send> action);
    Send* getAction(uint8_t bank, uint8_t button);
    void switchBank();
    void updateLEDs();
    uint8_t getCurrentBank() const;
    
private:
    std::array<std::array<std::unique_ptr<Send>, 4>, 3> banks;
    uint8_t currentBank = 0;
    LEDController& led1;
    LEDController& led2;
    LEDController& led3;
};EOF