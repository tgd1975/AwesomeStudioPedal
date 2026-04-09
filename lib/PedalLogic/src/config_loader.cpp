#include "config_loader.h"
#include "file_system.h"
#include "send.h"
#include "serial_action.h"
#include "non_send_action.h"
#include <ArduinoJson.h>
#include <cstring>
#include <typeinfo>

// Arduino compatibility and minimal includes
#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include "../test/fakes/arduino_shim.h"
#include <iostream>
#include <fstream>
#include <sstream>
#endif

// Forward declaration for the file system factory
IFileSystem* createFileSystem();

// Default configuration (same as the JSON file we created)
const char* ConfigLoader::DEFAULT_CONFIG =
    "{\n"
    "  \"profiles\": [\n"
    "    {\n"
    "      \"name\": \"Navigation\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Space\", \"value\": \" \"},\n"
    "        \"B\": {\"type\": \"SendMediaKeyAction\", \"name\": \"Stop\", \"value\": \"MEDIA_STOP\"},\n"
    "        \"C\": {\"type\": \"SendCharAction\", \"name\": \"Rewind\", \"value\": \"LEFT_ARROW\"},\n"
    "        \"D\": {\"type\": \"SendCharAction\", \"name\": \"Forward\", \"value\": \"RIGHT_ARROW\"}\n"
    "      }\n"
    "    },\n"
    "    {\n"
    "      \"name\": \"Messaging\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Hello\", \"value\": \"Hello\"},\n"
    "        \"B\": {\"type\": \"SendStringAction\", \"name\": \"World\", \"value\": \"World\"},\n"
    "        \"C\": {\"type\": \"SendKeyAction\", \"name\": \"Scroll Up\", \"value\": \"UP_ARROW\"},\n"
    "        \"D\": {\"type\": \"SendKeyAction\", \"name\": \"Scroll Down\", \"value\": \"DOWN_ARROW\"}\n"
    "      }\n"
    "    },\n"
    "    {\n"
    "      \"name\": \"Custom\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"name\": \"Custom A\", \"value\": \"Profile 2 A\"},\n"
    "        \"B\": {\"type\": \"SerialOutputAction\", \"name\": \"Serial Debug\", \"value\": \"Button B pressed - Custom Profile\"},\n"
    "        \"C\": {\"type\": \"DelayedAction\", \"name\": \"Delayed Send\", \"delayMs\": 5000, \"action\": {\"type\": \"SendStringAction\", \"value\": \"Profile 2 C\"}},\n"
    "        \"D\": {\"type\": \"SendStringAction\", \"name\": \"Custom D\", \"value\": \"Profile 2 D\"}\n"
    "      }\n"
    "    }\n"
    "  ]\n"
    "}\n";

// Debug output abstraction
void ConfigLoader::debugOutput(const char* message) {
#ifndef HOST_TEST_BUILD
    Serial.println(message);
#else
    std::cout << "DEBUG: " << message << std::endl;
#endif
}

void ConfigLoader::debugOutput(const char* prefix, const char* message) {
#ifndef HOST_TEST_BUILD
    Serial.print(prefix);
    Serial.println(message);
#else
    std::cout << prefix << message << std::endl;
#endif
}

/**
 * @brief File system instance for configuration storage
 * 
 * Singleton instance of the file system interface. Uses LittleFS on embedded targets
 * and standard file system on host for testing.
 */
static IFileSystem* fileSystem = createFileSystem();

/**
 * @brief Load configuration from a file
 * 
 * Reads configuration data from a file and loads it into the profile manager.
 * Works on both embedded targets (using LittleFS) and host systems (using standard files).
 * 
 * @param profileManager Profile manager to populate with loaded configuration
 * @param keyboard BLE keyboard interface for creating send actions
 * @param configPath Path to the configuration file
 * @return true if loading succeeded, false otherwise
 */
bool ConfigLoader::loadFromFile(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& configPath) {
    std::string content;
    if (!fileSystem->readFile(configPath.c_str(), content)) {
        debugOutput("Failed to read config file");
        return false;
    }
    return loadFromString(profileManager, keyboard, content);
}

