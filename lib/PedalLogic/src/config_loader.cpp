#include "config_loader.h"
#include "send.h"
#include "serial_action.h"
#include "non_send_action.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <cstring>
#include <typeinfo>

// Default configuration (same as the JSON file we created)
const char* ConfigLoader::DEFAULT_CONFIG = 
    "{\n"
    "  \"profiles\": [\n"
    "    {\n"
    "      \"name\": \"Navigation\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"value\": \" \"},\n"
    "        \"B\": {\"type\": \"SendMediaKeyAction\", \"value\": \"MEDIA_STOP\"},\n"
    "        \"C\": {\"type\": \"SendCharAction\", \"value\": \"LEFT_ARROW\"},\n"
    "        \"D\": {\"type\": \"SendCharAction\", \"value\": \"RIGHT_ARROW\"}\n"
    "      }\n"
    "    },\n"
    "    {\n"
    "      \"name\": \"Messaging\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"value\": \"Hello\"},\n"
    "        \"B\": {\"type\": \"SendStringAction\", \"value\": \"World\"},\n"
    "        \"C\": {\"type\": \"SendKeyAction\", \"value\": \"UP_ARROW\"},\n"
    "        \"D\": {\"type\": \"SendKeyAction\", \"value\": \"DOWN_ARROW\"}\n"
    "      }\n"
    "    },\n"
    "    {\n"
    "      \"name\": \"Custom\",\n"
    "      \"buttons\": {\n"
    "        \"A\": {\"type\": \"SendStringAction\", \"value\": \"Profile 2 A\"},\n"
    "        \"B\": {\"type\": \"SerialOutputAction\", \"value\": \"Button B pressed - Custom Profile\"},\n"
    "        \"C\": {\"type\": \"DelayedAction\", \"delayMs\": 5000, \"action\": {\"type\": \"SendStringAction\", \"value\": \"Profile 2 C\"}},\n"
    "        \"D\": {\"type\": \"SendStringAction\", \"value\": \"Profile 2 D\"}\n"
    "      }\n"
    "    }\n"
    "  ]\n"
    "}";

bool ConfigLoader::loadFromFile(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& configPath) {
    if (!LittleFS.begin(true)) {
        Serial.println("Failed to mount LittleFS file system");
        return false;
    }
    
    File configFile = LittleFS.open(configPath.c_str(), "r");
    if (!configFile) {
        Serial.println("Failed to open config file");
        return false;
    }
    
    String configContent = configFile.readString();
    configFile.close();
    
    return loadFromString(profileManager, keyboard, configContent.c_str());
}

bool ConfigLoader::loadFromString(ProfileManager& profileManager, IBleKeyboard* keyboard, const std::string& jsonConfig) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonConfig);
    
    if (error) {
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
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
                    newProfile->addAction(buttonIndex, std::move(action));
                }
            }
        }
        
        profileManager.addProfile(profileIndex, std::move(newProfile));
    }
    
    return true;
}

bool ConfigLoader::saveToFile(const ProfileManager& profileManager, const std::string& configPath) {
    if (!LittleFS.begin(true)) {
        Serial.println("Failed to mount LittleFS file system");
        return false;
    }
    
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
    
    File configFile = LittleFS.open(configPath.c_str(), "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }
    
    serializeJsonPretty(doc, configFile);
    configFile.close();
    
    return true;
}



uint8_t ConfigLoader::getButtonIndex(const char* buttonName) {
    if (strcmp(buttonName, "A") == 0) return Button::A;
    if (strcmp(buttonName, "B") == 0) return Button::B;
    if (strcmp(buttonName, "C") == 0) return Button::C;
    if (strcmp(buttonName, "D") == 0) return Button::D;
    return 255; // Invalid
}

std::unique_ptr<Action> ConfigLoader::createActionFromJson(const JsonObject& actionJson, IBleKeyboard* keyboard) {
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

JsonObject ConfigLoader::actionToJson(const Action* action) const {
    JsonObject result;
    
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