#include "file_system.h"
#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#include <LittleFS.h>
#else
#include <fstream>
#endif

/**
 * @class LittleFSFileSystem
 * @brief File system implementation using LittleFS for embedded targets
 */
class LittleFSFileSystem : public IFileSystem {
public:
    bool exists(const char* path) override {
#ifndef HOST_TEST_BUILD
        return LittleFS.exists(path);
#else
        // Mock implementation for host testing
        FILE* file = fopen(path, "r");
        if (file) {
            fclose(file);
            return true;
        }
        return false;
#endif
    }

    bool readFile(const char* path, std::string& content) override {
#ifndef HOST_TEST_BUILD
        if (!LittleFS.begin(true)) {
            return false;
        }

        File file = LittleFS.open(path, "r");
        if (!file) {
            return false;
        }

        String arduinoContent = file.readString();
        content = arduinoContent.c_str();
        file.close();
        return true;
#else
        // Host implementation using standard C++ files
        std::ifstream inFile(path);
        if (!inFile) {
            return false;
        }

        content = std::string((std::istreambuf_iterator<char>(inFile)),
                             std::istreambuf_iterator<char>());
        return true;
#endif
    }

    bool writeFile(const char* path, const std::string& content) override {
#ifndef HOST_TEST_BUILD
        if (!LittleFS.begin(true)) {
            return false;
        }

        File file = LittleFS.open(path, "w");
        if (!file) {
            return false;
        }

        if (file.print(content.c_str()) == 0) {
            file.close();
            return false;
        }

        file.close();
        return true;
#else
        // Host implementation using standard C++ files
        std::ofstream outFile(path);
        if (!outFile) {
            return false;
        }

        outFile << content;
        return outFile.good();
#endif
    }
};

// Factory function to create the appropriate file system
IFileSystem* createFileSystem() {
    static LittleFSFileSystem instance;
    return &instance;
}