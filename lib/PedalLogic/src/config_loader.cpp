#include "config_loader.h"
#include "file_system.h"
#include "i_logger.h"
#include "send_action.h"
#include "serial_action.h"
#include "non_send_action.h"
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
ILogger*     createLogger();

// Default configuration
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

// ---- Constructors ----

ConfigLoader::ConfigLoader()
    : fileSystem_(createFileSystem()), logger_(createLogger()) {}

ConfigLoader::ConfigLoader(IFileSystem* fs, ILogger* logger)
    : fileSystem_(fs), logger_(logger) {}

// ---- Key/action lookup tables ----

namespace {

struct KeyEntry      { const char* name; uint8_t code; };
struct MediaKeyEntry { const char* name; const uint8_t* report; };
struct ActionTypeEntry { const char* name; Action::Type type; };

static const KeyEntry KEY_TABLE[] = {
    { "LEFT_ARROW",  KEY_LEFT_ARROW  },
    { "RIGHT_ARROW", KEY_RIGHT_ARROW },
    { "UP_ARROW",    KEY_UP_ARROW    },
    { "DOWN_ARROW",  KEY_DOWN_ARROW  },
};

static const MediaKeyEntry MEDIA_KEY_TABLE[] = {
    { "MEDIA_STOP", KEY_MEDIA_STOP },
};

static const ActionTypeEntry ACTION_TYPE_TABLE[] = {
    { "SendStringAction",   Action::Type::SendString   },
    { "SendCharAction",     Action::Type::SendChar     },
    { "SendKeyAction",      Action::Type::SendKey      },
    { "SendMediaKeyAction", Action::Type::SendMediaKey },
    { "SerialOutputAction", Action::Type::SerialOutput },
    { "DelayedAction",      Action::Type::Delayed      },
};

Action::Type lookupActionType(const char* name) {
    for (const auto& e : ACTION_TYPE_TABLE) {
        if (strcmp(e.name, name) == 0) return e.type;
    }
    return Action::Type::Unknown;
}

uint8_t lookupKey(const char* name) {
    for (const auto& e : KEY_TABLE) {
        if (strcmp(e.name, name) == 0) return e.code;
    }
    return 0;
}

const uint8_t* lookupMediaKey(const char* name) {
    for (const auto& e : MEDIA_KEY_TABLE) {
        if (strcmp(e.name, name) == 0) return e.report;
    }
    return nullptr;
}

} // namespace

// ---- Public API ----

bool ConfigLoader::loadFromFile(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& configPath) {
    std::string content;
    if (!fileSystem_->readFile(configPath.c_str(), content)) {
        logger_->log("Failed to read config file");
        return false;
    }
    return loadFromString(profileManager, keyboard, content);
}

bool ConfigLoader::saveToFile(const ProfileManager& profileManager, const std::string& configPath) {
    DynamicJsonDocument doc(2048);
    JsonArray profiles = doc.createNestedArray("profiles");

    for (uint8_t profileIndex = 0; profileIndex < ProfileManager::NUM_PROFILES; profileIndex++) {
        const Profile* profile = profileManager.getProfile(profileIndex);
        if (!profile) continue;

        JsonObject profileObj = profiles.createNestedObject();
        profileObj["name"] = profile->getName().c_str();

        JsonObject buttons = profileObj.createNestedObject("buttons");

        for (const char* buttonName : {"A", "B", "C", "D"}) {
            uint8_t buttonIndex = getButtonIndex(buttonName);
            Action* action = profile->getAction(buttonIndex);
            if (action) {
                JsonObject actionObj = buttons.createNestedObject(buttonName);
                actionToJson(action, actionObj);
            }
        }
    }

    std::string content;
    serializeJson(doc, content);

    if (!fileSystem_->writeFile(configPath.c_str(), content)) {
        logger_->log("Failed to write config file");
        return false;
    }

    return true;
}

bool ConfigLoader::loadFromString(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error) {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++) {
        profileManager.addProfile(i, nullptr);
    }

    JsonArray profiles = doc["profiles"];
    for (uint8_t i = 0; i < profiles.size() && i < ProfileManager::NUM_PROFILES; i++) {
        JsonObject profileJson = profiles[i];
        const char* profileName = profileJson["name"] | "";

        auto newProfile = std::unique_ptr<Profile>(new Profile(profileName));
        populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
        profileManager.addProfile(i, std::move(newProfile));
    }

    return true;
}

