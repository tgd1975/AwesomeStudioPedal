#pragma once

#include "ble_config_service.h"
#include "ble_pedal_app.h"

/**
 * @brief ESP32-specific pedal application.
 *
 * Adds NVS-backed profile persistence (via Preferences) and the
 * BleConfigService that exposes config read/write over GATT.
 */
class Esp32PedalApp : public BlePedalApp
{
public:
    Esp32PedalApp();

protected:
    void platformSetup() override;
    void platformLoop() override;
    void saveProfile(uint8_t index) override;
    uint8_t loadProfile() override;

private:
    BleConfigService bleConfigService_;
};
