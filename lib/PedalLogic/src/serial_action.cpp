#include "serial_action.h"

#include <Arduino.h>

SerialOutputAction::SerialOutputAction(std::string message) : message(std::move(message)) {}

void SerialOutputAction::execute()
{
    Serial.print("ACTION: ");
    Serial.println(message.c_str());
}
