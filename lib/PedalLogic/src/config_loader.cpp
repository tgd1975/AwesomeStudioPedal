#include "config_loader.h"
#include "button_constants.h"
#include "config.h"
#include "delayed_action.h"
#include "file_system.h"
#include "i_logger.h"
#include "key_lookup.h"
#include "non_send_action.h"
#include "send_action.h"
#include "serial_action.h"
#include <ArduinoJson.h>
#include <cstring>

#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include "../test/fakes/arduino_shim.h"
#endif

using namespace ArduinoJson;

// Forward declarations for platform-specific factories
IFileSystem* createFileSystem();
ILogger* createLogger(); // NOLINT(readability-redundant-declaration)

/**
 * @brief Default configuration JSON string
 *
 * Contains a single "Navigation" profile with media control button mappings.
 * Used as fallback when no custom configuration file is available.
 */
const char* ConfigLoader::DEFAULT_CONFIG =
    "{\n"
    "  \"profiles\": [\n"
    "    {\n"
    "      \"name\": \"Navigation\",\n"
    "      \"description\": \"Profile for controlling media playback\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Space\", \"value\": \" \"},\n"
    "        \"B\": {\"type\": \"SendMediaKeyAction\", \"name\": \"Stop\", \"value\": "
    "\"MEDIA_STOP\"},\n"
    "        \"C\": {\"type\": \"SendCharAction\", \"name\": \"Rewind\", \"value\": "
    "\"LEFT_ARROW\"},\n"
    "        \"D\": {\"type\": \"SendCharAction\", \"name\": \"Forward\", \"value\": "
    "\"RIGHT_ARROW\"}\n"
    "      }\n"
    "    }\n"
    "  ]\n"
    "}\n";

// ---- Constructors ----

/**
 * @brief Default constructor
 *
 * Creates a ConfigLoader using production singletons for file system and logging.
 * Used in firmware builds.
 */
ConfigLoader::ConfigLoader() : fileSystem_(createFileSystem()), logger_(createLogger()) {}

/**
 * @brief Dependency injection constructor
 *
 * Creates a ConfigLoader with injected dependencies for testing or custom implementations.
 *
 * @param fs File system implementation
 * @param logger Logger implementation
 */
ConfigLoader::ConfigLoader(IFileSystem* fs, ILogger* logger) : fileSystem_(fs), logger_(logger) {}

// ---- Public API ----

/**
 * @brief Loads configuration from a file
 *
 * Reads the configuration file from the specified path and parses it using loadFromString().
 *
 * @param profileManager The profile manager to populate with loaded profiles
 * @param keyboard The BLE keyboard interface for creating keyboard actions
 * @param configPath Path to the configuration file
 * @return true if loading succeeded, false if file read failed
 */
bool ConfigLoader::loadFromFile(ProfileManager& profileManager,
                                IBleKeyboard* keyboard,
                                const std::string& configPath)
{
    std::string content;
    if (! fileSystem_->readFile(configPath.c_str(), content))
    {
        logger_->log("Failed to read config file");
        return false;
    }
    return loadFromString(profileManager, keyboard, content);
}

/**
 * @brief Loads configuration from a JSON string
 *
 * Parses the JSON configuration and populates the profile manager with the defined profiles.
 * Clears existing profiles before loading new ones.
 *
 * @param profileManager The profile manager to populate
 * @param keyboard The BLE keyboard interface for creating keyboard actions
 * @param jsonConfig JSON string containing the configuration
 * @return true if parsing and loading succeeded, false if JSON parsing failed
 */
bool ConfigLoader::loadFromString(ProfileManager& profileManager,
                                  IBleKeyboard* keyboard,
                                  const std::string& jsonConfig)
{
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error)
    {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    for (uint8_t i = 0; i < hardwareConfig.numProfiles; i++)
    {
        profileManager.addProfile(i, nullptr);
    }

    JsonArray profiles = doc["profiles"];
    for (uint8_t i = 0; i < profiles.size() && i < hardwareConfig.numProfiles; i++)
    {
        JsonObject profileJson = profiles[i];
        const char* profileName = profileJson["name"] | "";
        const char* profileDescription = profileJson["description"] | "";

        auto newProfile = std::make_unique<Profile>(profileName);
        newProfile->setDescription(profileDescription);
        populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
        profileManager.addProfile(i, std::move(newProfile));
    }

    profileManager.resetToFirstProfile();
    logLoadedConfig(profileManager);
    return true;
}

// ---- Helper methods ----

