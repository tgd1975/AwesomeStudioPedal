#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <unordered_map>

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
     * @brief Registers a callback function for a button press
     *
     * @param button Button index (0-4)
     * @param callback Function to call when button is pressed
     */
    void registerHandler(uint8_t button, EventCallback callback);

    /**
     * @brief Executes the press callback for the specified button
     *
     * @param button Button index (0-4) to dispatch
     */
    void dispatch(uint8_t button);

    /**
     * @brief Registers a callback for a button release event
     *
     * @param button Button index (0-3; SELECT does not emit release events)
     * @param callback Function to call on release
     */
    void registerReleaseHandler(uint8_t button, EventCallback callback);

    /**
     * @brief Executes the release callback for the specified button, if registered
     *
     * @param button Button index (0-3)
     */
    void dispatchRelease(uint8_t button);

    /**
     * @brief Registers a callback for a long press on a button
     *
     * @param button Button index (0-4)
     * @param cb Function to call on long press
     * @param thresholdMs Hold duration in ms before firing (default 500)
     */
    void registerLongPressHandler(uint8_t button, EventCallback cb, uint32_t thresholdMs = 500);

    /**
     * @brief Registers a callback for a double press on a button
     *
     * @param button Button index (0-4)
     * @param cb Function to call on double press
     */
    void registerDoublePressHandler(uint8_t button, EventCallback cb);

    /**
     * @brief Executes the long-press callback for the specified button
     *
     * @param button Button index (0-4)
     */
    void dispatchLongPress(uint8_t button);

    /**
     * @brief Executes the double-press callback for the specified button
     *
     * @param button Button index (0-4)
     */
    void dispatchDoublePress(uint8_t button);

    /**
     * @brief Returns the registered long-press threshold for a button (0 if none)
     */
    uint32_t getLongPressThreshold(uint8_t button) const;

    /**
     * @brief Clears all registered event handlers (press and release)
     */
    void clearHandlers();

private:
    std::array<EventCallback, 5> handlers{};             /**< Press callbacks for buttons 0-4 */
    std::array<EventCallback, 5> releaseHandlers{};      /**< Release callbacks for buttons 0-4 */
    std::array<EventCallback, 5> longPressHandlers_{};   /**< Long-press callbacks */
    std::array<EventCallback, 5> doublePressHandlers_{}; /**< Double-press callbacks */
    std::array<uint32_t, 5> longPressThresholds_{};      /**< Per-button long-press thresholds */
};
