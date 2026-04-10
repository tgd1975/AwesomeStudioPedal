# Build Guide

## Hardware target

ESP32 (NodeMCU-32S) is the only deployed and tested target.
nRF52840 (Adafruit Feather nRF52840) is implemented but not tested — no build guide is provided for it.

## Bill of materials

- ESP32 NodeMCU-32S development board
- 1–26 momentary tactile buttons (action buttons A–Z); default build uses 4 (A, B, C, D)
- 1 momentary tactile button (SELECT / profile cycle)
- 1–6 LEDs + current-limiting resistors (profile-select indicator array); default build uses 3
- 1 LED + resistor (Bluetooth status)
- 1 LED + resistor (power indicator)
- USB power supply or LiPo battery with regulator
- Enclosure (see options below)

To customise pin assignments or counts, edit
`lib/hardware/esp32/include/builder_config.h` before building.
Wiring more select LEDs raises the maximum number of profiles (see
[HARDWARE_CONFIG.md](HARDWARE_CONFIG.md) for the encoding table).

## Wiring diagram

```mermaid
graph LR
    ESP["ESP32 NodeMCU-32S"]

    ESP -->|GPIO 26| BLE_LED["LED: Bluetooth status"]
    ESP -->|GPIO 25| PWR_LED["LED: Power indicator"]
    ESP -->|GPIO 5| SEL1["LED: Profile select 1"]
    ESP -->|GPIO 18| SEL2["LED: Profile select 2"]
    ESP -->|GPIO 19| SEL3["LED: Profile select 3"]

    SEL["Button: SELECT"] -->|GPIO 21 pull-up| ESP
    BTN_A["Button: A"] -->|GPIO 13 pull-up| ESP
    BTN_B["Button: B"] -->|GPIO 12 pull-up| ESP
    BTN_C["Button: C"] -->|GPIO 27 pull-up| ESP
    BTN_D["Button: D"] -->|GPIO 14 pull-up| ESP
```

| Signal | GPIO | Type |
|--------|------|------|
| LED: Bluetooth status | GPIO 26 | Output |
| LED: Power indicator | GPIO 25 | Output |
| LED: Profile select 1 | GPIO 5 | Output |
| LED: Profile select 2 | GPIO 18 | Output |
| LED: Profile select 3 | GPIO 19 | Output |
| Button: SELECT | GPIO 21 | Input (pull-up) |
| Button: A | GPIO 13 | Input (pull-up) |
| Button: B | GPIO 12 | Input (pull-up) |
| Button: C | GPIO 27 | Input (pull-up) |
| Button: D | GPIO 14 | Input (pull-up) |

Note: a full Fritzing schematic will be added when available.

## Enclosure options

**3D-printed:** a printable enclosure is available on Thingiverse — link coming.

**Soft-touch footswitch alternative:** if you do not have a 3D printer, or need a more durable build
for live use, any standard SPST momentary footswitch works. This option is more robust for repeated
stomping.

## Next step

[FLASHING.md](FLASHING.md) — flash the firmware and upload the configuration.

## Builder Documentation

### Upload Instructions

**ESP32 (using esptool):**

```bash
# Install esptool
pip install esptool

# Upload firmware (replace COM3 with your port)
esptool.py --chip esp32 --port COM3 --baud 921600 write_flash 0x1000 awesome-pedal-esp32-vX.Y.Z.bin
```

**nRF52840 (using nrfjprog):**

```bash
# Install nrfjprog from Nordic Semiconductor
# Connect your device and run:
nrfjprog --program awesome-pedal-nrf52840-vX.Y.Z.bin --verify --reset
```

### Required Tools

**For ESP32:**

- [esptool](https://github.com/espressif/esptool) (Python-based)
- Python 3.7+
- USB drivers for your specific ESP32 board

**For nRF52840:**

- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools)
- J-Link software (for programming)
- USB drivers for your nRF52840 board
