#pragma once
#include <array>
#include <cstdint>
#include <functional>

/**
 * @class EventDispatcher
 * @brief Manages event callbacks for button presses
 *
 * This class provides a simple event dispatching system that maps
 * button indices to callback functions. It supports 5 buttons:
 * - 0-3: Action buttons A-D
 * - 4: Bank selection button
 */
class EventDispatcher
{
public:
    /**
     * @brief Callback function type for button events
     *
     * Function signature for event handlers (no parameters, void return)
     */
    using EventCallback = std::function<void()>;

    /**
     * @brief Registers a callback function for a button
     *
     * @param button Button index (0-4)
     * @param callback Function to call when button is pressed
     */
    void registerHandler(uint8_t button, EventCallback callback);

    /**
     * @brief Executes the callback for the specified button
     *
     * @param button Button index (0-4) to dispatch
     */
    void dispatch(uint8_t button);

    /**
     * @brief Clears all registered event handlers
     *
     * Resets all callbacks to empty functions.
     */
    void clearHandlers();

private:
    std::array<EventCallback, 5> handlers{}; /**< Array of callbacks for buttons 0-4 */
};
