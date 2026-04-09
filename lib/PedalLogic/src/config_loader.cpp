#include "config_loader.h"
#include "delayed_action.h"
#include "file_system.h"
#include "i_logger.h"
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
ILogger* createLogger();

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

// ---- Key/action lookup tables ----

namespace
{

    struct KeyEntry
    {
        const char* name;
        uint8_t code;
    };
    struct MediaKeyEntry
    {
        const char* name;
        const uint8_t* report;
    };
    struct ActionTypeEntry
    {
        const char* name;
        Action::Type type;
    };

    static const KeyEntry KEY_TABLE[] = {
        {"LEFT_ARROW", KEY_LEFT_ARROW},
        {"RIGHT_ARROW", KEY_RIGHT_ARROW},
        {"UP_ARROW", KEY_UP_ARROW},
        {"DOWN_ARROW", KEY_DOWN_ARROW},
    };

    static const MediaKeyEntry MEDIA_KEY_TABLE[] = {
        {"MEDIA_STOP", KEY_MEDIA_STOP},
    };

    static const ActionTypeEntry ACTION_TYPE_TABLE[] = {
        {"SendStringAction", Action::Type::SendString},
        {"SendCharAction", Action::Type::SendChar},
        {"SendKeyAction", Action::Type::SendKey},
        {"SendMediaKeyAction", Action::Type::SendMediaKey},
        {"SerialOutputAction", Action::Type::SerialOutput},
        {"DelayedAction", Action::Type::Delayed},
    };

    /**
     * @brief Looks up an action type by name
     *
     * @param name The action type name to look up
     * @return The corresponding Action::Type enum value, or Action::Type::Unknown if not found
     */
    Action::Type lookupActionType(const char* name)
    {
        for (const auto& e : ACTION_TYPE_TABLE)
        {
            if (strcmp(e.name, name) == 0)
                return e.type;
        }
        return Action::Type::Unknown;
    }

    /**
     * @brief Looks up a key code by name
     *
     * @param name The key name to look up (e.g., "LEFT_ARROW")
     * @return The corresponding USB HID key code, or 0 if not found
     */
    uint8_t lookupKey(const char* name)
    {
        for (const auto& e : KEY_TABLE)
        {
            if (strcmp(e.name, name) == 0)
                return e.code;
        }
        return 0;
    }

    /**
     * @brief Looks up a media key report by name
     *
     * @param name The media key name to look up (e.g., "MEDIA_STOP")
     * @return Pointer to the USB HID report for the media key, or nullptr if not found
     */
    const uint8_t* lookupMediaKey(const char* name)
    {
        for (const auto& e : MEDIA_KEY_TABLE)
        {
            if (strcmp(e.name, name) == 0)
                return e.report;
        }
        return nullptr;
    }

} // namespace

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
 * @brief Saves configuration to a file
 *
 * Serializes the current profile manager state to JSON and writes it to the specified file.
 * Includes all profiles with their names, descriptions, and button action configurations.
 *
 * @param profileManager The profile manager containing profiles to save
 * @param configPath Path to the configuration file to write
 * @return true if saving succeeded, false if file write failed
 */
