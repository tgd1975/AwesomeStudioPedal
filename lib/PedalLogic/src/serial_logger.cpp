#include "i_logger.h"

#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include <iostream>
#endif

/**
 * @class SerialLogger
 * @brief Concrete logger implementation using serial output
 *
 * Implements ILogger interface using Serial for Arduino environment
 * and std::cout for host test environment.
 */
class SerialLogger : public ILogger
{
public:
    /**
     * @brief Logs a single message to serial output
     *
     * @param message The message to log
     */
    void log(const char* message) override
    {
#ifndef HOST_TEST_BUILD
        Serial.println(message);
#else
        std::cout << "DEBUG: " << message << '\n';
#endif
    }

    /**
     * @brief Logs a message with prefix to serial output
     *
     * @param prefix The prefix to prepend
     * @param message The message to log
     */
    void log(const char* prefix, const char* message) override
    {
#ifndef HOST_TEST_BUILD
        Serial.print(prefix);
        Serial.println(message);
#else
        std::cout << prefix << message << '\n';
#endif
    }
};

/**
 * @brief Creates and returns a SerialLogger instance
 *
 * @return Pointer to the singleton SerialLogger instance
 */
ILogger* createLogger()
{
    static SerialLogger instance;
    return &instance;
}
