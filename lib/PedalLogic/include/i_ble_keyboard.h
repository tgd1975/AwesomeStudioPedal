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
// Values are identical to BleKeyboard.h (USB HID codes).
#ifndef ESP32_BLE_KEYBOARD_H
// Arrow keys
const uint8_t KEY_LEFT_ARROW = 0xD8;
const uint8_t KEY_RIGHT_ARROW = 0xD7;
const uint8_t KEY_UP_ARROW = 0xDA;
const uint8_t KEY_DOWN_ARROW = 0xD9;
// Navigation
const uint8_t KEY_PAGE_UP = 0xD3;
const uint8_t KEY_PAGE_DOWN = 0xD6;
const uint8_t KEY_HOME = 0xD2;
const uint8_t KEY_END = 0xD5;
const uint8_t KEY_INSERT = 0xD1;
const uint8_t KEY_DELETE = 0xD4;
// Control
const uint8_t KEY_BACKSPACE = 0xB2;
const uint8_t KEY_TAB = 0xB3;
const uint8_t KEY_RETURN = 0xB0;
const uint8_t KEY_ESC = 0xB1;
const uint8_t KEY_CAPS_LOCK = 0xC1;
const uint8_t KEY_PRTSC = 0xCE;
// Modifiers
const uint8_t KEY_LEFT_CTRL = 0x80;
const uint8_t KEY_LEFT_SHIFT = 0x81;
const uint8_t KEY_LEFT_ALT = 0x82;
const uint8_t KEY_LEFT_GUI = 0x83;
const uint8_t KEY_RIGHT_CTRL = 0x84;
const uint8_t KEY_RIGHT_SHIFT = 0x85;
const uint8_t KEY_RIGHT_ALT = 0x86;
const uint8_t KEY_RIGHT_GUI = 0x87;
// Function keys
const uint8_t KEY_F1 = 0xC2;
const uint8_t KEY_F2 = 0xC3;
const uint8_t KEY_F3 = 0xC4;
const uint8_t KEY_F4 = 0xC5;
const uint8_t KEY_F5 = 0xC6;
const uint8_t KEY_F6 = 0xC7;
const uint8_t KEY_F7 = 0xC8;
const uint8_t KEY_F8 = 0xC9;
const uint8_t KEY_F9 = 0xCA;
const uint8_t KEY_F10 = 0xCB;
const uint8_t KEY_F11 = 0xCC;
const uint8_t KEY_F12 = 0xCD;
const uint8_t KEY_F13 = 0xF0;
const uint8_t KEY_F14 = 0xF1;
const uint8_t KEY_F15 = 0xF2;
const uint8_t KEY_F16 = 0xF3;
const uint8_t KEY_F17 = 0xF4;
const uint8_t KEY_F18 = 0xF5;
const uint8_t KEY_F19 = 0xF6;
const uint8_t KEY_F20 = 0xF7;
const uint8_t KEY_F21 = 0xF8;
const uint8_t KEY_F22 = 0xF9;
const uint8_t KEY_F23 = 0xFA;
const uint8_t KEY_F24 = 0xFB;
// Numpad
const uint8_t KEY_NUM_0 = 0xEA;
const uint8_t KEY_NUM_1 = 0xE1;
const uint8_t KEY_NUM_2 = 0xE2;
const uint8_t KEY_NUM_3 = 0xE3;
const uint8_t KEY_NUM_4 = 0xE4;
const uint8_t KEY_NUM_5 = 0xE5;
const uint8_t KEY_NUM_6 = 0xE6;
const uint8_t KEY_NUM_7 = 0xE7;
const uint8_t KEY_NUM_8 = 0xE8;
const uint8_t KEY_NUM_9 = 0xE9;
const uint8_t KEY_NUM_SLASH = 0xDC;
const uint8_t KEY_NUM_ASTERISK = 0xDD;
const uint8_t KEY_NUM_MINUS = 0xDE;
const uint8_t KEY_NUM_PLUS = 0xDF;
const uint8_t KEY_NUM_ENTER = 0xE0;
const uint8_t KEY_NUM_PERIOD = 0xEB;
// Media key reports
static constexpr uint8_t KEY_MEDIA_NEXT_TRACK[2] = {1, 0};
static constexpr uint8_t KEY_MEDIA_PREVIOUS_TRACK[2] = {2, 0};
static constexpr uint8_t KEY_MEDIA_STOP[2] = {4, 0};
static constexpr uint8_t KEY_MEDIA_PLAY_PAUSE[2] = {8, 0};
static constexpr uint8_t KEY_MEDIA_MUTE[2] = {16, 0};
static constexpr uint8_t KEY_MEDIA_VOLUME_UP[2] = {32, 0};
static constexpr uint8_t KEY_MEDIA_VOLUME_DOWN[2] = {64, 0};
// Extended media keys
static constexpr uint8_t KEY_MEDIA_WWW_HOME[2] = {128, 0};
static constexpr uint8_t KEY_MEDIA_LOCAL_MACHINE_BROWSER[2] = {0, 1};
static constexpr uint8_t KEY_MEDIA_CALCULATOR[2] = {0, 2};
static constexpr uint8_t KEY_MEDIA_WWW_BOOKMARKS[2] = {0, 4};
static constexpr uint8_t KEY_MEDIA_WWW_SEARCH[2] = {0, 8};
static constexpr uint8_t KEY_MEDIA_WWW_STOP[2] = {0, 16};
static constexpr uint8_t KEY_MEDIA_WWW_BACK[2] = {0, 32};
static constexpr uint8_t KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION[2] = {0, 64};
static constexpr uint8_t KEY_MEDIA_EMAIL_READER[2] = {0, 128};
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