/**
 * @brief Save current configuration to a file
 * 
 * Serializes the current profile configuration to JSON format and saves it to a file.
 * Works on both embedded targets (using LittleFS) and host systems (using standard files).
 * 
 * @param profileManager Profile manager containing current configuration
 * @param configPath Path where configuration should be saved
 * @return true if saving succeeded, false otherwise
 */
bool ConfigLoader::saveToFile(const ProfileManager& profileManager, const std::string& configPath) {
    // Serialize current configuration to JSON
    DynamicJsonDocument doc(2048);
    JsonArray profiles = doc.createNestedArray("profiles");

    // Save each profile
    for (uint8_t profileIndex = 0; profileIndex < ProfileManager::NUM_PROFILES; profileIndex++) {
        const Profile* profile = profileManager.getProfile(profileIndex);
        if (!profile) continue;

        JsonObject profileObj = profiles.createNestedObject();
        profileObj["name"] = profile->getName().c_str();

        JsonObject buttons = profileObj.createNestedObject("buttons");

        // Save each button action
        for (const char* buttonName : {"A", "B", "C", "D"}) {
            uint8_t buttonIndex = getButtonIndex(buttonName);
            Action* action = profile->getAction(buttonIndex);

            if (action) {
                buttons[buttonName] = actionToJson(action);
            }
        }
    }

    std::string content;
    serializeJson(doc, content);

    if (!fileSystem->writeFile(configPath.c_str(), content)) {
        debugOutput("Failed to write config file");
        return false;
    }

    return true;
}

using namespace ArduinoJson;

/**
 * @brief Load configuration from JSON string
 * 
 * Parses a JSON configuration string and loads it into the profile manager.
 * This is the core configuration loading method that's hardware-independent.
 * 
 * @param profileManager Profile manager to populate
 * @param keyboard BLE keyboard interface for creating send actions
 * @param jsonConfig JSON configuration string to parse
 * @return true if loading succeeded, false if JSON parsing failed
 */
bool ConfigLoader::loadFromString(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error) {
        debugOutput("JSON parsing failed:", error.c_str());
        return false;
    }

    // Clear existing profiles
    for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++) {
        profileManager.addProfile(i, nullptr);
    }

    // Load each profile
    JsonArray profiles = doc["profiles"];
    for (uint8_t profileIndex = 0; profileIndex < profiles.size() && profileIndex < ProfileManager::NUM_PROFILES; profileIndex++) {
        JsonObject profile = profiles[profileIndex];
        String profileName = profile["name"];

        auto newProfile = std::unique_ptr<Profile>(new Profile(profileName.c_str()));

        // Load each button action
        JsonObject buttons = profile["buttons"];
        for (const char* buttonName : {"A", "B", "C", "D"}) {
            if (buttons.containsKey(buttonName)) {
                JsonObject actionJson = buttons[buttonName];
                uint8_t buttonIndex = getButtonIndex(buttonName);

                std::unique_ptr<Action> action = createActionFromJson(actionJson, keyboard);
                if (action) {
                    const char* actionName = actionJson["name"] | "";
                    if (actionName[0] != '\0') {
                        action->setName(actionName);
                    }
                    newProfile->addAction(buttonIndex, std::move(action));
                }
            }
        }

        profileManager.addProfile(profileIndex, std::move(newProfile));
    }

    return true;
}

/**
 * @brief Merge profiles from JSON configuration
 * 
 * Adds profiles from a JSON configuration into the profile manager, preserving existing profiles.
 * New profiles are added to the first available empty slots. Profiles with duplicate names
 * are skipped to prevent overwriting existing configurations.
 * 
 * @param profileManager Profile manager to update
 * @param keyboard BLE keyboard interface for creating send actions
 * @param jsonConfig JSON configuration string containing profiles to merge
 * @return true if merging succeeded, false if JSON parsing failed
 */
