#include "profile_manager.h"
#include "delayed_action.h"

ProfileManager::ProfileManager(ILEDController& led1, ILEDController& led2, ILEDController& led3)
    : led1(led1), led2(led2), led3(led3)
{
    updateLEDs();
}

void ProfileManager::addProfile(uint8_t profileIndex, std::unique_ptr<Profile> profile)
{
    if (profileIndex < NUM_PROFILES)
    {
        profileSlots[profileIndex] = std::move(profile);
    }
}

Action* ProfileManager::getAction(uint8_t profileIndex, uint8_t button) const
{
    if (profileIndex < NUM_PROFILES && profileSlots[profileIndex])
    {
        return profileSlots[profileIndex]->getAction(button);
    }
    return nullptr;
}

const Profile* ProfileManager::getProfile(uint8_t profileIndex) const
{
    if (profileIndex < NUM_PROFILES && profileSlots[profileIndex])
    {
        return profileSlots[profileIndex].get();
    }
    return nullptr;
}

uint8_t ProfileManager::switchProfile()
{
    // Advance to the next populated slot, skipping empty ones, wrapping around
    uint8_t next = currentProfile;
    for (uint8_t i = 0; i < NUM_PROFILES; i++)
    {
        next = (next + 1) % NUM_PROFILES;
        if (profileSlots[next])
            break;
    }
    currentProfile = next;
    updateLEDs();

    // Trigger post-switch blink feedback
    postSwitchBlink = true;
    blinkPhase = 0;
    blinkStartTime = 0; // will be initialised on first update() call

    return currentProfile;
}

/**
 * Binary LED encoding:
 *   bits = profileIndex + 1
 *   LED1 = bit 0, LED2 = bit 1, LED3 = bit 2
 *
 * This gives the familiar single-LED behaviour for profiles 0-2:
 *   profile 0 → bits=1 → LED1 on
 *   profile 1 → bits=2 → LED2 on
 *   profile 2 → bits=3 → LED1+LED2 on  ← departure from old behaviour
 *
 * Wait — to keep profiles 0-2 identical to the old single-LED behaviour we
 * shift the encoding so profiles 0/1/2 map to 001/010/100:
 *   profile 0 → LED1 on,  LED2 off, LED3 off
 *   profile 1 → LED1 off, LED2 on,  LED3 off
 *   profile 2 → LED1 off, LED2 off, LED3 on
 *   profile 3 → LED1 on,  LED2 on,  LED3 off
 *   profile 4 → LED1 on,  LED2 off, LED3 on
 *   profile 5 → LED1 off, LED2 on,  LED3 on
 *   profile 6 → LED1 on,  LED2 on,  LED3 on
 */
void ProfileManager::updateLEDs()
{
    if (postSwitchBlink)
        return; // LEDs managed by update() during blink

    // Map profile index to a 3-bit pattern
    // profiles 0-2: single LED (backward-compatible)
    // profiles 3-6: binary of (profileIndex - 2), i.e. 1..4 → 001..100 skipped,
    //               use profileIndex+1 with the bit-shift trick below
    static constexpr uint8_t LED_PATTERN[NUM_PROFILES] = {
        0b001, // profile 0 → LED1
        0b010, // profile 1 → LED2
        0b100, // profile 2 → LED3
        0b011, // profile 3 → LED1+LED2
        0b101, // profile 4 → LED1+LED3
        0b110, // profile 5 → LED2+LED3
        0b111, // profile 6 → LED1+LED2+LED3
    };

    uint8_t bits = LED_PATTERN[currentProfile];
    led1.setState((bits & 0b001) != 0);
    led2.setState((bits & 0b010) != 0);
    led3.setState((bits & 0b100) != 0);
}

void ProfileManager::update(uint32_t now)
{
    if (! postSwitchBlink)
        return;

    // Initialise blink start time on first call after switchProfile()
    if (blinkStartTime == 0)
    {
        blinkStartTime = now;
        // Start: all LEDs on
        led1.setState(true);
        led2.setState(true);
        led3.setState(true);
        return;
    }

    uint32_t elapsed = now - blinkStartTime;
    uint8_t phase = static_cast<uint8_t>(elapsed / BLINK_INTERVAL);

    if (phase == blinkPhase)
        return; // nothing to do yet
    blinkPhase = phase;

    uint8_t totalHalfCycles = BLINK_COUNT * 2; // 3 blinks = 6 half-cycles
    if (phase >= totalHalfCycles)
    {
        postSwitchBlink = false;
        updateLEDs(); // restore profile encoding
        return;
    }

    // Toggle all LEDs together
    bool on = (phase % 2 == 0);
    led1.setState(on);
    led2.setState(on);
    led3.setState(on);
}

void ProfileManager::resetToFirstProfile()
{
    currentProfile = 0;
    for (uint8_t i = 0; i < NUM_PROFILES; i++)
    {
        if (profileSlots[i])
        {
            currentProfile = i;
            break;
        }
    }
    postSwitchBlink = false;
    updateLEDs();
}

const std::string& ProfileManager::getProfileName(uint8_t profileIndex) const
{
    if (profileIndex < NUM_PROFILES && profileSlots[profileIndex])
    {
        return profileSlots[profileIndex]->getName();
    }
    static const std::string emptyString;
    return emptyString;
}

bool ProfileManager::hasActiveDelayedAction() const
{
    for (const auto& slot : profileSlots)
    {
        if (! slot)
            continue;
        for (uint8_t b = 0; b < Profile::NUM_BUTTONS; b++)
        {
            const Action* action = slot->getAction(b);
            if (action && action->isInProgress())
                return true;
        }
    }
    return false;
}

const char* ProfileManager::getActionTypeString(Action::Type actionType)
{
    switch (actionType)
    {
        case Action::Type::SendString:
            return "SendString";
        case Action::Type::SendChar:
            return "SendChar";
        case Action::Type::SendKey:
            return "SendKey";
        case Action::Type::SendMediaKey:
            return "SendMediaKey";
        case Action::Type::SerialOutput:
            return "SerialOutput";
        case Action::Type::Delayed:
            return "Delayed";
        default:
            return "Unknown";
    }
}
