---
id: TASK-239
title: Update wiring diagrams — add external pull-up resistors to button pins
status: closed
closed: 2026-04-24
opened: 2026-04-24
effort: Small (<2h)
effort_actual: Large (8-24h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Clarification
---

## Description

The generated circuit schematics at `docs/builders/wiring/esp32/main-circuit.svg`
and `docs/builders/wiring/nrf52840/main-circuit.svg` currently draw each button
as `GPIO → pushbutton → GND` with nothing else on the line. While the firmware
enables the MCU's internal pull-up (`pinMode(pin, INPUT_PULLUP)` in both
`lib/hardware/esp32/src/button.cpp` and `lib/hardware/nrf52840/src/button.cpp`),
the *wiring* the builder sees in the diagram implies the GPIO is only ever
connected to a momentary switch to ground — with a long cable run that line
effectively becomes a floating antenna, picking up noise that can cause spurious
presses despite the internal pull-up.

Update the schematic generator so every button pin is drawn with an explicit
external pull-up resistor to 3V3 (typical value 10 kΩ) in addition to the
switch-to-ground. The external resistor makes the intent unambiguous for anyone
hand-wiring a pedal and gives a much stronger pull than the MCU's internal
~45 kΩ pull-up, which materially reduces EMI susceptibility on long button runs.

## Acceptance Criteria

- [ ] `scripts/generate-schematic.py` draws a 10 kΩ resistor from each button
      GPIO pin to 3V3, alongside the existing switch-to-ground, for both ESP32
      and nRF52840 targets.
- [ ] Regenerated `docs/builders/wiring/esp32/main-circuit.svg` and
      `docs/builders/wiring/nrf52840/main-circuit.svg` show the new pull-up on
      every button pin (SELECT + BTN_A/B/C/D on ESP32; equivalent set on
      nRF52840) and still render without layout overlap.
- [ ] `docs/builders/BUILD_GUIDE.md` bill-of-materials / parts list mentions
      the pull-up resistors (quantity matches the button count per board) so
      builders know to source them.

## Test Plan

No automated tests required — change is to the schematic generator and the
generated SVG/docs. Verification is manual:

1. Run `python scripts/generate-schematic.py --target esp32` and `--target
   nrf52840`; open both SVGs and confirm each button pin now has a resistor to
   3V3 drawn next to the switch-to-ground branch.
2. Confirm the pre-commit WireViz/Schemdraw staleness hook (added in TASK-203)
   still passes after regeneration.

## Documentation

- `docs/builders/BUILD_GUIDE.md` — update the parts list to include 10 kΩ
  pull-up resistors (one per button pin, per board) and add a one-line note
  explaining why external pull-ups are used in addition to the MCU's internal
  pull-up.

## Notes

- The resistor value is a suggestion — 4.7 kΩ to 10 kΩ all work fine. Pick one
  value and stay consistent across both boards so the BOM is simple.
- Keep the firmware `INPUT_PULLUP` configuration as-is. Belt-and-braces is
  fine; removing the internal pull-up is out of scope and would make the pedal
  misbehave for anyone who built an earlier revision without external
  resistors.
- If the schematic layout gets crowded when the resistor is added on the same
  line as the button, consider stacking (resistor above, switch below) rather
  than shrinking font sizes.
