#include "delayed_action.h"

#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include "../test/fakes/arduino_shim.h"
#endif

DelayedAction::DelayedAction(std::unique_ptr<Action> action, uint32_t delayMs)
    : action(std::move(action)), delayMs(delayMs)
{
}

void DelayedAction::execute()
{
    if (!started) {
        startTime = millis();
        started = true;
    } else {
        if (millis() - startTime >= delayMs) {
            action->execute();
            started = false;
        }
    }
}

bool DelayedAction::update(uint32_t currentTime)
{
    if (!started) return false;
    return (currentTime - startTime >= delayMs);
}

void DelayedAction::getJsonProperties(JsonObject& json) const {
    json["delayMs"] = delayMs;
    JsonObject nestedAction = json.createNestedObject("action");
    action->getJsonProperties(nestedAction);
    nestedAction["type"] = getTypeName(action->getType());
}

const char* DelayedAction::getTypeName(Action::Type type) {
    switch (type) {
        case Action::Type::SendString:   return "SendStringAction";
        case Action::Type::SendChar:     return "SendCharAction";
        case Action::Type::SendKey:      return "SendKeyAction";
        case Action::Type::SendMediaKey: return "SendMediaKeyAction";
        case Action::Type::SerialOutput: return "SerialOutputAction";
        case Action::Type::Delayed:      return "DelayedAction";
        default:                         return "UnknownAction";
    }
}
