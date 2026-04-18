/**
 * BLE Config Integration Test Harness (ESP32 / NodeMCU-32S)
 *
 * This firmware starts the BLE Config service on real hardware and acts as the
 * System Under Test (SUT) for the Python runner (runner.py). It does NOT use
 * the Unity test framework — assertions live entirely in runner.py, which
 * connects as a BLE client and verifies protocol behaviour.
 *
 * Serial output format (runner.py parses these):
 *   [BLE_TEST] READY              — advertising started, runner may connect
 *   [BLE_TEST] PROFILE:<name>     — active profile changed (after successful upload)
 *   [BLE_TEST] RESET              — ESP32 is about to soft-reset (persistence test)
 *
 * Run via: make test-esp32-ble-config
 */

// clang-format off
#include <BleKeyboard.h>
// clang-format on

#include <Arduino.h>
#include <vector>

#include "ble_config_service.h"
#include "ble_keyboard_adapter.h"
#include "config.h"
#include "i_led_controller.h"
#include "led_controller.h"
#include "null_led_controller.h"
#include "pedal_config.h"
#include "profile_manager.h"

// ---------------------------------------------------------------------------
// Hardware peripherals
// ---------------------------------------------------------------------------

static BleKeyboardAdapter* bleAdapter = nullptr;

static LEDController* selectLedObjects[ProfileManager::MAX_SELECT_LEDS] = {};
static std::vector<ILEDController*> selectLeds;

static ProfileManager* pm = nullptr;
static BleConfigService bleConfigService;

// ---------------------------------------------------------------------------
// setup / loop
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // Override hardwareConfig from LittleFS if /config.json exists
    loadHardwareConfig();

    // Build select-LED vector
    for (uint8_t i = 0; i < hardwareConfig.numSelectLeds; i++)
    {
        selectLedObjects[i] = new LEDController(hardwareConfig.ledSelect[i]);
        selectLedObjects[i]->setup(false);
        selectLeds.push_back(selectLedObjects[i]);
    }

    // Profile manager
    pm = new ProfileManager(selectLeds);

    // BLE keyboard (provides the NimBLE server via ESP32 BLE Keyboard library)
    bleAdapter = createBleKeyboardAdapter();
    bleAdapter->begin();

    // BLE Config GATT service
    bleConfigService.begin(pm, bleAdapter, selectLeds);

    Serial.println("[BLE_TEST] READY");
}

void loop()
{
    bleConfigService.loop();
    pm->update(millis());

    // Emit serial line when active profile changes
    static uint8_t lastProfile = 0xFF;
    uint8_t cur = pm->getCurrentProfile();
    if (cur != lastProfile)
    {
        lastProfile = cur;
        const Profile* p = pm->getProfile(cur);
        if (p)
            Serial.printf("[BLE_TEST] PROFILE:%s\n", p->getName().c_str());
    }

    // Handle serial commands from runner.py
    if (Serial.available())
    {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "RESET")
        {
            Serial.println("[BLE_TEST] RESET");
            Serial.flush();
            delay(100);
            ESP.restart();
        }
        else if (cmd == "PROFILE?")
        {
            const Profile* p = pm->getProfile(pm->getCurrentProfile());
            if (p)
                Serial.printf("[BLE_TEST] PROFILE:%s\n", p->getName().c_str());
            else
                Serial.println("[BLE_TEST] PROFILE:(none)");
        }
    }
}
