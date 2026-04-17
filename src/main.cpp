#include <Arduino.h>
#ifdef ESP32
#include <Preferences.h>
#endif
#include <memory>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "FunctionalInterrupt.h"

#include "ble_keyboard_adapter.h"
#include "platform.h"

#include "button.h"
#include "button_constants.h"
#include "config.h"
#include "config_loader.h"
#include "delayed_action.h"
#include "event_dispatcher.h"
#include "led_controller.h"
#include "pedal_config.h"
#include "profile_manager.h"
#include "send_action.h"
#include "version.h"

/**
 * @file main.cpp
 * @brief Main application entry point for Awesome Guitar Pedal
 *
 * This file contains the setup, loop, and core application logic
 * for the ESP32-based guitar pedal controller.
 */

BleKeyboardAdapter* bleKeyboardAdapter = createBleKeyboardAdapter();

#ifdef ESP32
static constexpr const char* NVS_NAMESPACE = "pedal";
static constexpr const char* NVS_KEY_PROFILE = "profile";

static void saveCurrentProfile(uint8_t index)
{
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, false);
    prefs.putUChar(NVS_KEY_PROFILE, index);
    prefs.end();
}

static uint8_t loadSavedProfile()
{
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, true);
    uint8_t index = prefs.getUChar(NVS_KEY_PROFILE, 0);
    prefs.end();
    return index;
}
#else
// Profile persistence not yet implemented for this platform
static void saveCurrentProfile(uint8_t) {}
static uint8_t loadSavedProfile() { return 0; }
#endif

bool connected = false;

LEDController ledBluetooth(hardwareConfig.ledBluetooth);
LEDController ledPower(hardwareConfig.ledPower);

// Profile-select LEDs — one entry per hardwareConfig.numSelectLeds
LEDController* selectLedObjects[ProfileManager::MAX_SELECT_LEDS] = {};
std::vector<ILEDController*> selectLeds;

// Action buttons — one entry per hardwareConfig.numButtons
Button* actionButtonObjects[Btn::MAX] = {};

ProfileManager* profileManager = nullptr;
EventDispatcher eventDispatcher;

/**
 * @brief Signal a configuration load error by blinking all LEDs, then
 *        fall back to the hardcoded factory default via configureProfiles().
 */
void signalLoadError()
{
    Serial.println("CONFIG ERROR: falling back to factory default");
    constexpr int BLINK_COUNT = 5;
    constexpr int BLINK_DURATION = 100;
    for (int i = 0; i < BLINK_COUNT; i++)
    {
        ledPower.setState(true);
        ledBluetooth.setState(true);
        for (auto* led : selectLeds)
            led->setState(true);
        delay(BLINK_DURATION);
        ledPower.setState(false);
        ledBluetooth.setState(false);
        for (auto* led : selectLeds)
            led->setState(false);
        delay(BLINK_DURATION);
    }
    ledPower.setState(true);
    configureProfiles(*profileManager, bleKeyboardAdapter);
}

void attachInterrupts();

void IRAM_ATTR isr_buttons(uint8_t index)
{
    if (actionButtonObjects[index])
        actionButtonObjects[index]->isr();
}

Button BUTTON_SELECT(hardwareConfig.buttonSelect);

