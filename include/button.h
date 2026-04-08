#pragma once
#include <Arduino.h>

class Button {
  private:
    uint8_t PIN;
    bool isDebounced();

  public:
    volatile bool pressed = false;
    unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
    unsigned long debounceDelay = 300;    // the debounce time; increase if the output flickers

    Button(uint8_t PIN);

    void isr();
    bool event();
};
