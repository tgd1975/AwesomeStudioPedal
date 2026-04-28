#pragma once

#include <cstdint>

#include "ble_pedal_app.h"

class IBleKeyboard;

/**
 * @brief Host-side specialization of BlePedalApp for unit tests.
 *
 * Inherits BlePedalApp so tests can exercise the full setup() / loop()
 * flow on host. The IBleKeyboard is constructor-injected — pass a
 * MockBleKeyboard to assert on key writes.
 *
 * Persistence is in-memory: saveProfile stores into a private byte;
 * loadProfile returns it. Sufficient for round-trip testing without
 * dragging in IFileSystem.
 *
 * platformSetup / platformLoop are no-ops — there is no BleConfigService
 * equivalent to run on host.
 */
class HostPedalApp : public BlePedalApp
{
public:
    explicit HostPedalApp(IBleKeyboard* bleKeyboard) : BlePedalApp(bleKeyboard) {}

protected:
    void platformSetup() override {}
    void platformLoop() override {}
    void saveProfile(uint8_t index) override { storedProfile_ = index; }
    uint8_t loadProfile() override { return storedProfile_; }

private:
    uint8_t storedProfile_ = 0;
};
