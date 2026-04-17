#include "ble_keyboard_adapter.h"
#include <BleKeyboard.h>

static BleKeyboard bleKeyboard("AwesomeStudioPedal", "tgd1975");

BleKeyboardAdapter* createBleKeyboardAdapter() { return new BleKeyboardAdapter(bleKeyboard); }
