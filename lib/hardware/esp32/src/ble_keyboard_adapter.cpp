#include "ble_keyboard_adapter.h"
#include <BleKeyboard.h>

static BleKeyboard bleKeyboard("Strix-Pedal", "Strix");

BleKeyboardAdapter* createBleKeyboardAdapter() {
    return new BleKeyboardAdapter(bleKeyboard);
}
