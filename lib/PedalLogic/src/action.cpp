#include "action.h"
#include <Arduino.h>
#include <ArduinoJson.h>

DelayedAction::DelayedAction(std::unique_ptr<Action> action, uint32_t delayMs)
    : action(std::move(action)), delayMs(delayMs), startTime(0), started(false)
{
}

void DelayedAction::execute()
{
    if (!started) {
        // Start the delay
        startTime = millis();
        started = true;
    } else {
        // Check if delay is complete
        if (millis() - startTime >= delayMs) {
            action->execute();
            started = false; // Reset for potential reuse
        }
        // If delay is not complete, do nothing
    }
}

bool DelayedAction::update(uint32_t currentTime)
{
    if (!started) {
        return false; // Delay hasn't been started yet
    }
    
    if (currentTime - startTime >= delayMs) {
        return true; // Delay is complete
    }
    
    return false; // Delay is still in progress
}

void DelayedAction::getJsonProperties(JsonObject& json) const {
    json["delayMs"] = delayMs;
    JsonObject nestedAction = json.createNestedObject("action");
    action->getJsonProperties(nestedAction);
    nestedAction["type"] = getTypeName(action->getType());
}

const char* DelayedAction::getTypeName(Action::Type type) {
    switch (type) {
        case Action::Type::SendString: return "SendStringAction";
        case Action::Type::SendChar: return "SendCharAction";
        case Action::Type::SendKey: return "SendKeyAction";
        case Action::Type::SendMediaKey: return "SendMediaKeyAction";
        case Action::Type::SerialOutput: return "SerialOutputAction";
        case Action::Type::Delayed: return "DelayedAction";
        default: return "UnknownAction";
    }
}