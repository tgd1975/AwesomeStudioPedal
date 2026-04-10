# Development Setup

## Requirements

- **PlatformIO** — for building and flashing firmware (VS Code extension or standalone CLI)
- **CMake + Ninja** — for running host unit tests
- **clang-format** — for code formatting (enforced by pre-commit hook)
- **Python 3** (optional) — for the alternative serial monitor script

## Build command reference

| Command | What it does | Hardware needed |
|---------|--------------|----------------|
| `make build-esp32` | Build firmware for ESP32 | No |
| `make upload-esp32` | Flash firmware to ESP32 | Yes |
| `make uploadfs-esp32` | Upload `data/` filesystem image to ESP32 | Yes |
| `make run-esp32` | Build + upload firmware + upload filesystem + monitor | Yes |
| `make monitor-esp32` | Open serial monitor | Yes |
| `make test-host` | Run host unit tests (GoogleTest) | No |
| `make test-esp32-button` | Upload filesystem + on-device button tests | Yes (ESP32) |
| `make test-esp32-serial` | Upload filesystem + on-device serial tests | Yes (ESP32) |
| `make test-esp32-profilemanager` | Upload filesystem + on-device profile manager tests | Yes (ESP32) |
| `make build-nrf52840` | Build firmware for nRF52840 | No |
| `make upload-nrf52840` | Flash firmware to nRF52840 | Yes |
| `make uploadfs-nrf52840` | Upload `data/` filesystem image to nRF52840 | Yes |
| `make run-nrf52840` | Build + upload firmware + upload filesystem + monitor | Yes (nRF52840) |
| `make test-nrf52840-profilemanager` | Upload filesystem + on-device profile manager tests | Yes (nRF52840) |

## Updating the configuration without recompiling

`data/pedal_config.json` is the only file that needs to change when adding, removing, or editing
profiles and button actions. The firmware itself does not need to be rebuilt — only the filesystem
partition is re-flashed.

```bash
# Edit the config, then:
make uploadfs-esp32      # ESP32
make uploadfs-nrf52840   # nRF52840
```

The device reads the config at startup via `ConfigLoader::loadFromFile()`, so the next boot will
pick up the changes automatically. This is the normal workflow for profile customisation.

All `make test-esp32-*` and `make test-nrf52840-*` targets include the filesystem upload as a
prerequisite, so you never need to run it manually before running device tests.

## Host test build

Host tests run on the development machine via CMake and Ninja:

```bash
cmake -B .vscode/build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build .vscode/build --target pedal_tests
```

Or use the Makefile shortcut:

```bash
make test-host
```

## Serial port setup on Linux

See [FLASHING.md](../builders/FLASHING.md) for the `dialout` group setup steps.

## Alternative serial monitor

If PlatformIO's built-in monitor has issues, use the Python script:

```bash
python3 scripts/serial_monitor.py /dev/ttyUSB0 115200
```

## API documentation (local)

Generate Doxygen output locally:

```bash
doxygen Doxyfile
# Output: docs/api/html/index.html
```

The `docs/api/` directory is gitignored — generated output is never committed.
