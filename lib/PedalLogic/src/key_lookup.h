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
