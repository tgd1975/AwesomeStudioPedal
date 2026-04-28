# Data Files Documentation

## Overview

This directory contains files that are deployed to the device's LittleFS filesystem during firmware upload.

## File Reference

| Source File | Device Path | Purpose | Configuration Guide |
|-------------|-------------|---------|---------------------|
| `data/profiles.json` | `/profiles.json` | Button profile configuration | [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md) |
| `data/config.json` | `/config.json` | Hardware pin configuration (optional override) | [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md) |

## Schema Validation

JSON files in this directory are validated against their schemas on every commit:

| JSON File | Schema |
|-----------|--------|
| `data/profiles.json` | `data/profiles.schema.json` |
| `data/config.json` | `data/config.schema.json` |

Validate manually with:

```bash
python3 -c "import jsonschema, json; jsonschema.validate(json.load(open('data/profiles.json')), json.load(open('data/profiles.schema.json')))"
```

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

- [Button Configuration Guide](../builders/HARDWARE_CONFIG.md)
- [Firmware Upload Instructions](../building.md)
