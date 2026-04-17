---
id: TASK-093
title: Add IO state change log panel to simulator
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
group: Simulator
order: 8
---

## Description

The simulator's output section has a **BLE Output** panel (keystrokes sent over BLE) and a
**Serial Output** panel (SerialOutputAction values). There is no visibility into GPIO pin
state changes caused by `PinHighAction`, `PinLowAction`, `PinToggleAction`,
`PinHighWhilePressedAction`, and `PinLowWhilePressedAction`.

Add a third **IO State** panel that logs pin transitions triggered by button actions, showing
the current simulated state of each pin that has been touched. This gives users who use the
pedal as a relay controller (not just a BLE keyboard) a way to verify their pin-action
mappings without real hardware.

The panel tracks **action-driven** state changes only — not firmware startup pin assignments
or hardware-level events. Its state represents "what would the pins look like right now based
on the actions triggered so far."

## Acceptance Criteria

- [ ] A third output panel **IO State** is added next to BLE Output and Serial Output
- [ ] Every time a `PinHighAction`, `PinLowAction`, or `PinToggleAction` fires, a log line
      is appended: e.g. `[A] Pin 5 → HIGH`
- [ ] `PinHighWhilePressedAction` logs `Pin 5 → HIGH` on press and `Pin 5 → LOW` on release
- [ ] `PinLowWhilePressedAction` logs `Pin 5 → LOW` on press and `Pin 5 → HIGH` on release
- [ ] A compact **Pin State Summary** at the top (or bottom) of the panel shows the current
      simulated level of each pin seen so far: e.g. `Pin 5: HIGH | Pin 6: LOW`
- [ ] The pin-state summary updates live as actions fire
- [ ] **Clear Output** button (existing) also clears the IO State log and resets the
      pin-state summary
- [ ] No regressions to BLE or Serial output panels

## Test Plan

1. Load a profile that contains a `PinHighAction` on button A (pin 5) and a `PinToggleAction`
   on button B (pin 5).
2. Press A → IO State logs "Pin 5 → HIGH"; summary shows "Pin 5: HIGH".
3. Press B → IO State logs "Pin 5 → LOW" (toggle from HIGH); summary updates.
4. Load a profile with `PinHighWhilePressedAction` on button C (pin 6).
5. Hold C → "Pin 6 → HIGH" logged; release → "Pin 6 → LOW" logged.
6. Click **Clear Output** → IO State panel clears; summary resets.

## Notes

This task is motivated by growing use of pin actions (IO triggers, relay boards) alongside
the BLE keyboard functionality. Users need to validate their IO mappings the same way they
validate BLE output — without hardware.

Keep the panel visually compact; the existing BLE and Serial panels are the primary outputs
for most users.
