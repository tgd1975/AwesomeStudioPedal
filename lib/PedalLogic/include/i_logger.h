#pragma once

class ILogger
{
public:
    virtual ~ILogger() = default;
    virtual void log(const char* message) = 0;
    virtual void log(const char* prefix, const char* message) = 0;
};

ILogger* createLogger();
