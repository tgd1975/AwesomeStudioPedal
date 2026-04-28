---
id: TASK-103
title: On-Device GPIO Testrig for PinAction (ESP32)
status: closed
closed: 2026-04-21
opened: 2026-04-17
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
epic: IOTestrig
order: 3
prerequisites: [TASK-102]
---

## Description

Create a Unity on-device test suite that exercises all five `PinAction` modes against real GPIO hardware. Uses the NodeMCU-32S onboard LED (GPIO 2, active-high, unassigned in `builder_config.h`) as the test output — no jumper or logic analyser needed. Each mode is verified by `digitalRead(2)` assertions.

## Acceptance Criteria

- [ ] `test/test_pin_io_esp32/` directory created with a Unity test suite
- [ ] All five `PinAction` modes exercised: `PinHigh`, `PinLow`, `PinToggle`, `PinHighWhilePressed`, `PinLowWhilePressed`
- [ ] Assertions use `digitalRead(2)` to verify pin state after `execute()` and `executeRelease()`
- [ ] `make test-esp32-pin-io` target added to `Makefile`
- [ ] Test file header documents the active-high polarity assumption and the porting note for active-low boards

## Test Plan

**On-device tests** (`make test-esp32-pin-io`):

- New `test/test_pin_io_esp32/test_main.cpp` using Unity
- Requires: ESP32 (NodeMCU-32S) connected via USB

## Prerequisites

- **TASK-102** — `getJsonProperties` completes the `PinAction` surface; testrig tests the full class

## Notes

The NodeMCU-32S onboard LED is on GPIO 2, active-high. If ported to a board with an active-low LED the `PinLow` / `PinLowWhilePressed` cases will be the visually confirming ones — document this in the test file header. This is the only hardware-required task in Group A.
