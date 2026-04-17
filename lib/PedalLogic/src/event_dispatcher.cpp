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

void EventDispatcher::registerReleaseHandler(uint8_t button, EventCallback callback)
{
    if (button < releaseHandlers.size())
    {
        releaseHandlers[button] = std::move(callback);
    }
}

void EventDispatcher::dispatchRelease(uint8_t button)
{
    if (button < releaseHandlers.size() && releaseHandlers[button])
    {
        releaseHandlers[button]();
    }
}

/**
 * @brief Clears all registered event handlers (press and release)
 */
void EventDispatcher::clearHandlers()
{
    for (auto& handler : handlers)
    {
        handler = nullptr;
    }
    for (auto& handler : releaseHandlers)
    {
        handler = nullptr;
    }
}
