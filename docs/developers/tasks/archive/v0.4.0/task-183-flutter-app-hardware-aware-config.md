---
id: TASK-183
title: Flutter app — hardware-aware config editing and upload guard
status: closed
closed: 2026-04-20
opened: 2026-04-20
effort: Medium (2-8h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
epic: hardware-selector
order: 5
prerequisites: [TASK-179, TASK-180]
---

## Description

The Flutter app's `HardwareConfig` model, `PinField` widget, and upload screen are all
hardcoded to ESP32 assumptions:

- `HardwareConfig` has no `hardware` field — it cannot round-trip the value from
  `config.json` added in TASK-179
- `PinField` labels say "ESP32 GPIO (0–39)" and the tooltip references only ESP32 pin
  numbers
- `upload_screen.dart` uploads the config without checking whether the config targets
  the board the user is connected to

This task makes the app hardware-aware across all three layers.

## Acceptance Criteria

- [ ] `HardwareConfig` model has a `hardware` field (`String`, enum-validated to
  `"esp32"` / `"nrf52840"`); `fromJson` reads it, `toJson` writes it
- [ ] Wherever hardware config is created or edited in the app, a board selector widget
  is present (same two options as TASK-181); selecting a board updates `PinField` labels,
  valid range, and the help tooltip for that board
- [ ] `PinField` accepts a `BoardTarget` parameter (or equivalent) so it renders the
  correct label and tooltip; the hardcoded ESP32 strings are removed
- [ ] Before uploading hardware config via BLE, the app reads the connected device's
  hardware identity characteristic (added in TASK-180) and compares it to
  `hardwareConfig.hardware`; if they don't match, upload is blocked and a clear error
  dialog is shown
- [ ] `flutter analyze` passes with no errors or warnings after the changes
- [ ] Widget tests for `PinField` cover both board variants

## Test Plan

**Host tests** (`make flutter-test`):

- Add widget tests for `PinField` with `BoardTarget.esp32` and `BoardTarget.nrf52840`
  — verify correct label, range constraint, and tooltip text for each
- Add unit test for `HardwareConfig.fromJson` / `toJson` round-trip with `hardware` field
- Add unit test for the upload guard logic: matching hardware → proceeds; mismatched →
  error dialog shown

**On-device tests** (manual, requires physical hardware + phone):

- Connect app to a device; attempt to upload a config with the wrong `hardware` value;
  confirm the error dialog appears and no data is written
- Requires: ESP32 or nRF52840 with TASK-180 firmware, connected phone

## Prerequisites

- **TASK-179** — adds `hardware` to the schema; app model must round-trip this field
- **TASK-180** — firmware exposes device hardware identity over BLE; app queries this before upload

## Documentation

- `app/lib/widgets/pin_field.dart` — remove the hardcoded ESP32 reference from the public
  API; update any usage docs or inline comments
- `docs/builders/` — update app walkthrough to mention the board selector in the config
  editing flow

## Implementation Status

Code changes are written but **unverified** — `flutter analyze` and `flutter test` require
the devcontainer (Linux). Must reopen in devcontainer before closing this task.

**Done (needs verification):**

- `BoardTarget` enum and `boardTargetFromString()` added to `hardware_config.dart`
- `HardwareConfig` has `hardware` field; `fromJson`/`toJson` updated
- `PinField` accepts `board: BoardTarget` — ESP32 and nRF52840 labels/ranges/tooltips
- `ActionEditorScreen` has `board` parameter; all call sites pass `boardTarget` from `ProfilesState`
- `BleService` discovers `kHwIdentityUuid` characteristic and exposes `readDeviceHardware()`
- `upload_screen.dart` reads device hardware and blocks upload on mismatch
- Unit tests in `test/unit/hardware_config_test.dart` (8 tests)
- Widget tests in `test/widget/pin_field_test.dart` (3 tests)

**Still needed:**

- Run `flutter analyze` in devcontainer — fix any errors
- Run `flutter test` in devcontainer — verify all tests pass
- On-device manual test (hardware mismatch dialog)
- Close this task

## Notes

- Human-in-loop is `Support` for the on-device verification step
- The board selector in the app can be a simple `DropdownButtonFormField` reusing the
  existing `InputDecoration` style — no custom widget needed
- `BoardTarget` can be a simple Dart enum: `enum BoardTarget { esp32, nrf52840 }`
- TASK-164 (design implementation) is a visual-only refactor and does not conflict with
  this task, but coordinate so the board selector follows the design tokens
- If the connected device does not expose the hardware characteristic (pre-TASK-180
  firmware), show a warning snackbar rather than a hard block
