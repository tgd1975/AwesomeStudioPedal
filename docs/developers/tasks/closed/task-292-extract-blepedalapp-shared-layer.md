---
id: TASK-292
title: Extract BlePedalApp shared layer (Phase 2)
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Medium (2-8h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 4
prerequisites: [TASK-289]
---

## Description

Phase 2 of EPIC-020. Introduce an intermediate `BlePedalApp` class
between `PedalApp` and the concrete `Esp32PedalApp` /
`Nrf52840PedalApp` subclasses. `BlePedalApp` captures everything that
is true for any BLE-based pedal regardless of MCU:

- holds the `IBleKeyboard*` and the `connected_` flag
- attaches / detaches interrupts on connect / disconnect transitions
- drives the Bluetooth LED on / off in lock-step with `connected_`
- owns `processEvents()` and the per-button long-press / double-press
  arming state (`longPressArmed_[]`)
- owns the main `loop()` body — connection tracking, action polling,
  power-LED blink for delayed actions

After this task, `PedalApp` is a leaner base that only knows about
hardware setup, profile manager, event dispatcher, and the
hardware-mismatch / load-error halt patterns. `Esp32PedalApp` and
`Nrf52840PedalApp` inherit from `BlePedalApp` and only override the
four platform hooks (`platformSetup`, `platformLoop`, `saveProfile`,
`loadProfile`).

This sets up Phase 3 cleanly: a future `HostPedalApp` can inherit
directly from `PedalApp` (no BLE, no interrupts, no `processEvents`)
without inheriting the BLE machinery it does not need.

## Acceptance Criteria

- [ ] `include/ble_pedal_app.h` exists; declares
      `class BlePedalApp : public PedalApp`.
- [ ] `src/ble_pedal_app.cpp` contains the moved logic (constructor,
      `setup`/`loop`, `attachInterrupts`/`detachInterrupts`,
      `processEvents`, ISR fan-out methods).
- [ ] `Esp32PedalApp` and `Nrf52840PedalApp` now inherit from
      `BlePedalApp`. Their constructors pass the BLE keyboard up to
      `BlePedalApp(...)`.
- [ ] `PedalApp` no longer holds `bleKeyboard_`, `connected_`, or
      `longPressArmed_[]`. `PedalApp::loop()` no longer exists (or is
      removed); the `loop()` lives on `BlePedalApp`.
- [ ] All four PIO env builds pass: `pio run -e nodemcu-32s`,
      `pio run -e feather-nrf52840`. No new warnings.
- [ ] `make test-host` still passes (286 tests).
- [ ] Loop-body symmetry: the body of `setup()` / `loop()` in
      `BlePedalApp` is byte-equivalent to the pre-refactor body
      modulo member-name renames. No behaviour change.

## Test Plan

This is a refactor with no behaviour change.

**Host tests** (`make test-host`):

- The 286-test suite still passes.

**On-device verification** (manual, both targets):

- ESP32 and nRF52840 both build cleanly.
- Flash each and confirm baseline behaviour: BLE advertises, pairing
  works, button presses send actions, profile-select works,
  delayed-action LED blink behaves as before.

## Prerequisites

- **TASK-289** — completes Phase 1 with both targets going through
  `PedalApp`. Phase 2 only makes sense once both subclasses exist.

## Notes

- IDEA-028 specifies `BlePedalApp` lives in either `src/` or
  `lib/PedalLogic/src/`. Since `pedal_app.cpp` already lives in
  `src/`, place `ble_pedal_app.cpp` alongside it for consistency.
- All build envs already have `+<pedal_app.cpp>` in their
  `build_src_filter`; add `+<ble_pedal_app.cpp>` next to it.
- The ISR fan-out method `onActionButtonInterrupt` /
  `onSelectButtonInterrupt` and the `g_pedalApp` global pointer
  must move to `BlePedalApp` because interrupts are
  `BlePedalApp`-managed. `g_pedalApp` becomes `g_blePedalApp` (or
  similar) — the type changes from `PedalApp*` to `BlePedalApp*`.
- Reference: IDEA-028 §"Intermediate shared layers".
