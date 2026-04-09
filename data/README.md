# Data Directory - Automatic File Upload

## Important: All Files Here Are Automatically Deployed

This `data/` directory contains files that are **automatically uploaded** to the pedal device's file system during the build and upload process.

## How It Works

### Automatic Deployment

1. **Build Process:**

   ```bash
   pio run
   ```

   - PlatformIO automatically includes all files in this directory
   - Creates a LittleFS file system image
   - Embeds files in the firmware build

2. **Upload Process:**

   ```bash
   pio run --target upload
   ```

   - Uploads firmware to the device
   - **Automatically uploads all files from this directory**
   - Files become available on the device's LittleFS file system

3. **File System Only Update:**

   ```bash
   pio run --target uploadfs
   ```

   - Uploads only the file system (no firmware update)
   - Useful for updating configurations without reflashing firmware

## File Locations

| Development Path | Device Path | Purpose |
|------------------|-------------|---------|
| `data/pedal_config.json` | `/pedal_config.json` | Main pedal configuration |
| `data/*.json` | `/*.json` | Any additional JSON files |
| `data/*.txt` | `/*.txt` | Text files (logs, docs, etc.) |

## Current Files

- **`pedal_config.json`** - Main pedal configuration file
  - Defines button profiles and actions
  - JSON format for easy editing
  - Automatically loaded by the pedal on startup

## Adding New Files

To add new files that should be deployed:

1. **Place files in this directory:**

   ```bash
   cp my_file.json data/
   ```

2. **Files are automatically included** in next build/upload

3. **Access files in code:**

   ```cpp
   File file = LittleFS.open("/my_file.json", "r");
   if (file) {
       // Read file contents
       String content = file.readString();
       file.close();
   }
   ```

## Important Notes

### File System Limitations

- **ESP32 Flash Size:** Typically 4MB total
- **Storage Partition:** 1.5MB allocated for LittleFS
- **Max File Size:** Keep individual files under 500KB
- **Total Files:** Hundreds of small files are fine

### File Naming

- **Use short names:** `config.json` not `my_very_long_configuration_file.json`
- **Avoid special characters:** Stick to alphanumeric + underscore
- **Use lowercase:** `pedal_config.json` not `Pedal_Config.JSON`

### File Updates

- **Modified files** are automatically included in next upload
- **Deleted files** remain on device until explicitly removed
- **To clean up:** Format file system or delete files programmatically

## Accessing Files on Device

### List All Files (Debugging)

```cpp
void listFiles() {
    Serial.println("Files on device:");
    
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    
    while(file) {
        Serial.print("  /");
        Serial.print(file.name());
        Serial.print(" (");
        Serial.print(file.size());
        Serial.println(" bytes)");
        
        file = root.openNextFile();
    }
}
```

### Check File Exists

```cpp
bool fileExists(const char* path) {
    return LittleFS.exists(path);
}
```

### Delete File

```cpp
bool deleteFile(const char* path) {
    return LittleFS.remove(path);
}
```

### Format File System (Erase All)

```cpp
bool formatFileSystem() {
    return LittleFS.format();
}
```

## Configuration File Format

The main `pedal_config.json` file uses this structure:

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

## Best Practices

1. **Keep it simple:** Only essential files in this directory
2. **Validate JSON:** Use `jq` or online validators before deploying
3. **Version files:** Include version numbers in configuration files
4. **Document changes:** Keep this README updated when adding files
5. **Test updates:** Verify file system operations work after changes

## Troubleshooting

### Files Not Uploading

- Check file permissions: `ls -la data/`
- Verify PlatformIO configuration: `board_build.filesystem = littlefs`
- Build with verbose output: `pio run -v`

### File System Errors

- Format file system: `LittleFS.format()`
- Check free space: `LittleFS.totalBytes() - LittleFS.usedBytes()`
- Verify partition table: `pio run --target partition-table`

### Configuration Not Loading

- Check file path: Must start with `/`
- Verify JSON syntax: Use `Serial.println(configContent)` to debug
- Test with simple file first: Create `data/test.txt` with "Hello World"

## See Also

- [DATA_UPLOAD.md](../docs/DATA_UPLOAD.md) - Detailed upload process
- [PARTITIONS.md](../docs/PARTITIONS.md) - Partition table explanation
- [CONFIGURATION.md](../docs/CONFIGURATION.md) - Configuration format details
