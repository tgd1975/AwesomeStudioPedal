#pragma once
#include <ArduinoJson.h>
#include <cstdint>
#include <memory>
#include <string>

/**
 * @class Action
 * @brief Base class for all pedal actions
 */
class Action
{
public:
    enum class Type : uint8_t
    {
        Unknown,
        SendString,
        SendChar,
        SendKey,
        SendMediaKey,
        SerialOutput,
        Delayed,
        PinHigh,
        PinLow,
        PinToggle,
        PinHighWhilePressed,
        PinLowWhilePressed
    };

    void setName(const std::string& n) { name = n; }
    const std::string& getName() const { return name; }
    bool hasName() const { return ! name.empty(); }

    virtual Type getType() const { return Type::Unknown; }
    virtual void execute() = 0;

    /**
     * @brief Called on button release. No-op by default; pin "while pressed" actions override.
     */
    virtual void executeRelease() {}
    virtual uint32_t getDelay() const { return 0; }
    virtual bool isSendAction() const { return false; }

    /**
     * @brief Returns true if the action is currently in progress (e.g. a running delay)
     */
    virtual bool isInProgress() const { return false; }

    virtual void getJsonProperties(JsonObject& json) const { (void) json; }

    virtual ~Action() = default;

private:
    std::string name;
};
