# Flashing

## Prerequisites

- PlatformIO installed (VS Code extension or CLI)
- USB cable connected to the ESP32
- ESP32 powered on

## Clone and install

```bash
git clone <repo-url>
cd AwesomeStudioPedal
pio lib install
```

## First flash (firmware + filesystem)

On first flash, or after any firmware change, you need to upload both the firmware and the
filesystem. Run these two commands in order:

```bash
make upload-esp32      # flash compiled firmware
make uploadfs-esp32    # flash data/ directory (profiles.json)
```

Or use the combined shortcut that builds, uploads firmware, uploads filesystem, and opens the
serial monitor:

```bash
make run-esp32
```

## Updating the configuration without recompiling

The device configuration lives in `data/profiles.json`. You can edit it and push it to
the device without touching or recompiling the firmware. Only the filesystem partition is
re-written — takes a few seconds.

**ESP32:**

```bash
make uploadfs-esp32
```

**nRF52840:**

```bash
make uploadfs-nrf52840
```

This is the normal workflow for customising profiles, adding new button mappings, or changing
action values. No build step is needed; just edit the JSON file and run the upload.

## Serial port permissions on Linux

If your device is not detected or the upload fails with a permission error, add yourself to the
`dialout` group:

```bash
sudo usermod -a -G dialout $USER
# Log out and back in for this to take effect
```

## Partition layout

The flash is split between the firmware and a LittleFS partition for configuration files.
The partition table is at `config/esp32/partitions.csv`. No changes are needed for standard use.

## Troubleshooting

- **Device not found:** check the USB cable and connection; try a different port.
- **Permission denied (Linux):** verify `dialout` group membership and restart your session.
- **Config not loading:** validate `data/profiles.json` with a JSON linter, then re-run the
  filesystem upload.
- **LittleFS mount failed** (appears in serial output): run `make uploadfs-esp32` to re-upload
  the filesystem.
