#include "ble_keyboard_adapter.h"

#include "nrf52840_pedal_app.h"

Nrf52840PedalApp::Nrf52840PedalApp() : BlePedalApp(createBleKeyboardAdapter()) {}
