# Data File Upload Configuration

## How pedal_config.json is Automatically Deployed

### PlatformIO's Automatic Data Directory Handling

PlatformIO has built-in support for deploying data files to embedded devices. Here's how it works:

## Automatic Configuration

### 1. Data Directory Convention

PlatformIO automatically recognizes the `data/` directory in your project root:

```
project_root/
├── src/
├── lib/
├── test/
├── data/          ← PlatformIO automatically processes this directory
│   └── pedal_config.json
├── platformio.ini
└── ...
```

### 2. No Explicit Configuration Needed

Unlike some other build systems, PlatformIO **automatically** includes the `data/` directory contents in the file system image. No additional configuration in `platformio.ini` is required.

### 3. How It Works with Our Setup

Our configuration uses:

```ini
[env:nodemcu-32s]
board_build.filesystem = littlefs      # Use LittleFS file system
board_build.partitions = config/esp32/partitions.csv # Use custom partition table
```

The `config/esp32/partitions.csv` defines where the file system goes:

```csv
storage,  data, 0x99,    0x290000,0x170000,  # 1.5MB LittleFS partition
```

## Build and Upload Process

### Build Phase

```bash
pio run
```

1. Compiles the firmware
2. **Creates LittleFS image containing `data/pedal_config.json`**
3. Generates partition table binary from `config/esp32/partitions.csv`
4. Combines everything into flashable images

### Upload Phase

```bash
pio run --target upload
```

1. Flashes partition table to offset 0x8000
2. Flashes bootloader
3. Flashes main application to APP0 partition
4. **Flashes LittleFS image (containing pedal_config.json) to storage partition**

### File System Only Upload

```bash
pio run --target uploadfs
```

- Uploads only the file system (useful for updating configurations without reflashing firmware)

## File Locations

| Development | Device | Purpose |
|-------------|--------|---------|
| `data/pedal_config.json` | `/pedal_config.json` | Pedal configuration |
| `config/esp32/partitions.csv` | N/A | Partition table definition (build-time only) |

## Verification

### Check What Will Be Uploaded

```bash
# List files that will be included in the file system image
pio run --target buildfs
```

### Manual File System Operations

```cpp
// List files on device (debugging)
void listFiles() {
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while(file) {
        Serial.print("FILE: ");
        Serial.print(file.name());
        Serial.print(" SIZE: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }
}

// Check file exists
bool configExists() {
    return LittleFS.exists("/pedal_config.json");
}

// Read file size
size_t configSize() {
    File file = LittleFS.open("/pedal_config.json", "r");
    if (!file) return 0;
    size_t size = file.size();
    file.close();
    return size;
}
```

## Troubleshooting

### Files Not Uploading

1. **Check data directory exists:**

   ```bash
   ls -la data/
   ```

2. **Verify file system configuration:**

   ```ini
   [env:nodemcu-32s]
   board_build.filesystem = littlefs  # Must be set
   ```

3. **Check build output:**

   ```bash
   pio run -v  # Verbose mode shows file system processing
   ```

### File System Not Found on Device

1. **Check partition table:**

   ```bash
   pio run --target partition-table
   ```

2. **Verify LittleFS initialization:**

   ```cpp
   if (!LittleFS.begin(true)) {
       Serial.println("LittleFS mount failed");
       // Try formatting
       LittleFS.format();
       ESP.restart();
   }
   ```

3. **Check flash size:**
   - Ensure your board has enough flash (4MB recommended)
   - Partition table must fit within available flash

## Advanced Configuration

### Custom Data Directory

If you need to use a different directory:

```ini
[env:nodemcu-32s]
board_build.filesystem = littlefs
board_build.fs_start = data/  # Explicit data directory (usually not needed)
```

### Multiple Data Directories

For complex projects with multiple data sources:

```ini
[env:nodemcu-32s]
board_build.filesystem = littlefs
board_build.fs_start =
    data/
    assets/
    configs/
```

## Best Practices

1. **Keep data directory clean:** Only essential files
2. **Use small file names:** Saves space in file system
3. **Compress large files:** If using binary data
4. **Validate JSON:** Before deploying to avoid parse errors
5. **Version configurations:** Include version in JSON for migration

## References

- [PlatformIO File System Documentation](https://docs.platformio.org/en/latest/platforms/espressif32.html#filesystem)
- [LittleFS with PlatformIO](https://docs.platformio.org/en/latest/platforms/espressif32.html#littlefs)
- [ESP32 Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)
