---
id: TASK-027
title: Create upload instructions
status: open
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Document how to flash a pre-built firmware binary onto the device for both ESP32 and nRF52840.
These are instructions for end-users and builders who are not compiling from source.

## Acceptance Criteria

- [ ] Upload instructions exist (in `docs/building.md` or a dedicated `docs/upload.md`)
- [ ] ESP32 instructions use `esptool.py` with correct flash address and baud rate
- [ ] nRF52840 instructions use `nrfjprog`
- [ ] Both sections include "install the tool" step
- [ ] Linked from `README.md` and the firmware versions section (TASK-025)

## Notes

ESP32 example:

```bash
pip install esptool
esptool.py --chip esp32 --port COM3 --baud 921600 write_flash 0x1000 awesome-pedal-esp32-v1.2.0.bin
```

nRF52840 example:

```bash
# Install nRF Command Line Tools from Nordic Semiconductor
nrfjprog --program awesome-pedal-nrf52840-v1.2.0.bin --verify --reset
```

Replace `COM3` with the actual port (Linux: `/dev/ttyUSB0`, macOS: `/dev/cu.usbserial-*`).
