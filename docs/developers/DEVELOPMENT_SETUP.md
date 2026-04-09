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
| `make upload-esp32` | Flash firmware + filesystem to ESP32 | Yes |
| `make monitor-esp32` | Open serial monitor | Yes |
| `make test-host` | Run host unit tests (GoogleTest) | No |
| `make test-esp32-button` | On-device button tests | Yes (ESP32) |
| `make test-esp32-serial` | On-device serial tests | Yes (ESP32) |
| `make build-nrf52840` | Build firmware for nRF52840 | No |

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
