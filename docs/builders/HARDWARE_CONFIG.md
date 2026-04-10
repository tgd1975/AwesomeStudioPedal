# Hardware Configuration

## Pin assignments

GPIO pin assignments and build counts (number of profiles, select LEDs, and action buttons)
are set in `builder_config.h`, one file per hardware target:

```
lib/hardware/esp32/include/builder_config.h     ← edit this for ESP32
lib/hardware/nrf52840/include/builder_config.h  ← edit this for nRF52840
```

Open the file for your board, change the `CONF_*` values to match your wiring, and rebuild.
The file contains plain-English `static_assert` checks — if your configuration is invalid,
the build fails with a descriptive message telling you exactly what to fix.

For the default wiring table, see [BUILD_GUIDE.md](BUILD_GUIDE.md).

## Profile configuration

Button mappings are stored in `data/profiles.json`. The file is deployed to the device's
LittleFS filesystem during upload.

### File structure

```json
{
  "_doc": "Key reference: docs/builders/KEY_REFERENCE.md",
  "profiles": [
    {
      "name": "Profile Name",
      "buttons": {
        "A": { "type": "ActionType", "name": "Button label", "value": "..." },
        "B": { "type": "ActionType", "name": "Button label", "value": "..." }
      }
    }
  ]
}
```

Buttons are identified by letter (A, B, C, … Z). The number of button entries per profile
should match the number of action buttons wired (`CONF_NUM_BUTTONS` in `builder_config.h`).
The default build uses 4 buttons (A–D); the firmware supports up to 26 (A–Z).

### Action type reference

| Type | Description | Required fields |
|------|-------------|-----------------|
| `SendStringAction` | Types a text string | `value` |
| `SendCharAction` | Sends a single key | `value` (key name) |
| `SendKeyAction` | Sends a USB HID key code | `value` (key name) |
| `SendMediaKeyAction` | Sends a media key | `value` (media key name) |
| `SerialOutputAction` | Writes to serial monitor | `value` |
| `DelayedAction` | Waits, then executes another action | `delayMs`, `action` |

For the full list of valid `value` strings per action type, see
[KEY_REFERENCE.md](KEY_REFERENCE.md).

### DelayedAction

`DelayedAction` runs another action after a delay. The `delayMs` field sets how long to wait in
milliseconds. While the countdown runs, the power LED blinks.

Example — the camera remote delayed shutter:

```json
{
  "type": "DelayedAction",
  "name": "Delayed Shutter",
  "delayMs": 3000,
  "action": { "type": "SendMediaKeyAction", "value": "KEY_VOLUME_UP" }
}
```

Press the button, set down the controller, and the action fires 3 seconds later.

## Profile-select LED encoding

The firmware picks an encoding mode at runtime based on how many profiles and select LEDs
you have wired:

**One-hot mode** (`numProfiles ≤ numSelectLeds`): one LED lights exclusively for each profile.
Profile 1 → LED 1, Profile 2 → LED 2, etc. Easiest to read at a glance.

**Binary mode** (`numProfiles > numSelectLeds`): the 1-based profile number is encoded in
binary across all select LEDs. LED 1 = bit 0 (LSB), LED 2 = bit 1, etc.

The maximum number of profiles your wiring can represent is `2^numSelectLeds − 1`:

| Select LEDs wired | Max profiles |
|-------------------|-------------|
| 1 | 1 |
| 2 | 3 |
| 3 | 7 |
| 4 | 15 |
| 5 | 31 |
| 6 | 63 |

If you set `CONF_NUM_PROFILES` higher than this ceiling, the build will fail with an error
message.

## Editing profiles

1. Edit `data/profiles.json`.
2. If you only changed the config file (no firmware changes): run a filesystem-only upload:

   ```bash
   pio run -e nodemcu-32s --target uploadfs
   ```

3. If you also changed firmware: run the full upload:

   ```bash
   make upload-esp32
   ```

## Fallback behaviour

If the JSON file is missing or invalid on startup, all LEDs blink 5 times and the factory default
configuration loads. Factory defaults are hardcoded in `lib/PedalLogic/src/pedal_config.cpp`.

## Hardware target maturity

| Target | Board | Status |
|--------|-------|--------|
| ESP32 | NodeMCU-32S | Deployed and tested |
| nRF52840 | Adafruit Feather nRF52840 | Implemented, not tested — use at own risk |
