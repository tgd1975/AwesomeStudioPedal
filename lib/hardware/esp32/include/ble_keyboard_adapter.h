#pragma once
// clang-format off
// BleKeyboard.h must precede i_ble_keyboard.h: defines ESP32_BLE_KEYBOARD_H guard
// that prevents duplicate key constant definitions in i_ble_keyboard.h.
#include <BleKeyboard.h>
#include "i_ble_keyboard.h"
// clang-format on

/**
 * @class BleKeyboardAdapter
 * @brief Adapter that implements IBleKeyboard interface using BleKeyboard
 *
 * Concrete implementation that wraps the BleKeyboard library to provide
 * the IBleKeyboard interface. This allows the pedal logic to work with
 * the actual BLE keyboard hardware.
 */
class BleKeyboardAdapter : public IBleKeyboard
{
    BleKeyboard& kb; /**< Reference to the underlying BleKeyboard instance */
public:
    /**
     * @brief Constructs a BleKeyboardAdapter
     *
     * @param kb Reference to the BleKeyboard instance to wrap
     */
    explicit BleKeyboardAdapter(BleKeyboard& kb) : kb(kb) {}

    /**
     * @brief Initializes the BLE keyboard
     *
     * Delegates to the underlying BleKeyboard begin() method.
     */
    void begin() override { kb.begin(); }

    /**
     * @brief Checks if BLE keyboard is connected
     *
     * @return true if connected, false otherwise
     */
    bool isConnected() override { return kb.isConnected(); }

    /**
     * @brief Sends a single key press
     *
     * @param key USB HID key code to send
     */
    void write(uint8_t key) override { kb.write(key); }

    /**
     * @brief Sends a media key report
     *
     * @param key Media key report to send
     */
    void write(const MediaKeyReport key) override { kb.write(key); }

    /**
     * @brief Sends a text string
     *
     * @param text String to send as keyboard input
     */
    void print(const char* text) override { kb.print(text); }
};

/**
 * @brief Creates the platform-specific BleKeyboardAdapter instance
 *
 * Owns the underlying BleKeyboard object. Caller takes ownership of the
 * returned pointer.
 */
BleKeyboardAdapter* createBleKeyboardAdapter();
