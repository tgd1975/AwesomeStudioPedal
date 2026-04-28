// ble_keyboard_adapter.h must come first so its <BleKeyboard.h> include sets the
// ESP32_BLE_KEYBOARD_H guard before anything pulls in i_ble_keyboard.h (which
// would otherwise redefine BleKeyboard's media-key constants).
#include "ble_keyboard_adapter.h"

#include "esp32_pedal_app.h"

#include <Preferences.h>

namespace
{
    constexpr const char* NVS_NAMESPACE = "pedal";
    constexpr const char* NVS_KEY_PROFILE = "profile";
}

Esp32PedalApp::Esp32PedalApp() : BlePedalApp(createBleKeyboardAdapter()) {}

void Esp32PedalApp::platformSetup()
{
    bleConfigService_.begin(profileManager(), bleKeyboard(), selectLeds());
}

void Esp32PedalApp::platformLoop() { bleConfigService_.loop(); }

void Esp32PedalApp::saveProfile(uint8_t index)
{
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, false);
    prefs.putUChar(NVS_KEY_PROFILE, index);
    prefs.end();
}

uint8_t Esp32PedalApp::loadProfile()
{
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, true);
    uint8_t index = prefs.getUChar(NVS_KEY_PROFILE, 0);
    prefs.end();
    return index;
}
