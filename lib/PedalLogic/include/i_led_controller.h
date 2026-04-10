#pragma once
#include <cstdint>

/**
 * @class ILEDController
 * @brief Interface for LED control functionality
 *
 * Supports both immediate state changes and non-blocking blink sequences.
 * Call update() every loop iteration to drive timed behaviour.
 */
class ILEDController
{
public:
    virtual ~ILEDController() = default;

    virtual void setup(uint32_t initialState = 0) = 0;
    virtual void setState(bool state) = 0;
    virtual void toggle() = 0;

    /**
     * @brief Start a blink sequence
     *
     * @param intervalMs Half-period in ms (LED on for intervalMs, off for intervalMs)
     * @param count Number of on/off cycles; -1 = blink indefinitely until stopBlink()
     */
    virtual void startBlink(uint32_t intervalMs, int16_t count = -1) = 0;

    /**
     * @brief Stop any running blink and restore the pre-blink state
     */
    virtual void stopBlink() = 0;

    /**
     * @brief Drive timed behaviour — must be called every loop iteration
     *
     * @param now Current time in milliseconds (e.g. millis())
     */
    virtual void update(uint32_t now) = 0;

    /**
     * @brief Returns true if a blink sequence is currently running
     */
    virtual bool isBlinking() const = 0;
};
