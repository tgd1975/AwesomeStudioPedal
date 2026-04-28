---
id: TASK-282
title: Introduce PedalApp base + Esp32PedalApp, migrate ESP32 #ifdef blocks
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Medium (2-8h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 2
prerequisites: [TASK-261]
---

## Description

Introduce the `PedalApp` class hierarchy described in IDEA-028. This task
covers the ESP32 side only:

- Add a `PedalApp` abstract base class in `src/pedal_app.h` (no
  `<Arduino.h>`, no hardware-specific includes — only standard C++ and
  project interfaces).
- Add `Esp32PedalApp` in `lib/hardware/esp32/src/esp32_pedal_app.{h,cpp}`
  containing everything currently guarded by `#ifdef ESP32` in
  `src/esp32/main.cpp` — `BleConfigService`, NVS-based profile
  persistence, the `bleConfigService.loop()` call.
- Reduce `src/esp32/main.cpp` to the 4-line shim: include the header,
  declare a static `Esp32PedalApp app`, define `setup()`/`loop()` as
  forwarding calls. Zero `#ifdef ESP32` in this file.
- The shared application skeleton (button objects, LED objects,
  ProfileManager, EventDispatcher, ISRs, `process_events()`,
  `attachInterrupts`/`detachInterrupts`, the connect/disconnect block,
  power LED blink loop) moves into `PedalApp` itself. `BlePedalApp`
  is **not** introduced here — that is Phase 2 / TASK-263.

`src/nrf52840/main.cpp` is **not** touched by this task. It keeps its
existing `#ifdef ESP32` guards (which compile to no-ops for the
nRF52840 target). Migrating it to `Nrf52840PedalApp` and retiring the
shim duplication is T3 / TASK-283.

## Acceptance Criteria

- [ ] `src/pedal_app.h` exists, defines an abstract `PedalApp` class with
      pure-virtual `platformSetup()` / `platformLoop()` /
      `saveProfile()` / `loadProfile()`. Header has no `<Arduino.h>` and
      no hardware-specific includes.
- [ ] `lib/hardware/esp32/src/esp32_pedal_app.{h,cpp}` exists, defines
      `Esp32PedalApp : public PedalApp`, owns `BleConfigService`, and
      implements `saveProfile`/`loadProfile` via `Preferences` (NVS).
- [ ] `src/esp32/main.cpp` is reduced to a thin shim (≤ 10 lines) and
      contains zero `#ifdef ESP32` directives. `grep -c "#ifdef ESP32"
      src/esp32/main.cpp` returns 0.
- [ ] `pio run -e nodemcu-32s` builds clean — no new warnings.
- [ ] `make test-host` still passes — host build is unaffected.
- [ ] `pio run -e feather-nrf52840` still builds clean (nRF52840
      compiles `src/nrf52840/main.cpp` unchanged from T1).
- [ ] On-device smoke test on ESP32: device boots, advertises BLE,
      buttons fire actions, profile select works, profile is restored
      from NVS after power-cycle. No behaviour change vs. before this
      task.

## Test Plan

This is a refactor with no behaviour change. No new logic is added.

**Host tests** (`make test-host`):

- Verify the existing 286-test suite still passes. Code paths that
  were previously inside `#ifdef ESP32` (NVS, `BleConfigService`) are
  ESP32-only and were never compiled into the host build, so this
  reorganization should not affect host tests.

**On-device verification** (manual, ESP32 only):

- `pio run -e nodemcu-32s` builds successfully.
- Flash and confirm: BLE advertises, pairing works, button presses
  send the expected actions, profile-select switches profiles and
  saves to NVS, last-used profile is restored after reboot.

## Prerequisites

- **TASK-261** — moves `src/main.cpp` into per-target subfolders so
  this task can rewrite `src/esp32/main.cpp` independently of the
  nRF52840 entry point.

## Notes

- `lib/hardware/esp32/` is an existing PlatformIO library with its own
  `library.json`. Adding `esp32_pedal_app.{h,cpp}` here means the file
  is automatically excluded from the nRF52840 build via the existing
  `lib_ignore = HardwareESP32` line on nRF52840 envs.
- Globals currently at file scope in `main.cpp` (LED objects, button
  objects, `ProfileManager*`, `EventDispatcher`, `connected`,
  `longPressArmed[]`, ISR stubs) need a home. The cleanest split is:
  **owned by `PedalApp`** (button/LED arrays, dispatcher, profile
  manager, the connection-tracking flags) vs. **kept as file-scope in
  `esp32/main.cpp`** (the 26 IRAM_ATTR ISR stubs — they need to be
  free functions for `attachInterrupt`, and they bind to the
  `PedalApp` instance via a global pointer or static-singleton).
- `signalHardwareMismatch()` and `signalLoadError()` reference
  `ledPower`, `ledBluetooth`, `selectLeds`, `profileManager`. They
  belong to `PedalApp` since both ESP32 and (eventually) nRF52840
  need them. Move them as member functions.
- Do **not** introduce `BlePedalApp` in this task. Connection
  tracking, `process_events()`, `attachInterrupts`/`detachInterrupts`,
  the power-LED blink loop all stay in `PedalApp` for now and get
  pulled out into `BlePedalApp` in Phase 2.
- Reference: IDEA-028 §"Abstract `PedalApp` base class" and
  §"Concrete specializations in `lib/hardware/<target>/`".
