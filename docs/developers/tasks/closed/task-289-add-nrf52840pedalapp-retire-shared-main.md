---
id: TASK-289
title: Add Nrf52840PedalApp, retire shared main.cpp
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 3
prerequisites: [TASK-282]
---

## Description

Complete Phase 1 of EPIC-020 by introducing `Nrf52840PedalApp` and
reducing `src/nrf52840/main.cpp` to a shim — symmetric with what
TASK-282 did for the ESP32 side. After this task, no `#ifdef ESP32`
guards remain anywhere under `src/`.

Concretely:

- Add `lib/hardware/nrf52840/include/nrf52840_pedal_app.h` and
  `lib/hardware/nrf52840/src/nrf52840_pedal_app.cpp` with a
  `Nrf52840PedalApp : public PedalApp` class.
- Implement `platformSetup()` / `platformLoop()` as no-ops (no
  `BleConfigService` on this target).
- Implement `saveProfile()` / `loadProfile()` as stubs (no NVS
  available). `saveProfile` is a no-op; `loadProfile` returns 0.
- Construct via `PedalApp(createBleKeyboardAdapter())` — the
  nRF52840 hardware library already exposes that factory.
- Reduce `src/nrf52840/main.cpp` to the same 8-line shim pattern as
  `src/esp32/main.cpp` — include header, declare static instance,
  define `setup()`/`loop()` as forwarders.
- Update the four nRF52840 envs in `platformio.ini` to add
  `+<pedal_app.cpp>` to their `build_src_filter`, mirroring what
  ESP32 envs already have.

After this task, Phase 1 of EPIC-020 is complete and the codebase
moves into Phase 2 (extract `BlePedalApp` shared layer).

## Acceptance Criteria

- [ ] `lib/hardware/nrf52840/include/nrf52840_pedal_app.h` and
      `lib/hardware/nrf52840/src/nrf52840_pedal_app.cpp` exist.
- [ ] `src/nrf52840/main.cpp` is reduced to a thin shim (≤ 10 lines)
      and contains zero `#ifdef ESP32` directives.
- [ ] **Phase 1 milestone:** `grep -r "#ifdef ESP32" src/` returns
      zero hits.
- [ ] `pio run -e feather-nrf52840` builds clean — no new warnings.
- [ ] `pio run -e nodemcu-32s` still builds clean.
- [ ] `make test-host` still passes (286 tests).
- [ ] On-device smoke test on nRF52840: device boots, advertises
      BLE, buttons fire actions, profile-select switches profiles.
      No behaviour change vs. before this task. (Profile is **not**
      restored after reboot — stub `loadProfile` always returns 0;
      this matches pre-refactor behaviour where `saveCurrentProfile`
      was already a no-op on nRF52840.)

## Test Plan

This is a refactor with no behaviour change. No new logic.

**Host tests** (`make test-host`):

- The 286-test suite still passes.

**On-device verification** (manual, nRF52840 only):

- `pio run -e feather-nrf52840` builds successfully.
- Flash and confirm: BLE advertises, pairing works, button presses
  send the expected actions, profile-select cycles profiles.

## Prerequisites

- **TASK-282** — introduces the `PedalApp` base class and the
  `+<pedal_app.cpp>` build_src_filter pattern that this task
  applies to nRF52840 envs.

## Notes

- nRF52840 has no NVS-equivalent storage in the current code (the
  pre-refactor `saveCurrentProfile` was already a no-op stub for
  this target). Proper persistence on nRF52840 is out of scope for
  Phase 1; track it as a separate idea/task if needed.
- `lib/hardware/nrf52840/library.json` already declares the source
  directory, so the new `nrf52840_pedal_app.cpp` is picked up
  automatically once `lib_ignore = HardwareESP32` keeps the wrong
  target out.
- Reference: IDEA-028 §"Concrete specializations in
  `lib/hardware/<target>/`" (Nrf52840PedalApp row).
