# Tests

This directory contains all tests for the project, using two frameworks:

- **Host unit tests** (GoogleTest, via CMake) — run on the development machine, no hardware needed
- **On-device integration tests** (Unity, via PlatformIO) — run on physical hardware

## Running tests

| Command | What it runs |
|---|---|
| `make test-host` | Host unit tests (GoogleTest) |
| `make test-esp32-button` | On-device button tests (ESP32) |
| `make test-esp32-serial` | On-device serial output tests (ESP32) |
| `make test-nrf52840-button` | On-device button tests (nRF52840) |
| `make test-nrf52840-serial` | On-device serial output tests (nRF52840) |

For full testing context — hardware seam, mock classes, writing new tests, and coverage — see
[docs/developers/TESTING.md](../docs/developers/TESTING.md).

## Known startup messages

When running on-device tests you will see two lines like:

```text
E (156) esp_core_dump_flash: No core dump partition found!
```

These are harmless. The ESP32 checks for a core dump partition on every
boot to report any previous crash. Because `config/esp32/partitions.csv` does not define
a coredump partition, the chip logs these two lines and continues normally.
They appear before the Unity test runner starts and have no effect on results.
