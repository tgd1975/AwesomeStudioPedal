# Data Files Documentation

## Overview

This directory contains files that are deployed to the device's LittleFS filesystem during firmware upload.

## File Reference

| Source File | Device Path | Purpose | Configuration Guide |
|-------------|-------------|---------|---------------------|
| `data/profiles.json` | `/profiles.json` | Button profile configuration | [HARDWARE_CONFIG.md](../../builders/HARDWARE_CONFIG.md) |

## Usage

### Adding New Data Files

1. Place files in the `data/` directory
2. Files will be automatically deployed during upload
3. Reference files in your code using absolute paths (e.g., `/profiles.json`)

### Modifying Existing Files

- Edit files directly in the `data/` directory
- Changes will be deployed on next firmware upload
- Always validate JSON format before uploading

## Technical Details

- **Filesystem**: LittleFS
- **Max File Size**: Platform-dependent (check your specific hardware)
- **Character Encoding**: UTF-8
- **Line Endings**: Unix (LF)

## Related Documentation

- [Button Configuration Guide](../../builders/HARDWARE_CONFIG.md)
- [Firmware Upload Instructions](../../building.md)
- [File Format Specifications](../../developers/FILE_FORMATS.md)
