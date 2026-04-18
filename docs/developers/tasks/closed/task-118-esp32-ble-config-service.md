---
id: TASK-118
title: ESP32 BLE Config Service Implementation
status: closed
opened: 2026-04-17
effort: Large (>8h)
complexity: Senior
human-in-loop: Support
group: BLEConfig
order: 4
prerequisites: [TASK-117]
---

## Description

Implement `BleConfigService` — an ESP32 GATT service that receives a new `profiles.json` (and optionally `config.json`) over BLE chunked writes, validates and applies it, saves to LittleFS, and notifies `CONFIG_STATUS`. The service co-exists with the existing BLE HID service using NimBLE.

## Acceptance Criteria

- [ ] `lib/hardware/esp32/include/ble_config_service.h` created with `begin()`, `loop()`, `isApplying()` interface
- [ ] `lib/hardware/esp32/src/ble_config_service.cpp` implements chunked reassembly, JSON validation, `configureProfiles()` call, and LittleFS save
- [ ] `CONFIG_WRITE` characteristic accepts chunked packets (2-byte sequence number + payload); reassembles into full JSON
- [ ] `CONFIG_STATUS` characteristic notifies `OK` on success, `ERROR:<reason>` on failure, `BUSY` if transfer already in progress
- [ ] Hardware config upload via `CONFIG_WRITE_HW` characteristic applies `loadHardwareConfig()` and saves
- [ ] **LED blink on success:** all select LEDs blink 3× at 150 ms on / 150 ms off, then restore profile state (`ProfileManager::updateLEDs()`) — synchronous, before `OK` notification
- [ ] **LED blink on failure:** single 500 ms blink of all select LEDs
- [ ] Concurrent upload rejected with `BUSY` notification
- [ ] NimBLE HID and config services co-exist without conflict
- [ ] `src/main.cpp` calls `bleConfigService.begin(profileManager, bleKeyboard)` and `bleConfigService.loop()` in main loop
- [ ] Source registered in PlatformIO `lib_deps` or `CMakeLists.txt` as appropriate

## Files to Touch

- `lib/hardware/esp32/include/ble_config_service.h` (new)
- `lib/hardware/esp32/src/ble_config_service.cpp` (new)
- `src/main.cpp` — call `begin()` and `loop()`

## Test Plan

**Host tests** (`make test-host`): covered by TASK-120 (fake `IFileSystem` and `ProfileManager` stub).
**On-device integration**: covered by TASK-121.

## Prerequisites

- **TASK-117** — UUIDs and protocol details from spec document

## Notes

NimBLE on ESP32 can host both the BLE HID profile and a custom GATT service simultaneously. Keep `MAX_CONFIG_BYTES` configurable (default 32 KB). The LED blink sequence is synchronous (blocking 900 ms) and completes before `CONFIG_STATUS OK` is notified — ensures no race condition between physical feedback and CLI/app acknowledgement.