bool ConfigLoader::mergeConfig(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error) {
        debugOutput("JSON parsing failed:", error.c_str());
        return false;
    }

    JsonArray profiles = doc["profiles"];

    // Merge each profile - add to first available empty slot
    for (uint8_t newProfileIndex = 0; newProfileIndex < profiles.size() && newProfileIndex < ProfileManager::NUM_PROFILES; newProfileIndex++) {
        JsonObject newProfileJson = profiles[newProfileIndex];
        String profileName = newProfileJson["name"];

        // Check if profile with same name already exists
        bool profileExists = false;
        for (uint8_t existingIndex = 0; existingIndex < ProfileManager::NUM_PROFILES; existingIndex++) {
            const Profile* existingProfile = profileManager.getProfile(existingIndex);
            if (existingProfile && existingProfile->getName() == profileName.c_str()) {
                profileExists = true;
                break;
            }
        }

        if (profileExists) {
            debugOutput("Profile '", profileName.c_str());
            debugOutput("' already exists, skipping", "");
            continue;
        }

        // Find first empty slot
        uint8_t targetIndex = 255;
        for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++) {
            if (!profileManager.getProfile(i)) {
                targetIndex = i;
                break;
            }
        }

        if (targetIndex == 255) {
            debugOutput("No empty profile slots available for merging", "");
            continue;
        }

        // Create and add the new profile
        auto newProfile = std::unique_ptr<Profile>(new Profile(profileName.c_str()));

        // Load each button action
        JsonObject buttons = newProfileJson["buttons"];
        for (const char* buttonName : {"A", "B", "C", "D"}) {
            if (buttons.containsKey(buttonName)) {
                JsonObject actionJson = buttons[buttonName];
                uint8_t buttonIndex = getButtonIndex(buttonName);

                std::unique_ptr<Action> action = createActionFromJson(actionJson, keyboard);
                if (action) {
                    const char* actionName = actionJson["name"] | "";
                    if (actionName[0] != '\0') {
                        action->setName(actionName);
                    }
                    newProfile->addAction(buttonIndex, std::move(action));
                }
            }
        }

        profileManager.addProfile(targetIndex, std::move(newProfile));
        debugOutput("Added profile '", profileName.c_str());
        debugOutput("' to slot ", std::to_string(targetIndex).c_str());
    }

    return true;
}

/**
 * @brief Replace a specific profile in the configuration
 * 
 * Replaces the profile at the specified index with a new profile from JSON configuration.
 * This allows updating individual profiles without affecting others.
 * 
 * @param profileManager Profile manager to update
 * @param keyboard BLE keyboard interface for creating send actions
 * @param profileIndex Index of profile to replace (0-2)
 * @param jsonConfig JSON configuration string containing the replacement profile
 * @return true if replacement succeeded, false if index is invalid or JSON parsing failed
 */
bool ConfigLoader::replaceProfile(ProfileManager& profileManager, IBleKeyboard* keyboard, uint8_t profileIndex, const std::string& jsonConfig) {
    if (profileIndex >= ProfileManager::NUM_PROFILES) {
        debugOutput("Invalid profile index: ", std::to_string(profileIndex).c_str());
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error) {
        debugOutput("JSON parsing failed:", error.c_str());
        return false;
    }

    JsonArray profiles = doc["profiles"];
    if (profiles.size() == 0) {
        debugOutput("No profiles found in JSON config", "");
        return false;
    }

    // Use the first profile in the JSON config
    JsonObject newProfileJson = profiles[0];
    String profileName = newProfileJson["name"];

    auto newProfile = std::unique_ptr<Profile>(new Profile(profileName.c_str()));

    // Load each button action
    JsonObject buttons = newProfileJson["buttons"];
    for (const char* buttonName : {"A", "B", "C", "D"}) {
        if (buttons.containsKey(buttonName)) {
            JsonObject actionJson = buttons[buttonName];
            uint8_t buttonIndex = getButtonIndex(buttonName);

            std::unique_ptr<Action> action = createActionFromJson(actionJson, keyboard);
            if (action) {
                const char* actionName = actionJson["name"] | "";
                if (actionName[0] != '\0') {
                    action->setName(actionName);
                }
                newProfile->addAction(buttonIndex, std::move(action));
            }
        }
    }

    profileManager.addProfile(profileIndex, std::move(newProfile));
    debugOutput("Replaced profile at index ", std::to_string(profileIndex).c_str());
    debugOutput(" with '", profileName.c_str());
    debugOutput("'", "");

    return true;
}

