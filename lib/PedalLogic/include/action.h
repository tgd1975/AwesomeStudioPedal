#pragma once
#include <cstdint>
#include <memory>

#ifndef HOST_TEST_BUILD
#include <ArduinoJson.h>
#endif

/**
 * @class Action
 * @brief Base class for all pedal actions
 * 
 * Abstract base class that defines the interface for all actions
 * that can be executed by the pedal, including both send actions
 * and non-send actions.
 */
class Action
{
public:
    /**
     * @brief Action type identifiers for JSON serialization
     */
    enum class Type {
        Unknown,
        SendString,
        SendChar,
        SendKey,
        SendMediaKey,
        SerialOutput,
        Delayed
    };
    
    /**
     * @brief Gets the type of this action
     * 
     * @return The action type
     */
    virtual Type getType() const { return Type::Unknown; }
    
    /**
     * @brief Executes the action
     * 
     * Performs the action's primary function.
     */
    virtual void execute() = 0;
    
    /**
     * @brief Gets the delay time for this action
     * 
     * @return Delay time in milliseconds, 0 for immediate execution
     */
    virtual uint32_t getDelay() const { return 0; }
    
    /**
     * @brief Checks if this action is a send action
     * 
     * @return true if this action involves sending keyboard input, false otherwise
     */
    virtual bool isSendAction() const { return false; }
    
#ifndef HOST_TEST_BUILD
    virtual void getJsonProperties(JsonObject& json) const {}
#endif

    virtual ~Action() = default;
};

/**
 * @class DelayedAction
 * @brief Wrapper class for actions with delay
 * 
 * Decorates an action to add a delay before execution.
 */
class DelayedAction : public Action
{
private:
    std::unique_ptr<Action> action; /**< The action to be delayed */
    uint32_t delayMs; /**< Delay time in milliseconds */
    uint32_t startTime; /**< Time when delay started */
    bool started; /**< Whether delay has been started */

public:
    /**
     * @brief Constructs a DelayedAction
     * 
     * @param action Unique pointer to the action to delay
     * @param delayMs Delay time in milliseconds
     */
    DelayedAction(std::unique_ptr<Action> action, uint32_t delayMs);
    
    /**
     * @brief Executes the delayed action
     * 
     * If delay hasn't started, starts the delay.
     * If delay is complete, executes the wrapped action.
     */
    void execute() override;
    
    /**
     * @brief Gets the delay time for this action
     * 
     * @return Delay time in milliseconds
     */
    uint32_t getDelay() const override { return delayMs; }
    
    /**
     * @brief Checks if this action is a send action
     * 
     * @return true if the wrapped action is a send action, false otherwise
     */
    bool isSendAction() const override { return action->isSendAction(); }
    
    /**
     * @brief Updates the delay timer
     * 
     * @param currentTime Current time in milliseconds
     * @return true if delay is complete and action is ready to execute
     */
    bool update(uint32_t currentTime);
    
    /**
     * @brief Checks if the delay has been started
     * 
     * @return true if delay has been started
     */
    bool isStarted() const { return started; }
    
    /**
     * @brief Gets the inner action being delayed
     * 
     * @return Pointer to the inner action
     */
    const Action* getInnerAction() const { return action.get(); }
    
    /**
     * @brief Gets the type of this action
     * 
     * @return Action type
     */
    Action::Type getType() const override { return Action::Type::Delayed; }
    
#ifndef HOST_TEST_BUILD
    void getJsonProperties(JsonObject& json) const override;
    static const char* getTypeName(Action::Type type);
#endif
};