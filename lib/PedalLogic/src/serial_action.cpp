#include "serial_action.h"
#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include <iostream>
#endif

SerialOutputAction::SerialOutputAction(const std::string& message) : message(message) {}

void SerialOutputAction::execute()
{
#ifndef HOST_TEST_BUILD
    Serial.print("ACTION: ");
    Serial.println(message.c_str());
#else
    std::cout << "ACTION: " << message << std::endl;
#endif
}