bool ConfigLoader::saveToFile(const ProfileManager& profileManager, const std::string& configPath)
{
    DynamicJsonDocument doc(2048);
    JsonArray profiles = doc.createNestedArray("profiles");

    for (uint8_t profileIndex = 0; profileIndex < ProfileManager::NUM_PROFILES; profileIndex++)
    {
        const Profile* profile = profileManager.getProfile(profileIndex);
        if (! profile)
            continue;

        JsonObject profileObj = profiles.createNestedObject();
        profileObj["name"] = profile->getName().c_str();
        profileObj["description"] = profile->getDescription().c_str();

        JsonObject buttons = profileObj.createNestedObject("buttons");

        for (const char* buttonName : {"A", "B", "C", "D"})
        {
            uint8_t buttonIndex = getButtonIndex(buttonName);
            Action* action = profile->getAction(buttonIndex);
            if (action)
            {
                JsonObject actionObj = buttons.createNestedObject(buttonName);
                actionToJson(action, actionObj);
            }
        }
    }

    std::string content;
    serializeJson(doc, content);

    if (! fileSystem_->writeFile(configPath.c_str(), content))
    {
        logger_->log("Failed to write config file");
        return false;
    }

    return true;
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
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error)
    {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++)
    {
        profileManager.addProfile(i, nullptr);
    }

    JsonArray profiles = doc["profiles"];
    for (uint8_t i = 0; i < profiles.size() && i < ProfileManager::NUM_PROFILES; i++)
    {
        JsonObject profileJson = profiles[i];
        const char* profileName = profileJson["name"] | "";
        const char* profileDescription = profileJson["description"] | "";

        auto newProfile = std::unique_ptr<Profile>(new Profile(profileName));
        newProfile->setDescription(profileDescription);
        populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
        profileManager.addProfile(i, std::move(newProfile));
    }

    profileManager.resetToFirstProfile();
    return true;
}

/**
 * @brief Merges configuration from JSON into existing profiles
 *
 * Adds new profiles from the JSON configuration to the profile manager,
 * skipping any profiles that already exist by name. Only adds profiles
 * to empty slots in the profile manager.
 *
 * @param profileManager The profile manager to merge profiles into
 * @param keyboard The BLE keyboard interface for creating keyboard actions
 * @param jsonConfig JSON string containing profiles to merge
 * @return true if merging succeeded, false if JSON parsing failed
 */
bool ConfigLoader::mergeConfig(ProfileManager& profileManager,
                               IBleKeyboard* keyboard,
                               const std::string& jsonConfig)
{
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error)
    {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    JsonArray profiles = doc["profiles"];

    for (uint8_t newIdx = 0; newIdx < profiles.size() && newIdx < ProfileManager::NUM_PROFILES;
         newIdx++)
    {
        JsonObject profileJson = profiles[newIdx];
        const char* profileName = profileJson["name"] | "";
        const char* profileDescription = profileJson["description"] | "";

        // Skip if a profile with the same name already exists
        bool exists = false;
        for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++)
        {
            const Profile* p = profileManager.getProfile(i);
            if (p && p->getName() == profileName)
            {
                exists = true;
                break;
            }
        }
        if (exists)
        {
            logger_->log("Profile already exists, skipping: ", profileName);
            continue;
        }

        // Find first empty slot
        uint8_t targetIndex = 255;
        for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++)
        {
            if (! profileManager.getProfile(i))
            {
                targetIndex = i;
                break;
            }
        }
        if (targetIndex == 255)
        {
            logger_->log("No empty profile slots available for merging");
            continue;
        }

        auto newProfile = std::unique_ptr<Profile>(new Profile(profileName));
        newProfile->setDescription(profileDescription);
        populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
        profileManager.addProfile(targetIndex, std::move(newProfile));
        logger_->log("Added profile: ", profileName);
    }

    return true;
}

/**
 * @brief Replaces a specific profile with configuration from JSON
 *
 * Parses the JSON configuration and replaces the profile at the specified index
 * with the first profile found in the JSON. Validates the profile index before
 * attempting replacement.
 *
 * @param profileManager The profile manager containing the profile to replace
 * @param keyboard The BLE keyboard interface for creating keyboard actions
 * @param profileIndex Index of the profile to replace (0-3)
 * @param jsonConfig JSON string containing the replacement profile
 * @return true if replacement succeeded, false if validation failed or JSON parsing failed
 */
