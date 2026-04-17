---
id: TASK-028
title: Document required tools
status: closed
closed: 2026-04-11
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Document all tools required to build, flash, and develop the firmware, with version requirements
and installation links for each platform (Linux, macOS, Windows).

## Acceptance Criteria

- [ ] Required tools documented (in `docs/building.md` or a dedicated section)
- [ ] ESP32 tools covered: `esptool`, Python 3.7+, USB drivers
- [ ] nRF52840 tools covered: nRF Command Line Tools, J-Link software, USB drivers
- [ ] Build tools covered: PlatformIO, CMake, `clang-format`, `clang-tidy`
- [ ] Linked from `README.md` and `CONTRIBUTING.md`

## Notes

ESP32 required tools:

- [esptool](https://github.com/espressif/esptool) (Python-based, `pip install esptool`)
- Python 3.7+
- USB drivers for the specific ESP32 board

nRF52840 required tools:

- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools)
- J-Link software (for programming via SWD)
- USB drivers for the nRF52840 board
