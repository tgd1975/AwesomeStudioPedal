# Partition Table Configuration

## Overview

The `config/esp32/partitions.csv` file defines how the ESP32's flash memory is divided into different sections for various purposes.

## Partition Table Explanation

```csv
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,      # Non-Volatile Storage (20KB)
otadata,  data, ota,     0xe000,  0x2000,      # OTA Data (8KB)
app0,     app,  ota_0,   0x10000, 0x140000,    # Main Application (1.25MB)
app1,     app,  ota_1,   0x150000,0x140000,    # OTA Update Slot (1.25MB)
storage,  data, 0x99,    0x290000,0x170000,    # Configuration Storage (1.5MB)
```

## Partition Details

### 1. NVS (Non-Volatile Storage)

- **Size**: 20KB (0x5000 bytes)
- **Purpose**: Stores WiFi credentials, device settings, and other persistent configuration
- **Offset**: 0x9000 (36KB from start)

### 2. OTADATA

- **Size**: 8KB (0x2000 bytes)
- **Purpose**: Stores OTA (Over-The-Air) update metadata
- **Offset**: 0xE000 (56KB from start)

### 3. APP0 (Main Application)

- **Size**: 1.25MB (0x140000 bytes)
- **Purpose**: Primary application firmware
- **Offset**: 0x10000 (64KB from start)

### 4. APP1 (OTA Update Slot)

- **Size**: 1.25MB (0x140000 bytes)
- **Purpose**: Secondary slot for OTA updates
- **Offset**: 0x150000 (1.3125MB from start)

### 5. STORAGE (Configuration Storage)

- **Size**: 1.5MB (0x170000 bytes)
- **Purpose**: LittleFS file system for pedal configurations
- **Offset**: 0x290000 (2.625MB from start)
- **SubType**: 0x99 (custom data type)

## PlatformIO Integration

The partition table is integrated into PlatformIO via:

```ini
[env:nodemcu-32s]
board_build.partitions = config/esp32/partitions.csv
board_build.filesystem = littlefs
```

## How It Works

1. **Build Process**: PlatformIO uses `config/esp32/partitions.csv` to create the partition table binary
2. **Flash Layout**: The ESP32's flash is divided according to the partition table
3. **File System**: LittleFS uses the "storage" partition (1.5MB) for configuration files
4. **Runtime**: The pedal accesses `/pedal_config.json` from the LittleFS partition

## Memory Usage Breakdown

| Component | Size | Purpose |
|-----------|------|---------|
| Bootloader | ~32KB | ESP32 bootloader |
| NVS | 20KB | WiFi credentials & settings |
| OTA Data | 8KB | OTA update metadata |
| App0 | 1.25MB | Main application |
| App1 | 1.25MB | OTA update slot |
| Storage | 1.5MB | Configuration files |
| **Total** | **4MB** | (typical ESP32 flash size) |

## Customization

### Increasing Storage Size

To increase the storage partition (for more configuration files):

```csv
storage,  data, 0x99,    0x290000,0x200000,  # Increased to 2MB
```

### Adding Partitions

To add a new partition (e.g., for logs):

```csv
logs,     data, 0x9a,    0x400000,0x100000,  # Logs partition (1MB)
```

## Verification

### Check Partition Table

```cpp
esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
while (it) {
    const esp_partition_t* partition = esp_partition_get(it);
    Serial.printf("Partition: %s, Type: %d, SubType: %d, Size: %d\n",
                  partition->label, partition->type, partition->subtype, partition->size);
    it = esp_partition_next(it);
}
```

### Check LittleFS Info

```cpp
size_t total = LittleFS.totalBytes();
size_t used = LittleFS.usedBytes();
Serial.printf("LittleFS: Total: %d KB, Used: %d KB, Free: %d KB\n",
              total / 1024, used / 1024, (total - used) / 1024);
```

## Troubleshooting

### Partition Table Errors

- **Symptom**: Device fails to boot or file system not found
- **Solution**: Verify partition offsets don't overlap
- **Check**: `pio run --target partition-table`

### Insufficient Storage

- **Symptom**: File system operations fail
- **Solution**: Increase storage partition size or reduce other partitions
- **Check**: Monitor used/free space with `LittleFS.usedBytes()`

### OTA Update Failures

- **Symptom**: OTA updates fail
- **Solution**: Ensure APP0 and APP1 partitions are same size
- **Check**: Verify partition sizes match in `config/esp32/partitions.csv`

## Best Practices

1. **Keep NVS Small**: 20KB is typically sufficient for credentials
2. **Balance App/Storage**: Adjust based on application size vs configuration needs
3. **Alignment**: Keep partitions aligned to 4KB boundaries for best performance
4. **Document**: Always document custom partition tables for future reference

## References

- [ESP32 Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/partition.html)
- [LittleFS Documentation](https://github.com/littlefs-project/littlefs)
- [PlatformIO Partition Configuration](https://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables)
