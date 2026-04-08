#pragma once
#include <Arduino.h>

class Button {
  private:
    bool isDebounced();
    void incKeyPresses();

  private:
    uint8_t PIN;

  public:
    uint32_t numberKeyPresses = 0;
    volatile bool pressed = false;
    unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
    unsigned long debounceDelay = 300;    // the debounce time; increase if the output flickers

    Button(uint8_t PIN);

    void isr();
    bool event();
};
