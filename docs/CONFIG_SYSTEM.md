# Pedal Configuration System - Complete Guide

## Overview

The pedal now uses a **JSON-based configuration system** that supports:

- ✅ External configuration files (no hardcoding)
- ✅ Automatic deployment to device
- ✅ Runtime configuration changes
- ✅ Persistent storage across reboots
- ✅ Multiple action types (send, delayed, serial output, etc.)

## Quick Start

### 1. Edit Configuration

Modify `data/pedal_config.json` with your desired button mappings:

```json
{
  "profiles": [{
    "name": "MyProfile",
    "buttons": {
      "A": {"type": "SendStringAction", "value": "Hello"},
      "B": {"type": "SendKeyAction", "value": "UP_ARROW"}
    }
  }]
}
```

### 2. Build and Upload

```bash
pio run --target upload
```

This automatically:

- Compiles the firmware
- Creates LittleFS image with your configuration
- Flashes everything to the device

### 3. Done

The pedal will automatically load your configuration on startup.

## File Structure

```
project/
├── data/
│   ├── README.md              # You are here
│   └── pedal_config.json      # Main configuration file
├── lib/PedalLogic/
│   ├── include/config_loader.h # Configuration loader
│   └── src/config_loader.cpp   # Implementation
├── platformio.ini             # Build configuration
├── partitions.csv             # Flash memory layout
└── DATA_UPLOAD.md             # Deployment details
```

## Configuration File Format

### Basic Structure

```json
{
  "profiles": [
    {
      "name": "ProfileName",
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

### Supported Action Types

#### Send Actions (Keyboard Input)

- **SendStringAction**: Send text string

  ```json
  {"type": "SendStringAction", "value": "Hello World"}
  ```

- **SendCharAction**: Send character key

  ```json
  {"type": "SendCharAction", "value": "LEFT_ARROW"}
  ```

- **SendKeyAction**: Send USB HID key

  ```json
  {"type": "SendKeyAction", "value": "UP_ARROW"}
  ```

- **SendMediaKeyAction**: Send media key

  ```json
  {"type": "SendMediaKeyAction", "value": "MEDIA_STOP"}
  ```

#### Non-Send Actions

- **SerialOutputAction**: Debug/output to serial

  ```json
  {"type": "SerialOutputAction", "value": "Debug message"}
  ```

#### Complex Actions

- **DelayedAction**: Delayed execution

  ```json
  {
    "type": "DelayedAction",
    "delayMs": 5000,
    "action": {"type": "SendStringAction", "value": "Delayed text"}
  }
  ```

## Example Configurations

### Navigation Profile

```json
{
  "name": "Navigation",
  "buttons": {
    "A": {"type": "SendStringAction", "value": " "},
    "B": {"type": "SendMediaKeyAction", "value": "MEDIA_STOP"},
    "C": {"type": "SendCharAction", "value": "LEFT_ARROW"},
    "D": {"type": "SendCharAction", "value": "RIGHT_ARROW"}
  }
}
```

### Messaging Profile

```json
{
  "name": "Messaging",
  "buttons": {
    "A": {"type": "SendStringAction", "value": "Hello"},
    "B": {"type": "SendStringAction", "value": "World"},
    "C": {"type": "SendKeyAction", "value": "UP_ARROW"},
    "D": {"type": "SendKeyAction", "value": "DOWN_ARROW"}
  }
}
```

### Advanced Profile with Mixed Actions

```json
{
  "name": "Advanced",
  "buttons": {
    "A": {"type": "SendStringAction", "value": "Profile A"},
    "B": {"type": "SerialOutputAction", "value": "Button B pressed"},
    "C": {
      "type": "DelayedAction",
      "delayMs": 3000,
      "action": {"type": "SendStringAction", "value": "Delayed action"}
    },
    "D": {"type": "SendStringAction", "value": "Profile D"}
  }
}
```

## Runtime Configuration

### Load New Configuration

```cpp
ConfigLoader configLoader;
String newConfig = "{\"profiles\": [...]}";
configLoader.loadFromString(profileManager, keyboard, newConfig);
```

### Save Current Configuration

```cpp
configLoader.saveToFile(profileManager, "/pedal_config.json");
```

### Reset to Defaults

```cpp
configLoader.loadFromString(profileManager, keyboard, configLoader.getDefaultConfig());
```

## Deployment

### Automatic Deployment

Files in `data/` are automatically deployed:

```bash
# Build and upload everything
pio run --target upload

