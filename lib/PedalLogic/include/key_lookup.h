#pragma once

#include "action.h"
#include "i_ble_keyboard.h"
#include <cstdint>

/**
 * @brief Looks up an action type by name
 *
 * @param name The action type name to look up (e.g., "SendStringAction")
 * @return The corresponding Action::Type enum value, or Action::Type::Unknown if not found
 */
Action::Type lookupActionType(const char* name);

/**
 * @brief Looks up a key code by name
 *
 * @param name The key name to look up (e.g., "LEFT_ARROW")
 * @return The corresponding USB HID key code, or 0 if not found
 */
uint8_t lookupKey(const char* name);

/**
 * @brief Looks up a media key report by name
 *
 * @param name The media key name to look up (e.g., "MEDIA_STOP")
 * @return Pointer to the USB HID report for the media key, or nullptr if not found
 */
const uint8_t* lookupMediaKey(const char* name);

/**
 * @brief Reverse lookup: USB HID key code → canonical key name.
 *
 * First match wins; the table is authored canonical-name-first, so this
 * returns the KEY_-prefixed name when both that and a legacy alias exist.
 *
 * @param code The USB HID key code to look up
 * @return The canonical key name (e.g. "KEY_LEFT_ARROW"), or nullptr if not found
 */
const char* lookupKeyName(uint8_t code);

/**
 * @brief Reverse lookup: 2-byte media key report → canonical media key name.
 *
 * @param report Pointer to a 2-byte HID media key report (must not be null)
 * @return The canonical media key name (e.g. "MEDIA_STOP"), or nullptr if not found
 */
const char* lookupMediaKeyName(const uint8_t* report);
