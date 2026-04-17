# Building and Flashing from Source

This guide is for builders who want to compile the firmware themselves.
If you just want to flash a pre-built binary, see [Build Guide — Upload](builders/BUILD_GUIDE.md#upload-instructions).

## Prerequisites

### Quickest setup

Open the project in a [VS Code Dev Container](https://code.visualstudio.com/docs/devcontainers/containers)
or [GitHub Codespaces](https://github.com/features/codespaces) — all tools are installed automatically.

### Manual install

| Tool | Required for | Install |
|------|--------------|---------|
| [PlatformIO CLI](https://platformio.org/install/cli) | Build + flash ESP32/nRF52840 | `pip install platformio` |
| [CMake](https://cmake.org/download/) 3.20+ | Host unit tests | package manager or cmake.org |
| [Ninja](https://ninja-build.org/) | Host unit tests | `apt install ninja-build` |
| [GCC/G++](https://gcc.gnu.org/) 11+ | Host unit tests | `apt install g++` |
| [Python 3.9+](https://www.python.org/) | PlatformIO, scripts | system or python.org |
| [Git](https://git-scm.com/) | Version control | `apt install git` |

For the full optional tools list (clang-tidy, lcov, Doxygen), see
[DEVELOPMENT_SETUP.md](developers/DEVELOPMENT_SETUP.md).

## Get the source

```bash
git clone https://github.com/tgd1975/AwesomeStudioPedal.git
cd AwesomeStudioPedal
```

## Build the firmware

### ESP32 (NodeMCU-32S) — recommended

```bash
make build-esp32
# Output: .pio/build/nodemcu-32s/firmware.bin
```

Or with PlatformIO directly:

```bash
pio run -e nodemcu-32s
```

### nRF52840 (Adafruit Feather) — experimental

> nRF52840 support is implemented but not tested on hardware. Use at your own risk.

```bash
make build-nrf52840
# Output: .pio/build/feather-nrf52840/firmware.bin
```

## Flash to device

### ESP32

Connect the NodeMCU-32S via USB, then:

```bash
make upload-esp32        # flash firmware
make uploadfs-esp32      # flash configuration (data/ directory)
```

Or combined:

```bash
make run-esp32           # build + upload firmware + upload config + open monitor
```

**Linux only:** if the upload fails with a permission error, add yourself to the `dialout` group:

```bash
sudo usermod -a -G dialout $USER
# Log out and back in
```

### nRF52840

```bash
make upload-nrf52840
make uploadfs-nrf52840
```

Requires [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools)
(`nrfjprog`) from Nordic Semiconductor.

## Flash a pre-built binary (no compilation)

Pre-built binaries are attached to each [GitHub Release](../../../releases).

**ESP32** — using `esptool.py`:

```bash
pip install esptool
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 write_flash 0x1000 awesome-pedal-esp32-vX.Y.Z.bin
```

Replace `/dev/ttyUSB0` with your port (`COM3` on Windows, `/dev/cu.usbserial-*` on macOS).

**nRF52840** — using `nrfjprog`:

```bash
nrfjprog --program awesome-pedal-nrf52840-vX.Y.Z.bin --verify --reset
```

## Run host unit tests

No hardware needed — tests run on the development machine:

```bash
make test-host
```

Or directly:

```bash
cmake -B .vscode/build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build .vscode/build --target pedal_tests
.vscode/build/test/pedal_tests
```

## Update configuration without recompiling

Edit `data/pedal_config.json` then push only the filesystem partition:

```bash
make uploadfs-esp32
```

The device picks up the new configuration on next boot. No recompilation needed.

## Next steps

- [Hardware wiring and BOM](builders/BUILD_GUIDE.md) — pin connections and bill of materials
- [Hardware configuration](builders/HARDWARE_CONFIG.md) — customise pin assignments and profile count
- [Architecture](developers/ARCHITECTURE.md) — codebase overview for contributors
