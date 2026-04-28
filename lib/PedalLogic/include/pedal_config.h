#pragma once
#include "i_ble_keyboard.h"
#include "i_logger.h"
#include "profile_manager.h"
#include <string>

/**
 * @brief Loads pedal profiles from file, falling back to DEFAULT_CONFIG on failure.
 *
 * @return true  if configuration was loaded successfully (from file or default JSON)
 * @return false if even the default JSON failed to parse (should never happen)
 */
bool configureProfiles(ProfileManager& profileManager, IBleKeyboard* keyboard);

/**
 * @brief Loads hardware config from /config.json (LittleFS) if present.
 *
 * Overrides fields in the global hardwareConfig with values from the file.
 * Returns false and applies NO overrides if:
 *   - the file's "hardware" field is missing
 *   - the file's "hardware" field does not match hardwareConfig.hardware
 *     (i.e. the config targets a different board than this firmware)
 * Falls back silently to compiled-in defaults if the file is absent or unparseable.
 *
 * @return true  if the file was absent, unparseable, or loaded successfully
 * @return false if a hardware mismatch was detected — caller must halt activation
 */
bool loadHardwareConfig();

/**
 * @brief Core hardware-config parsing logic — injectable for testing.
 *
 * Parses @p jsonContent, validates the "hardware" field against
 * hardwareConfig.hardware, and applies overrides if valid.
 *
 * @param content      Raw JSON string (from config.json)
 * @param logger       Logger instance
 * @return true  if the config was applied (or skipped for a non-fatal reason)
 * @return false if a hardware mismatch was detected
 */
bool loadHardwareConfigFromJson(const std::string& jsonContent, ILogger* logger);
