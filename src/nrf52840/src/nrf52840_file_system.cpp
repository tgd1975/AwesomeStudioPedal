#include <Adafruit_LittleFS.h>
#include <Arduino.h>
#include <InternalFileSystem.h>

#include "file_system.h"

using namespace Adafruit_LittleFS_Namespace;

namespace
{
    class Nrf52840FileSystem : public IFileSystem
    {
    public:
        bool exists(const char* path) override { return InternalFS.exists(path); }

        bool readFile(const char* path, std::string& content) override
        {
            if (! InternalFS.begin())
                return false;

            File file = InternalFS.open(path, FILE_O_READ);
            if (! file)
                return false;

            String arduinoContent = file.readString();
            content = arduinoContent.c_str();
            file.close();
            return true;
        }

        bool writeFile(const char* path, const std::string& content) override
        {
            if (! InternalFS.begin())
                return false;

            File file = InternalFS.open(path, FILE_O_WRITE);
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
    static Nrf52840FileSystem instance;
    return &instance;
}
