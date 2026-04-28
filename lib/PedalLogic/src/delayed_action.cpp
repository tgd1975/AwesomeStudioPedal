#include "delayed_action.h"

#include <Arduino.h>

/**
 * @brief Constructs a DelayedAction that wraps an action with a time delay
 *
 * @param action The action to be executed after the delay
 * @param delayMs The delay duration in milliseconds
 */
DelayedAction::DelayedAction(std::unique_ptr<Action> action, uint32_t delayMs)
    : action(std::move(action)), delayMs(delayMs)
{
}

/**
 * @brief Executes the delayed action
 *
 * If not already started, begins the delay timer. If the delay has elapsed,
 * executes the inner action and resets the timer. Calls before the delay elapses
 * are ignored (debounce behavior).
 */
void DelayedAction::execute()
{
    if (! started)
    {
        startTime = static_cast<uint32_t>(millis());
        started = true;
    }
    else
    {
        if (millis() - startTime >= delayMs)
        {
            action->execute();
            started = false;
        }
    }
}

/**
 * @brief Updates the delay timer and checks if ready to execute
 */
bool DelayedAction::update(uint32_t currentTime) const
{
    if (! started)
    {
        return false;
    }
    return (currentTime - startTime >= delayMs);
}

/**
 * @brief Serializes the delayed action to JSON
 *
 * @param json The JSON object to populate with action properties
 */
void DelayedAction::getJsonProperties(JsonObject& json) const
{
    json["delayMs"] = delayMs;
    JsonObject nestedAction = json.createNestedObject("action");
    action->getJsonProperties(nestedAction);
    nestedAction["type"] = getTypeName(action->getType());
}

/**
 * @brief Converts an Action::Type enum to its string representation
 *
 * @param type The action type enum value
 * @return String representation of the action type
 */
const char* DelayedAction::getTypeName(Action::Type type)
{
    switch (type)
    {
        case Action::Type::SendString:
            return "SendStringAction";
        case Action::Type::SendChar:
            return "SendCharAction";
        case Action::Type::SendKey:
            return "SendKeyAction";
        case Action::Type::SendMediaKey:
            return "SendMediaKeyAction";
        case Action::Type::SerialOutput:
            return "SerialOutputAction";
        case Action::Type::Delayed:
            return "DelayedAction";
        case Action::Type::PinHigh:
            return "PinHighAction";
        case Action::Type::PinLow:
            return "PinLowAction";
        case Action::Type::PinToggle:
            return "PinToggleAction";
        case Action::Type::PinHighWhilePressed:
            return "PinHighWhilePressedAction";
        case Action::Type::PinLowWhilePressed:
            return "PinLowWhilePressedAction";
        case Action::Type::Macro:
            return "MacroAction";
        default:
            return "UnknownAction";
    }
}
