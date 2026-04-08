#pragma once
#include "i_ble_keyboard.h"
#include <bluefruit.h>

/**
 * @class BleKeyboardAdapter
 * @brief nRF52840-specific implementation of IBleKeyboard interface
 *
 * Wraps the Adafruit Bluefruit BLE HID keyboard (BLEHidAdafruit) to provide
 * the IBleKeyboard interface used by the pedal logic layer.
 */
class BleKeyboardAdapter : public IBleKeyboard {
public:
    /**
     * @brief Initializes the BLE stack and HID keyboard service
     */
    void begin() override;

    /**
     * @brief Checks if a central device is connected
     * @return true if connected
     */
    bool isConnected() override;

    /**
     * @brief Sends a single key press
     * @param key USB HID key code
     */
    void write(uint8_t key) override;

    /**
     * @brief Sends a media key report
     * @param key Media key report to send
     */
    void write(const MediaKeyReport key) override;

    /**
     * @brief Sends a text string
     * @param text String to send as keyboard input
     */
    void print(const char* text) override;

private:
    BLEHidAdafruit hid; /**< Adafruit BLE HID service instance */
};

/**
 * @brief Creates the platform-specific BleKeyboardAdapter instance
 *
 * Caller takes ownership of the returned pointer.
 */
BleKeyboardAdapter* createBleKeyboardAdapter();
