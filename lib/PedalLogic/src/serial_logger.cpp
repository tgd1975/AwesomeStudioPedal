#include "i_logger.h"

#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include <iostream>
#endif

class SerialLogger : public ILogger
{
public:
    void log(const char* message) override
    {
#ifndef HOST_TEST_BUILD
        Serial.println(message);
#else
        std::cout << "DEBUG: " << message << std::endl;
#endif
    }

    void log(const char* prefix, const char* message) override
    {
#ifndef HOST_TEST_BUILD
        Serial.print(prefix);
        Serial.println(message);
#else
        std::cout << prefix << message << std::endl;
#endif
    }
};

ILogger* createLogger()
{
    static SerialLogger instance;
    return &instance;
}
