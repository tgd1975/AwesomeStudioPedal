#pragma once
#include "action.h"
#include "button_constants.h"
#include "file_system.h"
#include "i_ble_keyboard.h"
#include "i_logger.h"
#include "profile_manager.h"
#include <ArduinoJson.h>
#include <string>

/**
 * @class ConfigLoader
 * @brief Loads pedal configuration from JSON files
 *
 * Responsible for parsing JSON configuration files and creating the appropriate
 * Action objects to populate the ProfileManager.
 */
class ConfigLoader
{
public:
    /// Default constructor: uses production singletons (firmware path)
    ConfigLoader();

    /// Injection constructor: use in tests or wherever DI is needed
    ConfigLoader(IFileSystem* fs, ILogger* logger);

    bool loadFromFile(ProfileManager& profileManager,
                      IBleKeyboard* keyboard,
                      const std::string& configPath);
    bool loadFromString(ProfileManager& profileManager,
                        IBleKeyboard* keyboard,
                        const std::string& jsonConfig);
    bool saveToFile(const ProfileManager& profileManager, const std::string& configPath);
    bool mergeConfig(ProfileManager& profileManager,
                     IBleKeyboard* keyboard,
                     const std::string& jsonConfig);
    bool replaceProfile(ProfileManager& profileManager,
                        IBleKeyboard* keyboard,
                        uint8_t profileIndex,
                        const std::string& jsonConfig);

    const char* getDefaultConfig() const { return DEFAULT_CONFIG; }

    static uint8_t getButtonIndex(const char* buttonName);

private:
    static const char* DEFAULT_CONFIG;

    IFileSystem* fileSystem_;
    ILogger* logger_;

    std::unique_ptr<Action> createActionFromJson(const ArduinoJson::JsonObject& actionJson,
                                                 IBleKeyboard* keyboard);
    void actionToJson(const Action* action, ArduinoJson::JsonObject& out) const;
    void populateProfileFromJson(Profile& profile,
                                 ArduinoJson::JsonObject buttons,
                                 IBleKeyboard* keyboard);
};
