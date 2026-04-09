# Hardware Configuration

## Pin assignments

GPIO pin assignments are defined in `include/config.h` and set per-platform in
`lib/hardware/esp32/src/config.cpp`. For the full wiring table, see
[BUILD_GUIDE.md](BUILD_GUIDE.md).

## Profile configuration

Button mappings are stored in `data/pedal_config.json`. The file is deployed to the device's
LittleFS filesystem during upload.

### File structure

```json
{
  "profiles": [
    {
      "name": "Profile Name",
      "buttons": {
        "A": { "type": "ActionType", "name": "Button label", "value": "..." },
        "B": { "type": "ActionType", "name": "Button label", "value": "..." },
        "C": { "type": "ActionType", "name": "Button label", "value": "..." },
        "D": { "type": "ActionType", "name": "Button label", "value": "..." }
      }
    }
  ]
}
```

### Action type reference

| Type | Description | Required fields |
|------|-------------|-----------------|
| `SendStringAction` | Types a text string | `value` |
| `SendCharAction` | Sends a single key | `value` (key name) |
| `SendKeyAction` | Sends a USB HID key code | `value` (key name) |
| `SendMediaKeyAction` | Sends a media key | `value` (media key name) |
| `SerialOutputAction` | Writes to serial monitor | `value` |
| `DelayedAction` | Waits, then executes another action | `delayMs`, `action` |

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

## Editing profiles

1. Edit `data/pedal_config.json`.
2. If you only changed the config file (no firmware changes): run a filesystem-only upload:

   ```bash
   pio run -e nodemcu-32s --target uploadfs
   ```

3. If you also changed firmware: run the full upload:

   ```bash
   make upload-esp32
   ```

## Fallback behaviour

If the JSON file is missing or invalid on startup, all 5 LEDs blink 5 times and the factory default
configuration loads. Factory defaults are hardcoded in `lib/PedalLogic/src/pedal_config.cpp`.

## Hardware target maturity

| Target | Board | Status |
|--------|-------|--------|
| ESP32 | NodeMCU-32S | Deployed and tested |
| nRF52840 | Adafruit Feather nRF52840 | Implemented, not tested — use at own risk |
