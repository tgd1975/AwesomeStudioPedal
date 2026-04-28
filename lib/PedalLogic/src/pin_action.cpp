#include "pin_action.h"

#include <Arduino.h>

PinAction::PinAction(Action::Type mode, uint8_t pin) : mode_(mode), pin_(pin) {}

void PinAction::execute()
{
    switch (mode_)
    {
        case Action::Type::PinHigh:
            digitalWrite(pin_, HIGH);
            break;
        case Action::Type::PinLow:
            digitalWrite(pin_, LOW);
            break;
        case Action::Type::PinToggle:
            pinState_ = ! pinState_;
            digitalWrite(pin_, pinState_ ? HIGH : LOW);
            break;
        case Action::Type::PinHighWhilePressed:
            digitalWrite(pin_, HIGH);
            break;
        case Action::Type::PinLowWhilePressed:
            digitalWrite(pin_, LOW);
            break;
        default:
            break;
    }
}

void PinAction::getJsonProperties(JsonObject& json) const { json["pin"] = pin_; }

void PinAction::executeRelease()
{
    switch (mode_)
    {
        case Action::Type::PinHighWhilePressed:
            digitalWrite(pin_, LOW);
            break;
        case Action::Type::PinLowWhilePressed:
            digitalWrite(pin_, HIGH);
            break;
        default:
            break;
    }
}
