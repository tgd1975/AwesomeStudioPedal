#pragma once

/**
 * @class IButton
 * @brief Interface for interrupt-driven button input
 *
 * Abstracts button press detection so that PedalLogic is independent
 * of the concrete hardware ISR mechanism. Implementations live in the
 * platform-specific hardware packages.
 */
class IButton
{
public:
    virtual ~IButton() = default;

    /**
     * @brief Initializes the GPIO pin for button input
     *
     * Configures the pin as input with pull-up and any platform-specific
     * GPIO setup required before interrupts can be attached.
     */
    virtual void setup() = 0;

    /**
     * @brief Checks for a button press event and clears the flag
     *
     * @return true if the button was pressed since the last call, false otherwise
     */
    virtual bool event() = 0;

    /**
     * @brief Resets the pressed flag
     *
     * Called before attaching interrupts to ensure no stale press is
     * processed immediately after the interrupt handler is installed.
     */
    virtual void reset() = 0;

    /**
     * @brief Checks for a button release event and clears the flag
     *
     * Returns true exactly once per release edge (HIGH after press), then resets.
     * Uses the ISR's awaitingRelease flag — safe to call from the main loop.
     */
    virtual bool releaseEvent() { return false; }
};
