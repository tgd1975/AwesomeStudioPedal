#pragma once

#include "ble_pedal_app.h"

/**
 * @brief nRF52840-specific pedal application.
 *
 * Stub implementation: no NVS-equivalent persistence on this target,
 * and no on-device config service. Both `platformSetup` and
 * `platformLoop` are no-ops; `saveProfile` discards the index and
 * `loadProfile` always returns 0 (matches pre-refactor behaviour).
 */
class Nrf52840PedalApp : public BlePedalApp
{
public:
    Nrf52840PedalApp();

protected:
    void platformSetup() override {}
    void platformLoop() override {}
    void saveProfile(uint8_t /*index*/) override {}
    uint8_t loadProfile() override { return 0; }
};
