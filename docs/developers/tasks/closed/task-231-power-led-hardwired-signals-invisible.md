---
id: TASK-231
title: Defect — Power LED hardwired to VCC; firmware error signals are invisible
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Small (<2h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
epic: feature_test
order: 19
---

## Description

On the current ESP32 pedal PCB, the green power LED is wired **directly to VCC**
rather than to a controllable GPIO. It is permanently on whenever the board is
powered, independent of firmware.

[data/config.json](../../../../data/config.json) nonetheless declares
`ledPower: 25`. The firmware treats pin 25 as a controllable output via
`LEDController` and drives it HIGH/LOW/blinking in several code paths — none of
which have any visible effect. Confirmed with the new LED hardware diagnostic
(`make test-esp32-leds`, TASK introducing it alongside this): driving pin 25
HIGH/LOW produces no visible state change on the green LED.

### Code paths that expect a controllable power LED but aren't visible

- [src/main.cpp:94-111](../../../../src/main.cpp#L94-L111) `signalHardwareMismatch()`
  — blinks power LED 3× fast on boot if the compiled hardware target does not
  match `config.json`. **Invisible** — the user gets no feedback and thinks the
  pedal simply didn't boot.
- [src/main.cpp:117-137](../../../../src/main.cpp#L117-L137) `signalLoadError()`
  — blinks ALL LEDs (including power) when config loading fails and the firmware
  falls back to the factory default. Power LED component is invisible; the blue
  and select LEDs still flash, so this is partially degraded rather than wholly
  invisible.
- [src/main.cpp:485-499](../../../../src/main.cpp#L485-L499) power-LED blink while
  a DelayedAction is in progress. **Invisible** — no "delayed action armed" cue.

### Consequences

- No user-visible cue on a boot-time hardware mismatch (safety-halt state).
- No cue that a DelayedAction is counting down.
- GPIO 25 is reserved but does nothing; cannot be reused on this PCB without a
  hardware or config change.

## Decisions to make

Three reasonable resolutions, need a product call:

1. **Accept the hardwired power LED as-is**, redirect all "power-LED" firmware
   signals to another LED (e.g. `ledBluetooth` pulsed distinctively, or a rapid
   burst on all three select LEDs). Treat GPIO 25 as reserved/unused in
   `config.json` (set to a sentinel value and make `LEDController` no-op on
   that value).
2. **Revise the PCB** to wire the power LED through GPIO 25 so firmware control
   is possible. Defer all software work until the PCB rev ships; document the
   current limitation.
3. **Split the role**: keep the green LED as a pure VCC indicator (permanent
   on) and repurpose GPIO 25 for a dedicated "activity/error" LED (would
   require a PCB change or off-board LED).

Option 1 is the lowest-effort software-only fix and preserves the current PCB.
Option 2 matches the original schematic intent.

## Acceptance Criteria

- [ ] Option chosen and documented.
- [ ] Boot-time hardware mismatch produces a user-visible signal.
- [ ] DelayedAction-in-progress produces a user-visible signal.
- [ ] Unit / on-device tests (as applicable) updated to assert the new
      observable signal rather than asserting state on a pin that does nothing.

## Test Plan

Depends on the chosen option:

- **Option 1** — Host tests for the signal redirection logic; on-device test
  that triggers the hardware-mismatch halt (e.g. flash firmware with a
  deliberately wrong `hardware` field in config.json) and confirms the chosen
  alternative LED pattern is visible.
- **Option 2** — Validate against a revised PCB with the new wiring; no new
  tests beyond a smoke run of the existing firmware.

## Notes

- Discovered during TASK-151 U-02 execution when the user ran the newly
  introduced `make test-esp32-leds` diagnostic to isolate "blue LED doesn't
  light" — turned out to be unrelated (see TASK-229 where the BLE
  connection-state detection was the real issue), but the diagnostic exposed
  this separate, pre-existing PCB/firmware mismatch.
- The same question applies on nRF52840 — check that pedal's PCB whether the
  power LED is also VCC-hardwired. If so, the same fix applies symmetrically.
