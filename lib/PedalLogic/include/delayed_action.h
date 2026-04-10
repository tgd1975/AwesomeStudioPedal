#pragma once
#include "action.h"
#include <ArduinoJson.h>
#include <cstdint>
#include <memory>

/**
 * @class DelayedAction
 * @brief Wraps an action and defers its execution by a configurable delay
 *
 * The first call to execute() starts the timer. Subsequent calls before the
 * delay has elapsed are ignored. Once the delay elapses the inner action is
 * executed and the timer resets, allowing the action to be triggered again.
 */
class DelayedAction : public Action
{
public:
    DelayedAction(std::unique_ptr<Action> action, uint32_t delayMs);

    void execute() override;

    uint32_t getDelay() const override { return delayMs; }
    bool isSendAction() const override { return action->isSendAction(); }
    bool isInProgress() const override { return started; }
    Action::Type getType() const override { return Action::Type::Delayed; }

    /**
     * @brief Polls whether the delay has elapsed
     *
     * @param currentTime Current time in milliseconds (e.g. millis())
     * @return true if the delay has elapsed and the inner action should fire
     */
    bool update(uint32_t currentTime) const;

    bool isStarted() const { return started; }
    const Action* getInnerAction() const { return action.get(); }

    void getJsonProperties(JsonObject& json) const override;
    static const char* getTypeName(Action::Type type);

private:
    std::unique_ptr<Action> action;
    uint32_t delayMs;
    uint32_t startTime = 0;
    bool started = false;
};
