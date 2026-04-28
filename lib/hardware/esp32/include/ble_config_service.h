#pragma once
#include "i_ble_keyboard.h"
#include "i_led_controller.h"
#include "profile_manager.h"
#include <NimBLEServer.h>
#include <cstdint>
#include <functional>
#include <vector>

/**
 * @class BleConfigService
 * @brief ESP32 GATT wrapper that feeds BLE write packets into BleConfigReassembler.
 *
 * UUIDs and protocol defined in docs/developers/BLE_CONFIG_PROTOCOL.md.
 * Call begin() once during setup and loop() every main-loop iteration.
 */
class BleConfigService
{
public:
    BleConfigService() = default;

    // Registers GATT service via BleKeyboardAdapter's onStarted hook.
    // Must be called before adapter->begin().
    void begin(ProfileManager* profileManager,
               IBleKeyboard* keyboard,
               std::vector<ILEDController*> selectLeds);

    void loop();

    bool isApplying() const;
};
