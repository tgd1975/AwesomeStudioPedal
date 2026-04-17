---
id: TASK-077
title: Add pin operation action types (high, low, toggle, high/low while pressed)
status: closed
closed: 2026-04-16
opened: 2026-04-16
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
---

## Description

The pedal currently supports only BLE keyboard and serial output actions. Adding GPIO pin
operations unlocks direct hardware control — driving relay coils, toggling effect-loop switches,
or signalling external devices without a Bluetooth host.

Five new action modes are required:

| Mode | Behaviour |
|---|---|
| `PinHighAction` | Sets the pin HIGH on press. |
| `PinLowAction` | Sets the pin LOW on press. |
| `PinToggleAction` | Toggles the pin state on each press. |
| `PinHighWhilePressed` | Sets pin HIGH on press; restores LOW on release. |
| `PinLowWhilePressed` | Sets pin LOW on press; restores HIGH on release. |

All five must be non-blocking: no `delay()` inside `execute()`. The "while pressed" modes drive
the release side via the trigger-system extension tracked in TASK-078; this task implements the
action objects themselves and wires them into the config loader and JSON schema.

Implementation steps:

1. Add `PinHigh`, `PinLow`, `PinToggle`, `PinHighWhilePressed`, `PinLowWhilePressed` to
   `Action::Type` in `lib/PedalLogic/include/action.h`.
2. Create `lib/PedalLogic/include/pin_action.h` and
   `lib/PedalLogic/src/pin_action.cpp` with a concrete `PinAction` class (or five small
   subclasses) that calls `digitalWrite()` in `execute()`. Use `#ifndef HOST_TEST_BUILD` / shim
   pattern already established in `delayed_action.cpp`.
3. Extend `ConfigLoader::createActionFromJson()` in
   `lib/PedalLogic/src/config_loader.cpp` to recognise the five new type names
   (e.g. `"PinHighAction"`, `"PinHighWhilePressedAction"`, …).
4. Add the pin number as a mandatory `"pin"` field in the JSON button config.
5. Update `ProfileManager::getActionTypeString()` and `DelayedAction::getTypeName()` for the new
   types.

## Acceptance Criteria

- [ ] All five modes compile for both the ESP32 target and the host test build.
- [ ] `PinHigh/Low/Toggle` correctly calls `digitalWrite()` with the configured pin and state.
- [ ] `PinHighWhilePressed` and `PinLowWhilePressed` store their "release" target state so TASK-078
      can invoke it on button release without knowing the action type.
- [ ] `ConfigLoader` parses the five new type names from JSON; unknown pin values log an error and
      return `nullptr`.
- [ ] Unit tests cover construction and `execute()` for each of the five modes using a fake GPIO
      shim.

## Notes

For the host test build, `digitalWrite()` must be shimmed in `test/fakes/arduino_shim.h` (already
contains `millis()` and similar stubs).

The "while pressed" release state should be exposed via a virtual method on `Action`
(e.g. `virtual void executeRelease() {}`), defaulting to no-op, so the trigger system (TASK-078)
can call it uniformly without casting.

JSON example:

```json
"A": {"type": "PinHighWhilePressedAction", "name": "Relay", "pin": 22}
```
