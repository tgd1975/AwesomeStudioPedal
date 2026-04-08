#pragma once
#include <cstdint>

/**
 * @brief Type alias for media key reports
 *
 * Represents a 2-byte media key report for BLE keyboard
 */
using MediaKeyReport = uint8_t[2];

// Key constants: define only when BleKeyboard.h has not been included,
// so that firmware TUs that include both headers don't get redefinitions.
#ifndef ESP32_BLE_KEYBOARD_H
const uint8_t KEY_UP_ARROW = 0xDA;    /**< USB HID key code for up arrow */
const uint8_t KEY_DOWN_ARROW = 0xD9;  /**< USB HID key code for down arrow */
const uint8_t KEY_LEFT_ARROW = 0xD8;  /**< USB HID key code for left arrow */
const uint8_t KEY_RIGHT_ARROW = 0xD7; /**< USB HID key code for right arrow */

static constexpr uint8_t KEY_MEDIA_STOP[2] = {4, 0}; /**< Media key report for stop */
#endif

/**
 * @class IBleKeyboard
 * @brief Interface for Bluetooth LE keyboard functionality
 *
 * Abstract interface that defines the contract for BLE keyboard operations.
 * Allows for dependency injection and mocking in tests.
 */
class IBleKeyboard
{
public:
    virtual ~IBleKeyboard() = default;

    /**
     * @brief Initializes the BLE keyboard
     *
     * Starts BLE advertising and sets up the keyboard service.
     */
    virtual void begin() = 0;

    /**
     * @brief Checks if BLE keyboard is connected to a host
     *
     * @return true if connected, false otherwise
     */
    virtual bool isConnected() = 0;

    /**
     * @brief Sends a single key press
     *
     * @param key USB HID key code to send
     */
    virtual void write(uint8_t key) = 0;

    /**
     * @brief Sends a media key report
     *
     * @param key Media key report (2-byte array)
     */
    virtual void write(const MediaKeyReport key) = 0;

    /**
     * @brief Sends a text string
     *
     * @param text Null-terminated string to send as keyboard input
     */
    virtual void print(const char* text) = 0;
};
