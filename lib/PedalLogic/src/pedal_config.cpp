#include "pedal_config.h"
#include "action.h"
#include "button_constants.h"
#include "config.h"
#include "config_loader.h"
#include "profile.h"
#include "send_action.h"
#include "serial_action.h"
#include <ArduinoJson.h>
#include <memory>
#include <string>

// Forward declaration for platform-specific factory (avoids pulling in full DI headers)
IFileSystem* createFileSystem();
ILogger* createLogger();

/**
 * @brief Load hardware config overrides from /config.json (LittleFS) if present.
 */
void loadHardwareConfig()
{
    IFileSystem* fs = createFileSystem();
    ILogger* logger = createLogger();

    std::string content;
    if (! fs->readFile("/config.json", content))
    {
        // File absent — use compiled-in defaults silently
        return;
    }

    ArduinoJson::DynamicJsonDocument doc(1024);
    auto err = ArduinoJson::deserializeJson(doc, content);
    if (err)
    {
        logger->log("loadHardwareConfig: JSON parse failed:", err.c_str());
        return;
    }

    if (doc.containsKey("numProfiles"))
        hardwareConfig.numProfiles = doc["numProfiles"];
    if (doc.containsKey("numSelectLeds"))
        hardwareConfig.numSelectLeds = doc["numSelectLeds"];
    if (doc.containsKey("numButtons"))
        hardwareConfig.numButtons = doc["numButtons"];
    if (doc.containsKey("ledBluetooth"))
        hardwareConfig.ledBluetooth = doc["ledBluetooth"];
    if (doc.containsKey("ledPower"))
        hardwareConfig.ledPower = doc["ledPower"];
    if (doc.containsKey("buttonSelect"))
        hardwareConfig.buttonSelect = doc["buttonSelect"];

    if (doc.containsKey("ledSelect"))
    {
        ArduinoJson::JsonArray arr = doc["ledSelect"];
        uint8_t n = arr.size() < 6 ? arr.size() : 6;
        for (uint8_t i = 0; i < n; i++)
            hardwareConfig.ledSelect[i] = arr[i];
    }

    if (doc.containsKey("buttonPins"))
    {
        ArduinoJson::JsonArray arr = doc["buttonPins"];
        uint8_t n = arr.size() < 26 ? arr.size() : 26;
        for (uint8_t i = 0; i < n; i++)
            hardwareConfig.buttonPins[i] = arr[i];
    }

    logger->log("loadHardwareConfig: overrides applied from /config.json");
}

/**
 * @brief Load profiles from file; fall back to DEFAULT_CONFIG on failure.
 *
 * Returns false only if DEFAULT_CONFIG itself fails to parse — which indicates
 * a programming error and should never happen at runtime.
 */
bool configureProfiles(ProfileManager& profileManager, IBleKeyboard* keyboard)
{
    ConfigLoader configLoader;

    if (configLoader.loadFromFile(profileManager, keyboard, "/profiles.json"))
    {
        return true;
    }

    // File missing or invalid — load the hardcoded default
    if (! configLoader.loadFromString(profileManager, keyboard, ConfigLoader::getDefaultConfig()))
    {
        // DEFAULT_CONFIG failed to parse: this is a compile-time bug, not a runtime error
        return false;
    }

    // Persist the default so future boots load from file
    configLoader.saveToFile(profileManager, "/profiles.json");
    return true;
}
