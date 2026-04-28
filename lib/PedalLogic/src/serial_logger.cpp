#include "i_logger.h"

#include <Arduino.h>

namespace
{
    class SerialLogger : public ILogger
    {
    public:
        void log(const char* message) override { Serial.println(message); }

        void log(const char* prefix, const char* message) override
        {
            Serial.print(prefix);
            Serial.println(message);
        }
    };
} // namespace

ILogger* createLogger()
{
    static SerialLogger instance;
    return &instance;
}
