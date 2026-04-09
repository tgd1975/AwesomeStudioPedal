#pragma once
#include <string>

/**
 * @interface IFileSystem
 * @brief Abstract interface for file system operations
 * 
 * Provides a platform-independent way to perform file operations.
 * Implementations can use LittleFS, SD cards, or standard file systems.
 */
class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    /**
     * @brief Check if a file exists
     * @param path Path to the file
     * @return true if file exists, false otherwise
     */
    virtual bool exists(const char* path) = 0;

    /**
     * @brief Read entire file content
     * @param path Path to the file
     * @param content Output parameter for file content
     * @return true if successful, false otherwise
     */
    virtual bool readFile(const char* path, std::string& content) = 0;

    /**
     * @brief Write content to a file
     * @param path Path to the file
     * @param content Content to write
     * @return true if successful, false otherwise
     */
    virtual bool writeFile(const char* path, const std::string& content) = 0;
};