// Individual ISR stubs for up to 26 buttons — only used entries are attached
void IRAM_ATTR isr_btn0() { isr_buttons(0); }
void IRAM_ATTR isr_btn1() { isr_buttons(1); }
void IRAM_ATTR isr_btn2() { isr_buttons(2); }
void IRAM_ATTR isr_btn3() { isr_buttons(3); }
void IRAM_ATTR isr_btn4() { isr_buttons(4); }
void IRAM_ATTR isr_btn5() { isr_buttons(5); }
void IRAM_ATTR isr_btn6() { isr_buttons(6); }
void IRAM_ATTR isr_btn7() { isr_buttons(7); }
void IRAM_ATTR isr_btn8() { isr_buttons(8); }
void IRAM_ATTR isr_btn9() { isr_buttons(9); }
void IRAM_ATTR isr_btn10() { isr_buttons(10); }
void IRAM_ATTR isr_btn11() { isr_buttons(11); }
void IRAM_ATTR isr_btn12() { isr_buttons(12); }
void IRAM_ATTR isr_btn13() { isr_buttons(13); }
void IRAM_ATTR isr_btn14() { isr_buttons(14); }
void IRAM_ATTR isr_btn15() { isr_buttons(15); }
void IRAM_ATTR isr_btn16() { isr_buttons(16); }
void IRAM_ATTR isr_btn17() { isr_buttons(17); }
void IRAM_ATTR isr_btn18() { isr_buttons(18); }
void IRAM_ATTR isr_btn19() { isr_buttons(19); }
void IRAM_ATTR isr_btn20() { isr_buttons(20); }
void IRAM_ATTR isr_btn21() { isr_buttons(21); }
void IRAM_ATTR isr_btn22() { isr_buttons(22); }
void IRAM_ATTR isr_btn23() { isr_buttons(23); }
void IRAM_ATTR isr_btn24() { isr_buttons(24); }
void IRAM_ATTR isr_btn25() { isr_buttons(25); }

void IRAM_ATTR isr_select() { BUTTON_SELECT.isr(); }

using IsrFunc = void (*)();
static const IsrFunc BTN_ISRS[Btn::MAX] = {
    isr_btn0,  isr_btn1,  isr_btn2,  isr_btn3,  isr_btn4,  isr_btn5,  isr_btn6,
    isr_btn7,  isr_btn8,  isr_btn9,  isr_btn10, isr_btn11, isr_btn12, isr_btn13,
    isr_btn14, isr_btn15, isr_btn16, isr_btn17, isr_btn18, isr_btn19, isr_btn20,
    isr_btn21, isr_btn22, isr_btn23, isr_btn24, isr_btn25,
};

/**
 * @brief Initializes all hardware components
 */
void setup_hardware()
{
    ledPower.setup(1);
    ledBluetooth.setup(0);

    for (uint8_t i = 0; i < hardwareConfig.numSelectLeds; i++)
    {
        selectLedObjects[i] = new LEDController(hardwareConfig.ledSelect[i]);
        selectLedObjects[i]->setup(i == 0 ? 1 : 0);
        selectLeds.push_back(selectLedObjects[i]);
    }

    for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
    {
        actionButtonObjects[i] = new Button(hardwareConfig.buttonPins[i]);
        actionButtonObjects[i]->setup();
    }

    BUTTON_SELECT.setup();
}

/**
 * @brief Executes an action with proper logging
 */
void executeActionWithLogging(uint8_t buttonIndex)
{
    char btnLabel[2];
    Btn::name(buttonIndex, btnLabel);

    uint8_t profileIndex = profileManager->getCurrentProfile();
    const char* profileName = profileManager->getProfileName(profileIndex).c_str();
    Serial.printf("Button %s pressed (Profile: %s)\n", btnLabel, profileName);

    if (auto action = profileManager->getAction(profileIndex, buttonIndex))
    {
        if (action->isInProgress())
        {
            Serial.println("  -> DelayedAction in progress, ignoring");
            return;
        }

        const char* actionType = ProfileManager::getActionTypeString(action->getType());
        if (action->hasName())
        {
            Serial.printf("  -> Executing %s action [%s]\n", actionType, action->getName().c_str());
        }
        else
        {
            Serial.printf("  -> Executing %s action\n", actionType);
        }
        action->execute();
    }
    else
    {
        Serial.println("  -> no action configured");
    }
}

/**
 * @brief Configures event handlers for all buttons
 */
