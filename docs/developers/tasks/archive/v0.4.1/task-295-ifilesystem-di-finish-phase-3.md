---
id: TASK-295
title: Phase 3c — IFileSystem DI; eliminate HOST_TEST_BUILD from littlefs (finish Phase 3)
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Medium (2-8h)
effort_actual: Small (<2h)
complexity: Senior
human-in-loop: Support
epic: hal-refactor
order: 7
prerequisites: [TASK-294]
---

## Description

Final slice of Phase 3 of EPIC-020. The most invasive of the three —
introduces full filesystem dependency injection.

`lib/PedalLogic/src/littlefs_file_system.cpp` carries 5 `#ifndef
HOST_TEST_BUILD` guards plus a `#ifdef NRF52840_XXAA` guard. Unlike
the logger / timing / GPIO files, this one cannot be cleaned up with
a compat-header trick: it actually talks to a different filesystem
backend on each platform (LittleFS via SPIFFS on ESP32, LittleFS via
Adafruit_LittleFS on nRF52840), and host tests need a fully
in-memory fake.

Concretely:

- Define `IFileSystem` interface in `lib/PedalLogic/include/file_system.h`
  (the file already exists — extend it). Include open / read / write /
  remove / list operations sufficient for `ConfigLoader` and
  `BleConfigReassembler`.
- Move the platform-specific LittleFS code out of
  `littlefs_file_system.cpp` into per-target implementations:
  - `lib/hardware/esp32/src/esp32_file_system.cpp`
    (ESP32-LittleFS / SPIFFS path)
  - `lib/hardware/nrf52840/src/nrf52840_file_system.cpp`
    (Adafruit_LittleFS path)
  - `test/fakes/host_file_system.cpp` (in-memory map<string, string>)
- Replace direct `LittleFS` calls in `ConfigLoader` /
  `BleConfigReassembler` with calls through an injected
  `IFileSystem*`. Concrete subclasses of `BlePedalApp` /
  `HostPedalApp` construct the right backend.
- Drop `target_compile_definitions(pedal_tests PRIVATE
  HOST_TEST_BUILD)` from `test/CMakeLists.txt`.
- Delete `lib/PedalLogic/src/littlefs_file_system.cpp` (the per-
  target backends now live in `lib/hardware/<target>/src/`).

**Phase 3 milestone met:** `grep -r "HOST_TEST_BUILD\|NRF52840_XXAA"
lib/PedalLogic/` returns zero hits.

## Acceptance Criteria

- [ ] `IFileSystem` interface in
      `lib/PedalLogic/include/file_system.h` covers everything
      `ConfigLoader` / `BleConfigReassembler` need.
- [ ] `Esp32FileSystem`, `Nrf52840FileSystem`, and
      `HostFileSystem` implementations exist in their respective
      directories. Each is selected at link time, not via
      `#ifdef`.
- [ ] `ConfigLoader` and `BleConfigReassembler` no longer call
      `LittleFS.*` directly — they go through the injected
      `IFileSystem*`.
- [ ] `lib/PedalLogic/src/littlefs_file_system.cpp` is deleted.
- [ ] `target_compile_definitions(pedal_tests PRIVATE
      HOST_TEST_BUILD)` is removed from `test/CMakeLists.txt`.
- [ ] **Phase 3 milestone:** `grep -r
      "HOST_TEST_BUILD\|NRF52840_XXAA" lib/PedalLogic/` returns
      zero hits.
- [ ] All builds clean: `pio run -e nodemcu-32s`,
      `pio run -e feather-nrf52840`. No new warnings.
- [ ] `make test-host` passes; the test that exercises config
      load now uses `HostFileSystem` (not the old
      `HOST_TEST_BUILD`-guarded path).
- [ ] On-device smoke test on each target: config load from
      LittleFS still works, BLE config write-and-reload still
      works.

## Test Plan

**Host tests** (`make test-host`):

- Existing `test_config_loader.cpp` and BLE-config tests must
  pass against `HostFileSystem` instead of the `HOST_TEST_BUILD`-
  guarded mock.
- Add `test/unit/test_host_file_system.cpp` covering basic
  open/read/write/list semantics of the in-memory fake.

**On-device tests** (manual, both targets):

- `pio run -e nodemcu-32s` and `pio run -e feather-nrf52840`
  build. Flash and confirm: device loads config.json from
  LittleFS at boot, BLE config write persists across reboot.

## Prerequisites

- **TASK-294** — finishes the simpler guards. By the time this
  task starts, `littlefs_file_system.cpp` is the only PedalLogic
  file with `HOST_TEST_BUILD` and the only thing keeping the
  compile-definition alive.

## Notes

- This is the most invasive task in EPIC-020 — it touches the
  config-loader hot path and changes a class signature
  (`ConfigLoader` constructor gains an `IFileSystem*`
  parameter). Plan a careful review.
- Consider keeping the deprecated `LittleFS` direct-access
  path under a brief compatibility shim if any on-device test
  fixture (`test/test_ble_config_esp32/`) hard-codes file
  paths. Better: update the fixture too.
- Adafruit_LittleFS on nRF52840 has subtly different semantics
  from ESP32-LittleFS (filename length, mkdir behaviour). The
  per-target backend is the right place to absorb those
  differences; `IFileSystem` should expose the lowest common
  denominator.
- Reference: IDEA-028 §"Eliminate platform guards from
  PedalLogic", §"Test / host specialization".
