#include <Arduino.h>
#include <LittleFS.h>

#include "file_system.h"

namespace
{
    class Esp32FileSystem : public IFileSystem
    {
    public:
        bool exists(const char* path) override { return LittleFS.exists(path); }

        bool readFile(const char* path, std::string& content) override
        {
            if (! LittleFS.begin(true))
                return false;

            File file = LittleFS.open(path, "r");
            if (! file)
                return false;

            String arduinoContent = file.readString();
            content = arduinoContent.c_str();
            file.close();
            return true;
        }

        bool writeFile(const char* path, const std::string& content) override
        {
            if (! LittleFS.begin(true))
                return false;

            File file = LittleFS.open(path, "w");
            if (! file)
                return false;

            if (file.print(content.c_str()) == 0)
            {
                file.close();
                return false;
            }

            file.close();
            return true;
        }
    };
} // namespace

IFileSystem* createFileSystem()
{
    static Esp32FileSystem instance;
    return &instance;
}
