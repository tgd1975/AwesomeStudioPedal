#pragma once

/**
 * @class ILogger
 * @brief Interface for logging functionality
 *
 * Provides abstract logging interface that can be implemented
 * for different environments (Arduino, host testing, etc.)
 */
class ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Logs a single message
     *
     * @param message The message to log
     */
    virtual void log(const char* message) = 0;

    /**
     * @brief Logs a message with a prefix
     *
     * @param prefix The prefix to prepend to the message
     * @param message The message to log
     */
    virtual void log(const char* prefix, const char* message) = 0;
};

/**
 * @brief Creates a logger instance
 *
 * @return Pointer to a logger implementation
 */
ILogger* createLogger();
