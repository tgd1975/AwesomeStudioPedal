#include "i_logger.h"

#include <iostream>

namespace
{
    class HostLogger : public ILogger
    {
    public:
        void log(const char* message) override { std::cout << "DEBUG: " << message << '\n'; }
        void log(const char* prefix, const char* message) override
        {
            std::cout << prefix << message << '\n';
        }
    };
} // namespace

// Host-side factory — replaces SerialLogger's createLogger() in host test builds.
// The production-side createLogger() in lib/PedalLogic/src/serial_logger.cpp returns
// a SerialLogger; this one returns a HostLogger that writes to std::cout, preserving
// the existing "DEBUG: " prefix that several tests assert on.
ILogger* createLogger()
{
    static HostLogger logger;
    return &logger;
}
