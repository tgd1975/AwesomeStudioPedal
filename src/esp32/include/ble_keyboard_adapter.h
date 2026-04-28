#pragma once
// clang-format off
// BleKeyboard.h must precede i_ble_keyboard.h: defines ESP32_BLE_KEYBOARD_H guard
// that prevents duplicate key constant definitions in i_ble_keyboard.h.
#include <BleKeyboard.h>
#include "i_ble_keyboard.h"
// clang-format on

#include "config.h"
#include <NimBLEDevice.h>
#include <functional>

using OnStartedCb = std::function<void(BLEServer*)>;

/**
 * @brief BleKeyboard subclass that exposes an onStarted hook.
 *
 * onStarted fires inside BleKeyboard::begin(), after hid->startServices() but
 * before adv->start(). This is the only safe window to register additional
 * GATT services when USE_NIMBLE is defined (ble_gatts_start() locks the table).
 *
 * We also relax the security posture here: the BleKeyboard library defaults to
 * bonding + MITM + SC, but our hardware has no display or keypad
 * (IOCap = NoInputNoOutput). MITM=true + NoInputNoOutput is an unpairable
 * combination on strict stacks like BlueZ. Consumer no-display BT keyboards
 * (Logitech MX Keys, Apple Magic Keyboard, etc.) ship with MITM=false; we
 * match that posture so Linux and Windows can pair us in addition to Android
 * and iOS. See TASK-229 and BLE_CONFIG_IMPLEMENTATION_NOTES.md.
 */
class HookableBleKeyboard : public BleKeyboard
{
public:
    HookableBleKeyboard(const char* name, const char* manufacturer)
        : BleKeyboard(name, manufacturer)
    {
    }

    void setOnStartedCallback(OnStartedCb cb) { cb_ = cb; }

protected:
    void onStarted(BLEServer* pServer) override
    {
        // Two mutually exclusive pairing postures, picked by hardwareConfig:
        //
        // pairingEnabled == false  →  Just Works (no PIN)
        //   - MITM=false / IOCap=NoInputNoOutput
        //   - Matches consumer no-display BT keyboards (Logitech MX Keys,
        //     Apple Magic Keyboard, etc). MITM=true + NoInputNoOutput is an
        //     unpairable combination on strict stacks like BlueZ; this posture
        //     lets Linux/Windows pair us in addition to Android/iOS. See
        //     TASK-229 and BLE_CONFIG_IMPLEMENTATION_NOTES.md.
        //
        // pairingEnabled == true   →  Passkey Entry (initiator types our PIN)
        //   - MITM=true / IOCap=DisplayOnly + setSecurityPasskey(...)
        //   - We "display" the passkey (conceptually — the value lives in
        //     hardwareConfig.json); the connecting host enters it. MITM=true
        //     is what tells the SMP IOCap matrix to pick Passkey Entry instead
        //     of collapsing to Just Works. Setting only IOCap without MITM was
        //     the original TASK-237 defect: passkey configured but never
        //     checked, anyone in range could bond. See TASK-246.
        if (hardwareConfig.pairingEnabled)
        {
            NimBLEDevice::setSecurityAuth(/*bonding=*/true, /*mitm=*/true, /*sc=*/true);
            NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
            NimBLEDevice::setSecurityPasskey(hardwareConfig.pairingPin);
        }
        else
        {
            NimBLEDevice::setSecurityAuth(/*bonding=*/true, /*mitm=*/false, /*sc=*/true);
            NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
        }

        if (cb_)
            cb_(pServer);
    }

private:
    OnStartedCb cb_;
};

/**
 * @class BleKeyboardAdapter
 * @brief Implements IBleKeyboard by wrapping HookableBleKeyboard.
 */
class BleKeyboardAdapter : public IBleKeyboard
{
    HookableBleKeyboard& kb_;

public:
    explicit BleKeyboardAdapter(HookableBleKeyboard& kb) : kb_(kb) {}

    void setOnStartedCallback(OnStartedCb cb) { kb_.setOnStartedCallback(cb); }

    void begin() override { kb_.begin(); }
    bool isConnected() override { return kb_.isConnected(); }
    void write(uint8_t key) override { kb_.write(key); }
    void write(const MediaKeyReport key) override { kb_.write(key); }
    void print(const char* text) override { kb_.print(text); }
};

BleKeyboardAdapter* createBleKeyboardAdapter();
