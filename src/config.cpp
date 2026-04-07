#include "config.h"

const HardwareConfig hardwareConfig = {
    // LED pins
    .ledBluetooth = GPIO_NUM_26,
    .ledPower = GPIO_NUM_25,
    .ledSelect1 = GPIO_NUM_5,
    .ledSelect2 = GPIO_NUM_18,
    .ledSelect3 = GPIO_NUM_19,
    
    // Button pins
    .buttonSelect = GPIO_NUM_21,
    .buttonA = GPIO_NUM_13,
    .buttonB = GPIO_NUM_12,
    .buttonC = GPIO_NUM_27,
    .buttonD = GPIO_NUM_14
};EOF