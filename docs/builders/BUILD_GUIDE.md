# Build Guide

## Hardware target

ESP32 (NodeMCU-32S) is the only deployed and tested target.
nRF52840 (Adafruit Feather nRF52840) is implemented but not tested — no build guide is provided for it.

## Bill of materials

- ESP32 NodeMCU-32S development board
- 4 momentary tactile buttons (action: A, B, C, D)
- 1 momentary tactile button (SELECT / profile cycle)
- 3 LEDs + current-limiting resistors (profile indicator array)
- 1 LED + resistor (Bluetooth status)
- 1 LED + resistor (power indicator)
- USB power supply or LiPo battery with regulator
- Enclosure (see options below)

## Wiring diagram

```mermaid
graph LR
    ESP["ESP32 NodeMCU-32S"]

    ESP -->|GPIO 26| BLE_LED["LED: Bluetooth status"]
    ESP -->|GPIO 25| PWR_LED["LED: Power indicator"]
    ESP -->|GPIO 5|  BIT1["LED: Profile bit 1 (LSB)"]
    ESP -->|GPIO 18| BIT2["LED: Profile bit 2"]
    ESP -->|GPIO 19| BIT3["LED: Profile bit 3 (MSB)"]

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
| LED: Profile bit 1 (LSB) | GPIO 5 | Output |
| LED: Profile bit 2 | GPIO 18 | Output |
| LED: Profile bit 3 (MSB) | GPIO 19 | Output |
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