# Upload only file system (faster for config changes)
pio run --target uploadfs
```

### Manual File Operations

```cpp
// List files
void listFiles() {
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while(file) {
        Serial.printf("/%s (%d bytes)\n", file.name(), file.size());
        file = root.openNextFile();
    }
}

// Check if file exists
bool configExists() {
    return LittleFS.exists("/pedal_config.json");
}

// Delete file
LittleFS.remove("/old_config.json");

// Format file system (erase all)
LittleFS.format();
```

## Troubleshooting

### Configuration Not Loading

1. **Check file exists:**

   ```cpp
   bool exists = LittleFS.exists("/pedal_config.json");
   ```

2. **Validate JSON:** Use online JSON validator

3. **Check LittleFS initialization:**

   ```cpp
   if (!LittleFS.begin(true)) {
       Serial.println("LittleFS mount failed!");
       LittleFS.format();
       ESP.restart();
   }
   ```

### File System Full

```cpp
size_t total = LittleFS.totalBytes();
size_t used = LittleFS.usedBytes();
Serial.printf("Usage: %d/%d bytes\n", used, total);
```

### Partition Table Issues

```bash
# Check partition table
pio run --target partition-table

# Verify flash size matches your board (typically 4MB)
```

## Advanced Topics

### Custom Action Types

To add new action types:

1. Create new `Action` subclass
2. Add to `createActionFromJson()` in `config_loader.cpp`
3. Add to `actionToJson()` for serialization
4. Update JSON schema documentation

### Configuration Versioning

```json
{
  "version": "1.0",
  "profiles": [...]
}
```

### Multiple Configuration Files

```cpp
// Load different configuration
configLoader.loadFromFile(profileManager, keyboard, "/alt_config.json");
```

### Configuration Backup/Restore

```cpp
// Backup current config
String backup = readFile("/pedal_config.json");
writeFile("/backup_config.json", backup);

// Restore from backup
String restored = readFile("/backup_config.json");
configLoader.loadFromString(profileManager, keyboard, restored);
```

## Files Reference

| File | Purpose |
|------|---------|
| `data/pedal_config.json` | Main configuration file |
| `data/README.md` | Data directory documentation |
| `lib/PedalLogic/include/config_loader.h` | ConfigLoader interface |
| `lib/PedalLogic/src/config_loader.cpp` | ConfigLoader implementation |
| `platformio.ini` | Build configuration |
| `partitions.csv` | Flash memory layout |
| `CONFIGURATION.md` | Configuration format details |
| `DATA_UPLOAD.md` | Deployment process |
| `PARTITIONS.md` | Partition table explanation |

## Support

### Key Constants

```cpp
// Button constants
Button::A, Button::B, Button::C, Button::D

// Key constants
KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW
KEY_MEDIA_STOP, etc.
```

### Common Key Values

| Constant | Value | Description |
|----------|-------|-------------|
| `KEY_UP_ARROW` | 0xDA | Up arrow key |
| `KEY_DOWN_ARROW` | 0xD9 | Down arrow key |
| `KEY_LEFT_ARROW` | 0xD8 | Left arrow key |
| `KEY_RIGHT_ARROW` | 0xD7 | Right arrow key |
| `KEY_MEDIA_STOP` | {4, 0} | Media stop key |

## Migration Guide

### From Hardcoded to JSON

**Before:**

```cpp
profile0->addAction(Button::A, std::unique_ptr<Action>(new SendStringAction(keyboard, " ")));
```

**After:**

```json
{
  "profiles": [{
    "name": "Navigation",
    "buttons": {
      "A": {"type": "SendStringAction", "value": " "}
    }
  }]
}
```

### Adding New Profiles

Just add to the `profiles` array:

```json
{
  "profiles": [
    {"name": "ExistingProfile", ...},
    {"name": "NewProfile", "buttons": {...}}  // Add new profile here
  ]
}
```

### Changing Button Actions

Modify the button's action object:

```json
{
  "buttons": {
    "A": {"type": "SendStringAction", "value": "Old"}  // Before
    "A": {"type": "SendKeyAction", "value": "UP_ARROW"}  // After
  }
}
```

## Best Practices

1. **Keep configurations small** (< 500KB per file)
2. **Use meaningful profile names**
3. **Test configurations** before deploying
4. **Backup important configurations**
5. **Document custom actions** in README
6. **Version your configurations**
7. **Validate JSON** before uploading
8. **Test file system operations**
9. **Monitor free space**
10. **Handle errors gracefully** in code

## Future Enhancements

- Web-based configuration editor
- Configuration presets and libraries
- Cloud sync for configurations
- Configuration versioning and migration
- User interface for runtime configuration
- Configuration validation tools
