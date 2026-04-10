#pragma once
#include "action.h"
#include <string>

/**
 * @class SerialOutputAction
 * @brief Action that outputs text to serial console
 *
 * Example of a non-send action that demonstrates the extensibility
 * of the Action hierarchy for debugging and logging purposes.
 */
class SerialOutputAction : public Action
{
private:
    std::string message; /**< Message to output to serial */

public:
    /**
     * @brief Constructs a SerialOutputAction
     *
     * @param message The message to output to serial console
     */
    explicit SerialOutputAction(std::string message);

    /**
     * @brief Executes the serial output action
     *
     * Outputs the configured message to the serial console.
     */
    void execute() override;

    /**
     * @brief Gets the type of this action
     *
     * @return Action type
     */
    Action::Type getType() const override { return Action::Type::SerialOutput; }

#ifndef HOST_TEST_BUILD
    void getJsonProperties(JsonObject& json) const override { json["value"] = message.c_str(); }
#endif

    /**
     * @brief Checks if this action is a send action
     *
     * @return false since this is not a send action
     */
    bool isSendAction() const override { return false; }

    /**
     * @brief Gets the message that will be output
     *
     * @return The message string
     */
    const std::string& getMessage() const { return message; }
};