// Existing helper methods remain unchanged
uint8_t ConfigLoader::getButtonIndex(const char* buttonName) {
    if (strcmp(buttonName, "A") == 0) return Button::A;
    if (strcmp(buttonName, "B") == 0) return Button::B;
    if (strcmp(buttonName, "C") == 0) return Button::C;
    if (strcmp(buttonName, "D") == 0) return Button::D;
    return 255; // Invalid
}

std::unique_ptr<Action> ConfigLoader::createActionFromJson(const ArduinoJson::JsonObject& actionJson, IBleKeyboard* keyboard) {
    const char* type = actionJson["type"];

    if (strcmp(type, "SendStringAction") == 0) {
        const char* value = actionJson["value"];
        return std::unique_ptr<Action>(new SendStringAction(keyboard, value));
    }
    else if (strcmp(type, "SendMediaKeyAction") == 0) {
        const char* value = actionJson["value"];
        if (strcmp(value, "MEDIA_STOP") == 0) {
            return std::unique_ptr<Action>(new SendMediaKeyAction(keyboard, KEY_MEDIA_STOP));
        }
    }
    else if (strcmp(type, "SendCharAction") == 0) {
        const char* value = actionJson["value"];
        if (strcmp(value, "LEFT_ARROW") == 0) {
            return std::unique_ptr<Action>(new SendCharAction(keyboard, KEY_LEFT_ARROW));
        }
        else if (strcmp(value, "RIGHT_ARROW") == 0) {
            return std::unique_ptr<Action>(new SendCharAction(keyboard, KEY_RIGHT_ARROW));
        }
    }
    else if (strcmp(type, "SendKeyAction") == 0) {
        const char* value = actionJson["value"];
        if (strcmp(value, "UP_ARROW") == 0) {
            return std::unique_ptr<Action>(new SendKeyAction(keyboard, KEY_UP_ARROW));
        }
        else if (strcmp(value, "DOWN_ARROW") == 0) {
            return std::unique_ptr<Action>(new SendKeyAction(keyboard, KEY_DOWN_ARROW));
        }
    }
    else if (strcmp(type, "SerialOutputAction") == 0) {
        const char* value = actionJson["value"];
        return std::unique_ptr<Action>(new SerialOutputAction(value));
    }
    else if (strcmp(type, "DelayedAction") == 0) {
        uint32_t delayMs = actionJson["delayMs"];
        JsonObject nestedAction = actionJson["action"];
        std::unique_ptr<Action> innerAction = createActionFromJson(nestedAction, keyboard);

        if (innerAction) {
            return std::unique_ptr<Action>(new DelayedAction(std::move(innerAction), delayMs));
        }
    }

    return nullptr;
}

ArduinoJson::JsonObject ConfigLoader::actionToJson(const Action* action) const {
    JsonObject result;
    if (action->hasName()) {
        result["name"] = action->getName().c_str();
    }

    // Use the new virtual methods instead of typeid
    switch (action->getType()) {
        case Action::Type::SendString:
            result["type"] = "SendStringAction";
            action->getJsonProperties(result);
            break;
        case Action::Type::SendChar:
            result["type"] = "SendCharAction";
            action->getJsonProperties(result);
            break;
        case Action::Type::SendKey:
            result["type"] = "SendKeyAction";
            action->getJsonProperties(result);
            break;
        case Action::Type::SendMediaKey:
            result["type"] = "SendMediaKeyAction";
            action->getJsonProperties(result);
            break;
        case Action::Type::SerialOutput:
            result["type"] = "SerialOutputAction";
            action->getJsonProperties(result);
            break;
        case Action::Type::Delayed:
            result["type"] = "DelayedAction";
            action->getJsonProperties(result);
            break;
        default:
            result["type"] = "UnknownAction";
            break;
    }

    return result;
}