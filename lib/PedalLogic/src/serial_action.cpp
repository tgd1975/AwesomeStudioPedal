#include "serial_action.h"
#include <Arduino.h>

SerialOutputAction::SerialOutputAction(const std::string& message)
    : message(message)
{
}

void SerialOutputAction::execute()
{
    Serial.print("ACTION: ");
    Serial.println(message.c_str());
}