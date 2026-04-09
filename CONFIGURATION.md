# Pedal Configuration System

## Overview

The pedal now uses a JSON-based configuration system that allows for runtime configuration changes and persistent storage. This replaces the previous hardcoded configuration approach.

## Configuration File

The configuration is stored in `data/pedal_config.json` and follows this structure:

```json
{
  "profiles": [
    {
      "name": "Profile Name",
      "buttons": {
        "A": {"type": "ActionType", "value": "action_value"},
        "B": {"type": "ActionType", "value": "action_value"},
        "C": {"type": "ActionType", "value": "action_value"},
        "D": {"type": "ActionType", "value": "action_value"}
      }
    }
  ]
}
```

## Supported Action Types

### Send Actions (Keyboard Input)

- **SendStringAction**: Sends a text string

  ```json
  {"type": "SendStringAction", "value": "Hello World"}
  ```

- **SendCharAction**: Sends a single character key

  ```json
  {"type": "SendCharAction", "value": "LEFT_ARROW"}
  ```

- **SendKeyAction**: Sends a USB HID key code

  ```json
  {"type": "SendKeyAction", "value": "UP_ARROW"}
  ```

- **SendMediaKeyAction**: Sends a media key

  ```json
  {"type": "SendMediaKeyAction", "value": "MEDIA_STOP"}
  ```

### Non-Send Actions

- **SerialOutputAction**: Outputs text to serial console

  ```json
  {"type": "SerialOutputAction", "value": "Debug message"}
  ```

### Complex Actions

- **DelayedAction**: Delays execution of another action

  ```json
  {
    "type": "DelayedAction",
    "delayMs": 5000,
    "action": {"type": "SendStringAction", "value": "Delayed text"}
  }
  ```

## Deployment

The configuration file `data/pedal_config.json` must be deployed to the device's SPIFFS file system. This can be done using the PlatformIO upload filesystem command:

```bash
pio run --target uploadfs
```

## Runtime Configuration

The system supports:

1. **Loading from file**: On startup, tries to load from `/pedal_config.json`
2. **Fallback to defaults**: If file loading fails, uses built-in default configuration
3. **Saving changes**: Modified configurations can be saved back to the file system
4. **Runtime modifications**: Future versions will support dynamic configuration changes

## Implementation Details

- **ConfigLoader class**: Handles JSON parsing and Action object creation
- **SPIFFS file system**: Used for persistent storage on the device
- **ArduinoJSON library**: Used for JSON parsing and serialization
- **Action hierarchy**: All actions inherit from the base Action class

## Example Configuration

The default configuration includes three profiles:

1. **Navigation**: Space, Media Stop, Left/Right arrows
2. **Messaging**: "Hello", "World", Up/Down arrows  
3. **Custom**: Mixed actions including delayed and serial output

## Future Enhancements

- Web-based configuration editor
- Runtime configuration API
- Configuration versioning
- User presets and profiles
- Cloud sync for configurations