bool ConfigLoader::mergeConfig(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error) {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    JsonArray profiles = doc["profiles"];

    for (uint8_t newIdx = 0; newIdx < profiles.size() && newIdx < ProfileManager::NUM_PROFILES; newIdx++) {
        JsonObject profileJson = profiles[newIdx];
        const char* profileName = profileJson["name"] | "";

        // Skip if a profile with the same name already exists
        bool exists = false;
        for (uint8_t i = 0; i < ProfileManager::NUM_PROFILES; i++) {
            const Profile* p = profileManager.getProfile(i);
            if (p && p->getName() == profileName) {
                exists = true;
                break;
            }
        }
        if (exists) {
            logger_->log("Profile already exists, skipping: ", profileName);
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
            logger_->log("No empty profile slots available for merging");
            continue;
        }

        auto newProfile = std::unique_ptr<Profile>(new Profile(profileName));
        populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
        profileManager.addProfile(targetIndex, std::move(newProfile));
        logger_->log("Added profile: ", profileName);
    }

    return true;
}

bool ConfigLoader::replaceProfile(ProfileManager& profileManager, IBleKeyboard* keyboard, uint8_t profileIndex, const std::string& jsonConfig) {
    if (profileIndex >= ProfileManager::NUM_PROFILES) {
        logger_->log("Invalid profile index");
        return false;
    }

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);

    if (error) {
        logger_->log("JSON parsing failed:", error.c_str());
        return false;
    }

    JsonArray profiles = doc["profiles"];
    if (profiles.size() == 0) {
        logger_->log("No profiles found in JSON config");
        return false;
    }

    JsonObject profileJson = profiles[0];
    const char* profileName = profileJson["name"] | "";

    auto newProfile = std::unique_ptr<Profile>(new Profile(profileName));
    populateProfileFromJson(*newProfile, profileJson["buttons"], keyboard);
    profileManager.addProfile(profileIndex, std::move(newProfile));
    logger_->log("Replaced profile with: ", profileName);

    return true;
}

// ---- Helper methods ----

uint8_t ConfigLoader::getButtonIndex(const char* buttonName) {
    if (strcmp(buttonName, "A") == 0) return Button::A;
    if (strcmp(buttonName, "B") == 0) return Button::B;
    if (strcmp(buttonName, "C") == 0) return Button::C;
    if (strcmp(buttonName, "D") == 0) return Button::D;
    return 255;
}

void ConfigLoader::populateProfileFromJson(Profile& profile, JsonObject buttons, IBleKeyboard* keyboard) {
    for (const char* buttonName : {"A", "B", "C", "D"}) {
        if (!buttons.containsKey(buttonName)) continue;

        JsonObject actionJson = buttons[buttonName];
        uint8_t buttonIndex = getButtonIndex(buttonName);

        std::unique_ptr<Action> action = createActionFromJson(actionJson, keyboard);
        if (action) {
            const char* actionName = actionJson["name"] | "";
            if (actionName[0] != '\0') {
                action->setName(actionName);
            }
            profile.addAction(buttonIndex, std::move(action));
        }
    }
}

std::unique_ptr<Action> ConfigLoader::createActionFromJson(const JsonObject& actionJson, IBleKeyboard* keyboard) {
    const char* typeName = actionJson["type"] | "";
    Action::Type type = lookupActionType(typeName);

    switch (type) {
        case Action::Type::SendString: {
            const char* value = actionJson["value"] | "";
            return std::unique_ptr<Action>(new SendStringAction(keyboard, value));
        }
        case Action::Type::SendChar: {
            uint8_t code = lookupKey(actionJson["value"] | "");
            if (code != 0)
                return std::unique_ptr<Action>(new SendCharAction(keyboard, code));
            break;
        }
        case Action::Type::SendKey: {
            uint8_t code = lookupKey(actionJson["value"] | "");
            if (code != 0)
                return std::unique_ptr<Action>(new SendKeyAction(keyboard, code));
            break;
        }
        case Action::Type::SendMediaKey: {
            const uint8_t* report = lookupMediaKey(actionJson["value"] | "");
            if (report)
                return std::unique_ptr<Action>(new SendMediaKeyAction(keyboard, report));
            break;
        }
        case Action::Type::SerialOutput: {
            const char* value = actionJson["value"] | "";
            return std::unique_ptr<Action>(new SerialOutputAction(value));
        }
        case Action::Type::Delayed: {
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

void ConfigLoader::actionToJson(const Action* action, JsonObject& out) const {
    if (action->hasName()) {
        out["name"] = action->getName().c_str();
    }

    switch (action->getType()) {
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
