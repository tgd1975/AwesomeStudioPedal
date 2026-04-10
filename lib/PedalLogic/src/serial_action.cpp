#include "serial_action.h"
#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include <iostream>
#endif

/**
 * @brief Constructs a SerialOutputAction with the specified message
 */
SerialOutputAction::SerialOutputAction(std::string message) : message(std::move(message)) {}

/**
 * @brief Executes the serial output action
 *
 * Outputs the configured message to the serial console with an "ACTION: " prefix.
 * This method handles both Arduino environment (Serial) and host test environment (std::cout).
 *
 * @note In Arduino environment, uses Serial.print/println
 * @note In host test environment, uses std::cout
 */
void SerialOutputAction::execute()
{
#ifndef HOST_TEST_BUILD
    Serial.print("ACTION: ");
    Serial.println(message.c_str());
#else
    std::cout << "ACTION: " << message << '\n';
#endif
}