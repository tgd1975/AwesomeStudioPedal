#include "event_dispatcher.h"

/**
 * @brief Registers a callback function for a specific button
 *
 * Performs bounds checking to ensure valid button index.
 */
void EventDispatcher::registerHandler(uint8_t button, EventCallback callback)
{
    if (button < handlers.size())
    {
        handlers[button] = std::move(callback);
    }
}

/**
 * @brief Executes the callback associated with a button
 *
 * Performs bounds checking and null checking before execution.
 */
void EventDispatcher::dispatch(uint8_t button)
{
    if (button < handlers.size() && handlers[button])
    {
        handlers[button]();
    }
}

/**
 * @brief Clears all registered event handlers
 *
 * Sets all callbacks to nullptr, effectively disabling them.
 */
void EventDispatcher::clearHandlers()
{
    for (auto& handler : handlers)
    {
        handler = nullptr;
    }
}
