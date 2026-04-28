#pragma once
#include "action.h"
#include <cstdint>

/**
 * @class PinAction
 * @brief GPIO pin operations triggered by button press (and optionally release).
 *
 * Modes:
 *   PinHigh             — drive pin HIGH on press
 *   PinLow              — drive pin LOW on press
 *   PinToggle           — toggle pin on each press
 *   PinHighWhilePressed — HIGH on press, LOW on release
 *   PinLowWhilePressed  — LOW on press, HIGH on release
 *
 * For host test builds, digitalWrite() is shimmed via arduino_shim.h.
 */
class PinAction : public Action
{
public:
    explicit PinAction(Action::Type mode, uint8_t pin);

    void execute() override;
    void executeRelease() override;
    Action::Type getType() const override { return mode_; }
    void getJsonProperties(JsonObject& json) const override;

private:
    Action::Type mode_;
    uint8_t pin_;
    bool pinState_ = false; /**< Current state for PinToggle */
};
