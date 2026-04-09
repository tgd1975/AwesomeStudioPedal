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

## Flash firmware and filesystem

Use this on first flash or after any firmware change:

```bash
make upload-esp32
```

This uploads both the compiled firmware and the `data/` directory (which contains `pedal_config.json`)
to the device in one step.

## Filesystem-only update

If you only edited `data/pedal_config.json` and the firmware has not changed, a filesystem-only upload
is faster:

```bash
pio run -e nodemcu-32s --target uploadfs
```

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
- **Config not loading:** validate `data/pedal_config.json` with a JSON linter, then re-run the
  filesystem upload.
- **LittleFS mount failed** (appears in serial output): run
  `pio run -e nodemcu-32s --target uploadfs` to re-upload the filesystem.
