// Direct framework-bundled includes so PIO's LDF activates the libraries
// from src/. The LDF in chain mode follows only direct .cpp #include lines;
// without these, Bluefruit and nRFCrypto would only be referenced
// transitively via ble_keyboard_adapter.h and stay unactivated.
#include <Adafruit_nRFCrypto.h>
#include <bluefruit.h>

#include "ble_keyboard_adapter.h"

void BleKeyboardAdapter::begin()
{
    Bluefruit.begin();
    Bluefruit.setTxPower(4);
    // Best practice: keep <=14 chars — the BLE 4.x advertising PDU only
    // leaves ~14 bytes for the GAP Local Name once Flags / TX Power /
    // Service UUID AD entries are included; longer names are truncated
    // by the host stack in the pre-connect scan list.
    Bluefruit.setName("Strix-Pedal");

    hid.begin();

    // Start BLE advertising
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
    Bluefruit.Advertising.addService(hid);
    Bluefruit.ScanResponse.addName();
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);
    Bluefruit.Advertising.setFastTimeout(30);
    Bluefruit.Advertising.start(0);
}

bool BleKeyboardAdapter::isConnected() { return Bluefruit.connected(); }

void BleKeyboardAdapter::write(uint8_t key)
{
    hid.keyPress(key);
    hid.keyRelease();
}

void BleKeyboardAdapter::write(const MediaKeyReport key)
{
    // MediaKeyReport is a 2-byte array; map byte[0] to Adafruit consumer key
    hid.consumerKeyPress(key[0]);
    hid.consumerKeyRelease();
}

void BleKeyboardAdapter::print(const char* text) { hid.keySequence(text); }

BleKeyboardAdapter* createBleKeyboardAdapter() { return new BleKeyboardAdapter(); }
