# Configuration Deployment Guide

## Automatic Deployment Setup

The pedal configuration system is now set up for automatic deployment using PlatformIO's LittleFS support.

## What Happens Automatically

### 1. **Build Process**

When you run `pio run`, PlatformIO will:

- Compile the firmware
- Prepare the file system image containing `data/pedal_config.json`
- The configuration file is embedded in the build process

### 2. **Upload Process**

When you run `pio run --target upload`, PlatformIO will:

- Upload the compiled firmware to the device
- **Automatically upload the file system** containing the configuration

### 3. **First Run Behavior**

On first startup, the pedal will:

1. Try to load configuration from `/pedal_config.json`
2. If not found, use built-in default configuration
3. Save the default configuration to the file system
4. All subsequent runs will use the file system configuration

## Manual Deployment (if needed)

If you need to manually upload just the file system:

```bash
pio run --target uploadfs
```

## Configuration File Location

- **Source**: `data/pedal_config.json` (in your project directory)
- **Device**: `/pedal_config.json` (on the device's LittleFS file system)

## PlatformIO Configuration

The following has been automatically configured:

### platformio.ini

```ini
[env:nodemcu-32s]
board_build.filesystem = littlefs
lib_deps = 
    t-vk/ESP32 BLE Keyboard @ ^0.3.2
    bblanchon/ArduinoJson @ ^6.21.0
```

### partitions.csv

```csv
storage,  data, 0x99,    0x290000,0x170000,
```

## Runtime Configuration Changes

The system supports runtime configuration changes:

```cpp
ConfigLoader configLoader;
// Load modified configuration
configLoader.loadFromString(profileManager, keyboard, newConfigJson);
// Save to persistent storage
configLoader.saveToFile(profileManager, "/pedal_config.json");
```

## Troubleshooting

### File System Not Found

If the device can't find the configuration file:

1. Check that `board_build.filesystem = littlefs` is in platformio.ini
2. Run `pio run --target uploadfs` to upload the file system
3. Check serial output for LittleFS initialization messages

### Configuration Not Loading

1. Verify the JSON file is valid (use a JSON validator)
2. Check file permissions on the device
3. Ensure the partition table includes the storage partition

## File System Management

### Format File System

```cpp
LittleFS.format();
```

### Check File System Info

```cpp
size_t totalBytes = LittleFS.totalBytes();
size_t usedBytes = LittleFS.usedBytes();
```

### List Files

```cpp
File root = LittleFS.open("/");
File file = root.openNextFile();
while(file){
    Serial.print("FILE: ");
    Serial.println(file.name());
    file = root.openNextFile();
}
```

## Future Enhancements

- Web-based configuration editor
- OTA configuration updates
- Configuration versioning and migration
- Backup/restore functionality