bool ConfigLoader::replaceProfile(ProfileManager& profileManager,
                                  IBleKeyboard* keyboard,
                                  uint8_t profileIndex,
                                  const std::string& jsonConfig)
{
    if (profileIndex >= ProfileManager::NUM_PROFILES)
    {
        logger_->log("Invalid profile index");
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error)
    {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    JsonArray profiles = doc["profiles"];
    if (profiles.size() == 0)
    {
        logger_->log("No profiles found in JSON config");
        return false;
    }

    JsonObject profileJson = profiles[0];
    const char* profileName = profileJson["name"] | "";
    const char* profileDescription = profileJson["description"] | "";

    auto newProfile = std::unique_ptr<Profile>(new Profile(profileName));
    newProfile->setDescription(profileDescription);
    populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
    profileManager.addProfile(profileIndex, std::move(newProfile));
    logger_->log("Replaced profile with: ", profileName);

    return true;
}

// ---- Helper methods ----

/**
 * @brief Converts button name to button index
 *
 * @param buttonName The button name ("A", "B", "C", or "D")
 * @return The corresponding Button:: index, or 255 if not found
 */
uint8_t ConfigLoader::getButtonIndex(const char* buttonName)
{
    if (strcmp(buttonName, "A") == 0)
        return Button::A;
    if (strcmp(buttonName, "B") == 0)
        return Button::B;
    if (strcmp(buttonName, "C") == 0)
        return Button::C;
    if (strcmp(buttonName, "D") == 0)
        return Button::D;
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
    for (const char* buttonName : {"A", "B", "C", "D"})
    {
        if (! buttons.containsKey(buttonName))
            continue;

        JsonObject actionJson = buttons[buttonName];
        uint8_t buttonIndex = getButtonIndex(buttonName);

        std::unique_ptr<Action> action = createActionFromJson(actionJson, keyboard);
        if (action)
        {
            const char* actionName = actionJson["name"] | "";
            if (actionName[0] != '\0')
            {
                action->setName(actionName);
            }
            profile.addAction(buttonIndex, std::move(action));
        }
    }
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
        {
            uint8_t code = lookupKey(actionJson["value"] | "");
            if (code != 0)
                return std::unique_ptr<Action>(new SendCharAction(keyboard, code));
            break;
        }
        case Action::Type::SendKey:
        {
            uint8_t code = lookupKey(actionJson["value"] | "");
            if (code != 0)
                return std::unique_ptr<Action>(new SendKeyAction(keyboard, code));
            break;
        }
        case Action::Type::SendMediaKey:
        {
            const uint8_t* report = lookupMediaKey(actionJson["value"] | "");
            if (report)
                return std::unique_ptr<Action>(new SendMediaKeyAction(keyboard, report));
            break;
        }
        case Action::Type::SerialOutput:
        {
            const char* value = actionJson["value"] | "";
            return std::unique_ptr<Action>(new SerialOutputAction(value));
        }
        case Action::Type::Delayed:
        {
            uint32_t delayMs = actionJson["delayMs"] | 0u;
            JsonObject nestedJson = actionJson["action"];
            std::unique_ptr<Action> inner = createActionFromJson(nestedJson, keyboard);
            if (inner)
                return std::unique_ptr<Action>(new DelayedAction(std::move(inner), delayMs));
            break;
        }
        default:
            break;
    }

    return nullptr;
}

/**
 * @brief Serializes an Action object to JSON
 *
 * Converts an Action object to its JSON representation for saving to configuration files.
 * Includes the action type, name (if set), and type-specific properties.
 *
 * @param action The action to serialize
 * @param out The JSON object to populate with action properties
 */
void ConfigLoader::actionToJson(const Action* action, JsonObject& out) const
{
    if (action->hasName())
    {
        out["name"] = action->getName().c_str();
    }

    switch (action->getType())
    {
        case Action::Type::SendString:
            out["type"] = "SendStringAction";
            action->getJsonProperties(out);
            break;
        case Action::Type::SendChar:
            out["type"] = "SendCharAction";
            action->getJsonProperties(out);
            break;
        case Action::Type::SendKey:
            out["type"] = "SendKeyAction";
            action->getJsonProperties(out);
            break;
        case Action::Type::SendMediaKey:
            out["type"] = "SendMediaKeyAction";
            action->getJsonProperties(out);
            break;
        case Action::Type::SerialOutput:
            out["type"] = "SerialOutputAction";
            action->getJsonProperties(out);
            break;
        case Action::Type::Delayed:
            out["type"] = "DelayedAction";
            action->getJsonProperties(out);
            break;
        default:
            out["type"] = "UnknownAction";
            break;
    }
}