/**
 * @brief Converts button name to button index
 *
 * @param buttonName The button name ("A", "B", "C", or "D")
 * @return The corresponding Btn:: index, or 255 if not found
 */
uint8_t ConfigLoader::getButtonIndex(const char* buttonName)
{
    if (buttonName[0] >= 'A' && buttonName[0] <= ('A' + Btn::MAX - 1) && buttonName[1] == '\0')
    {
        return static_cast<uint8_t>(buttonName[0] - 'A');
    }
    return 255;
}

/**
 * @brief Populates a profile with actions from JSON configuration
 *
 * Parses the buttons object from JSON and creates appropriate Action objects
 * for each button, adding them to the profile.
 *
 * @param profile The profile to populate
 * @param buttons JSON object containing button configurations
 * @param keyboard The BLE keyboard interface for creating keyboard actions
 */
void ConfigLoader::populateProfileFromJson(Profile& profile,
                                           JsonObject buttons,
                                           IBleKeyboard* keyboard)
{
    char buttonName[2];
    for (uint8_t b = 0; b < hardwareConfig.numButtons; b++)
    {
        Btn::name(b, buttonName);
        if (! buttons.containsKey(buttonName))
        {
            continue;
        }

        JsonObject actionJson = buttons[buttonName];

        std::unique_ptr<Action> action = createActionFromJson(actionJson, keyboard);
        if (action)
        {
            const char* actionName = actionJson["name"] | "";
            if (actionName[0] != '\0')
            {
                action->setName(actionName);
            }
            profile.addAction(b, std::move(action));
        }
    }
}

/**
 * @brief Creates a SendChar action from JSON, handling named keys and raw ASCII
 */
std::unique_ptr<Action> ConfigLoader::createSendCharActionFromJson(const JsonObject& actionJson,
                                                                   IBleKeyboard* keyboard)
{
    const char* value = actionJson["value"] | "";
    uint8_t code = lookupKey(value);
    if (code != 0)
    {
        return std::unique_ptr<Action>(new SendCharAction(keyboard, static_cast<char>(code)));
    }
    // Single printable ASCII character (e.g. "[", "]", "c", " ")
    if (value[0] != '\0' && value[1] == '\0')
    {
        return std::unique_ptr<Action>(new SendCharAction(keyboard, value[0]));
    }
    logger_->log("SendChar: unknown key value: ", value);
    return nullptr;
}

/**
 * @brief Creates an Action object from JSON configuration
 *
 * Parses the action JSON object and creates the appropriate Action subclass
 * based on the "type" field. Supports all action types including nested
 * DelayedAction with inner actions.
 *
 * @param actionJson JSON object containing action configuration
 * @param keyboard BLE keyboard interface for keyboard actions
 * @return Unique pointer to the created Action, or nullptr if type is unknown or invalid
 */
std::unique_ptr<Action> ConfigLoader::createActionFromJson(const JsonObject& actionJson,
                                                           IBleKeyboard* keyboard)
{
    const char* typeName = actionJson["type"] | "";
    Action::Type type = lookupActionType(typeName);

    switch (type)
    {
        case Action::Type::SendString:
        {
            const char* value = actionJson["value"] | "";
            return std::unique_ptr<Action>(new SendStringAction(keyboard, value));
        }
        case Action::Type::SendChar:
            return createSendCharActionFromJson(actionJson, keyboard);
        case Action::Type::SendKey:
        {
            uint8_t code = lookupKey(actionJson["value"] | "");
            if (code != 0)
            {
                return std::unique_ptr<Action>(new SendKeyAction(keyboard, code));
            }
            break;
        }
        case Action::Type::SendMediaKey:
        {
            const uint8_t* report = lookupMediaKey(actionJson["value"] | "");
            if (report)
            {
                return std::unique_ptr<Action>(new SendMediaKeyAction(keyboard, report));
            }
            break;
        }
        case Action::Type::SerialOutput:
        {
            const char* value = actionJson["value"] | "";
            return std::unique_ptr<Action>(new SerialOutputAction(value));
        }
        case Action::Type::Delayed:
        {
            uint32_t delayMs = actionJson["delayMs"] | 0U;
            JsonObject nestedJson = actionJson["action"];
            std::unique_ptr<Action> inner = createActionFromJson(nestedJson, keyboard);
            if (inner)
            {
                return std::unique_ptr<Action>(new DelayedAction(std::move(inner), delayMs));
            }
            break;
        }
        default:
            break;
    }

    return nullptr;
}

