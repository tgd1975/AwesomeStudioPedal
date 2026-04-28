#pragma once

#include <cstdint>
#include <vector>

#include "button_constants.h"
#include "profile_manager.h"

class Button;
class LEDController;
class EventDispatcher;
class ILEDController;

/**
 * @brief Abstract base for the pedal application.
 *
 * Holds the hardware-agnostic core: button / LED objects, profile manager,
 * event dispatcher, hardware-mismatch and load-error signaling, button
 * event handler registration. Knows nothing about BLE, interrupts, or the
 * main loop body — those live in `BlePedalApp`.
 *
 * Concrete subclasses go through `BlePedalApp` (for on-device targets) or
 * inherit `PedalApp` directly (for a future host/test variant).
 *
 * This header is deliberately Arduino-free: it only references project
 * interfaces and standard C++. Arduino APIs (`attachInterrupt`, `millis`,
 * `Serial`, …) live in the corresponding .cpp files and concrete
 * subclass implementations.
 */
class PedalApp
{
public:
    PedalApp() = default;
    virtual ~PedalApp() = default;

    PedalApp(const PedalApp&) = delete;
    PedalApp& operator=(const PedalApp&) = delete;

protected:
    // Platform hooks ------------------------------------------------------
    virtual void platformSetup() = 0;
    virtual void platformLoop() = 0;
    virtual void saveProfile(uint8_t index) = 0;
    virtual uint8_t loadProfile() = 0;

    // Shared setup / signaling --------------------------------------------
    void setupCommon(); // serial init, hw config, profile manager, events
    void signalHardwareMismatch();
    void signalLoadError();
    void executeActionWithLogging(uint8_t buttonIndex);

    // Accessors used by BlePedalApp / concrete subclasses ------------------
    ProfileManager* profileManager() { return profileManager_; }
    std::vector<ILEDController*>& selectLeds() { return selectLeds_; }
    Button** actionButtonObjects() { return actionButtonObjects_; }
    Button* buttonSelect() { return buttonSelect_; }
    LEDController* ledPower() { return ledPower_; }
    LEDController* ledBluetooth() { return ledBluetooth_; }
    EventDispatcher* eventDispatcher() { return eventDispatcher_; }

private:
    void setupHardware();
    void setupEventHandlers();

    ProfileManager* profileManager_ = nullptr;
    EventDispatcher* eventDispatcher_ = nullptr;

    LEDController* ledBluetooth_ = nullptr;
    LEDController* ledPower_ = nullptr;
    LEDController* selectLedObjects_[ProfileManager::MAX_SELECT_LEDS] = {};
    std::vector<ILEDController*> selectLeds_;

    Button* actionButtonObjects_[Btn::MAX] = {};
    Button* buttonSelect_ = nullptr;
};
