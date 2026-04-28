#pragma once

#include <cstdint>

#include "button_constants.h"
#include "pedal_app.h"

class IBleKeyboard;

/**
 * @brief Shared BLE-pedal layer between PedalApp and concrete subclasses.
 *
 * Captures everything that is true for any BLE-based pedal regardless of
 * MCU: connection-state tracking, interrupt attach / detach on
 * connect / disconnect, the Bluetooth LED state, the per-button
 * long-press / double-press arming, and the main `setup()` / `loop()`
 * skeleton.
 *
 * Concrete subclasses (`Esp32PedalApp`, `Nrf52840PedalApp`) inherit this
 * and only override the four platform hooks declared on `PedalApp`.
 */
class BlePedalApp : public PedalApp
{
public:
    explicit BlePedalApp(IBleKeyboard* bleKeyboard);
    ~BlePedalApp() override;

    void setup();
    void loop();

    // ISR fan-out — invoked by the free ISR stubs in ble_pedal_app.cpp.
    void onActionButtonInterrupt(uint8_t index);
    void onSelectButtonInterrupt();

protected:
    IBleKeyboard* bleKeyboard() { return bleKeyboard_; }

private:
    void attachInterrupts();
    void detachInterrupts();
    void processEvents();

    IBleKeyboard* bleKeyboard_ = nullptr;
    bool connected_ = false;
    bool longPressArmed_[Btn::MAX] = {};
};

/**
 * @brief Process-wide pointer used by the IRAM-resident ISR stubs to
 *        dispatch back into the BlePedalApp instance. Set by
 *        BlePedalApp's constructor; nulled by its destructor.
 */
extern BlePedalApp* g_blePedalApp;