void setup_event_handlers()
{
    for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
    {
        uint8_t idx = i;
        eventDispatcher.registerHandler(idx, [idx]() { executeActionWithLogging(idx); });

        // Release handler: call executeRelease() on the current profile's action (no-op for
        // all non-pin actions; drives pin back to idle for PinHighWhilePressed /
        // PinLowWhilePressed).
        eventDispatcher.registerReleaseHandler(
            idx,
            [idx]()
            {
                uint8_t profile = profileManager->getCurrentProfile();
                if (auto action = profileManager->getAction(profile, idx))
                {
                    action->executeRelease();
                }
            });
    }

    // SELECT button is registered at index numButtons (press only — no release handler)
    uint8_t selectHandlerIdx = hardwareConfig.numButtons;
    eventDispatcher.registerHandler(selectHandlerIdx,
                                    []()
                                    {
                                        uint8_t profile = profileManager->switchProfile();
                                        Serial.printf("Switched to Profile %d\n", profile + 1);
                                        saveCurrentProfile(profile);
                                    });
}

/**
 * @brief Arduino setup function - runs once at startup
 */
void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println("AwesomeStudioPedal " FIRMWARE_VERSION " started");

    setup_hardware();

    profileManager = new ProfileManager(selectLeds);
    setup_event_handlers();

    bleKeyboardAdapter->begin();

    if (! configureProfiles(*profileManager, bleKeyboardAdapter))
    {
        signalLoadError();
    }

    // Restore last-used profile from NVS
    uint8_t savedProfile = loadSavedProfile();
    if (savedProfile != profileManager->getCurrentProfile() &&
        profileManager->getProfile(savedProfile) != nullptr)
    {
        profileManager->setCurrentProfile(savedProfile);
        Serial.printf("Restored profile %d from NVS\n", savedProfile + 1);
    }

    attachInterrupts();
}

/**
 * @brief Attaches interrupt handlers to all buttons
 */
void attachInterrupts()
{
    for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
    {
        actionButtonObjects[i]->reset();
        attachInterrupt(hardwareConfig.buttonPins[i], BTN_ISRS[i], CHANGE);
    }
    BUTTON_SELECT.reset();
    attachInterrupt(hardwareConfig.buttonSelect, isr_select, CHANGE);
}

/**
 * @brief Detaches interrupt handlers from all buttons
 */
void detachInterrupts()
{
    for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
    {
        actionButtonObjects[i]->reset();
        detachInterrupt(hardwareConfig.buttonPins[i]);
    }
    BUTTON_SELECT.reset();
    detachInterrupt(hardwareConfig.buttonSelect);
}

/**
 * @brief Processes button events and dispatches them to handlers
 */
void process_events()
{
    for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
    {
        if (! actionButtonObjects[i])
        {
            continue;
        }
        if (actionButtonObjects[i]->event())
        {
            eventDispatcher.dispatch(i);
        }
        if (actionButtonObjects[i]->releaseEvent())
        {
            eventDispatcher.dispatchRelease(i);
        }
    }

    if (BUTTON_SELECT.event())
    {
        eventDispatcher.dispatch(hardwareConfig.numButtons);
    }
}

/**
 * @brief Arduino main loop - runs repeatedly
 */
void loop()
{
    if (bleKeyboardAdapter->isConnected())
    {
        if (! connected)
        {
            Serial.println("connected");
            attachInterrupts();
            ledBluetooth.setState(1);
            connected = true;
        }
    }
    else
    {
        if (connected)
        {
            Serial.println("disconnected");
            connected = false;
            detachInterrupts();
            ledBluetooth.setState(0);
        }
    }
    process_events();

    uint32_t now = millis();

    // Poll in-progress DelayedActions for the current profile.
    // DelayedAction::execute() checks elapsed time internally and fires exactly once.
    {
        uint8_t profile = profileManager->getCurrentProfile();
        for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
        {
            Action* action = profileManager->getAction(profile, i);
            if (action && action->isInProgress())
            {
                action->execute();
            }
        }
    }

    ledPower.update(now);
    ledBluetooth.update(now);
    for (auto* led : selectLeds)
        led->update(now);

    profileManager->update(now);

    if (profileManager->hasActiveDelayedAction())
    {
        if (! ledPower.isBlinking())
        {
            ledPower.startBlink(500);
        }
    }
    else
    {
        if (ledPower.isBlinking())
        {
            ledPower.stopBlink();
            ledPower.setState(true);
        }
    }

    delay(10);
}
