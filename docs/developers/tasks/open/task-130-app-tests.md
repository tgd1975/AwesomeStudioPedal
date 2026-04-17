---
id: TASK-130
title: App Unit, Widget, and Integration Tests
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: MobileApp
order: 9
prerequisites: [TASK-123]
---

## Description

Comprehensive Flutter test suite covering unit, widget, and integration layers. Tests use `mockito` for `BleService` in widget and integration tests to avoid requiring real hardware.

## Acceptance Criteria

### Unit tests (`app/test/unit/`)
- [ ] `ActionConfig.fromJson` / `toJson` round-trips for every action type (SendKey, SendMediaKey, SendString, PinHigh, PinLow, PinToggle, PinHighWhilePressed, PinLowWhilePressed, Delayed, Macro)
- [ ] `SchemaService.validateProfiles` accepts `data/profiles.json`; rejects profile with missing `type` field
- [ ] `BleService.uploadProfiles` chunking: 1 500-byte payload produces 3 chunks with correct 2-byte big-endian sequence numbers

### Widget tests (`app/test/widget/`)
- [ ] `ActionTypeDropdown` renders all action types; selecting "Macro" shows `MacroStepList`
- [ ] `ActionEditorScreen` for `DelayedAction` shows nested action editor
- [ ] Validation banner shows red error count when invalid profile is loaded

### Integration test (`app/test/integration/`)
- [ ] App launches, `ScannerScreen` appears, mock `BleService` injects a fake scan result
- [ ] Tapping "Connect" navigates to `HomeScreen`
- [ ] Editing a profile and tapping upload calls `BleService.uploadProfiles` with valid JSON

### General
- [ ] `flutter test` passes with no failures
- [ ] `flutter analyze` passes

## Files to Touch

- `app/test/unit/` (new directory + test files)
- `app/test/widget/` (new directory + test files)
- `app/test/integration/` (new directory + test file)

## Test Plan

`flutter test` in `app/` — all cases pass.

## Prerequisites

- **TASK-123** — Flutter project must exist; individual tests should be added as their target feature (TASK-124 through TASK-128) is completed, but this task consolidates the full suite
