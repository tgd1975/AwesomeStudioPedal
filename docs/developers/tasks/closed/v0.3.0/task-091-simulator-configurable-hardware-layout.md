---
id: TASK-091
title: Make simulator hardware layout configurable (buttons and select LEDs)
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
group: Simulator
order: 6
---

## Description

The simulator is hard-coded to four buttons (A–D) and three profile-select LEDs. Builders
who wire up a different number of buttons or LEDs have no way to test their `profiles.json`
with the correct physical layout in the simulator.

Make the simulator's hardware layout match whatever is described in the loaded files:

1. **From a `config.json`** (produced by the configuration builder, TASK-088): read
   `numButtons` and `numSelectLeds` and render the correct number of button and LED widgets.
2. **From a loaded `profiles.json`**: infer `numButtons` from the maximum button slot used
   across all profiles (e.g. if profiles use A–F, render 6 buttons); `numSelectLeds` inferred
   from the number of profiles (minimum bits needed).
3. **Manual override in the toolbar**: a compact `Buttons: [1–26]` and `Select LEDs: [0–6]`
   control so the user can adjust without loading a file.

The button grid must re-render dynamically when any of the above changes.

## Acceptance Criteria

- [ ] **Load `config.json`** button added to the simulator toolbar (separate from the existing
      "Load profiles.json" picker)
- [ ] Loading a `config.json` updates `numButtons` and `numSelectLeds` and re-renders the
      button grid and select-LED strip accordingly
- [ ] Loading a `profiles.json` infers `numButtons` from the slots used and re-renders
- [ ] Manual `Buttons` and `Select LEDs` spinner/selects in the toolbar override the
      auto-detected values and re-render immediately
- [ ] Buttons are labelled A, B, C, … up to the configured count
- [ ] Select LEDs match `numSelectLeds`; the binary profile display updates correctly
- [ ] Profile cycling via SELECT wraps correctly for the configured number of LEDs
      (max profiles = 2^numSelectLeds − 1, minimum 1)
- [ ] Existing four-button/three-LED behaviour preserved when no changes are made
- [ ] No regressions to button-press, BLE output, or serial output panels

## Test Plan

1. Open simulator, set Buttons = 6, Select LEDs = 2 manually → verify 6 buttons (A–F) and
   2 select LEDs render.
2. Load a `profiles.json` that uses buttons A–F only → verify 6 buttons inferred.
3. Load a `config.json` with `numButtons: 2, numSelectLeds: 1` → verify 2 buttons and 1
   select LED render.
4. Press SELECT repeatedly → verify profile wraps at profile 1 (max = 2^1 − 1 = 1 with
   1 LED).
5. Load a profiles.json with 3 profiles while having 1 select LED → verify a warning is
   shown (only 1 profile reachable with 1 select LED).

## Notes

Depends on TASK-088 for the `config.json` format definition. The `Load config.json` button
can be added before TASK-088 is done, but the format must align once it is.

The manual override spinners are the highest-value part of this task — implement those
first so the task delivers value independently of TASK-088.
