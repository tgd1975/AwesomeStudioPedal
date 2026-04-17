---
id: TASK-109
title: On-Device Multi-Press Integration Test (ESP32)
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
group: LongDoublePress
order: 6
prerequisites: [TASK-104]
---

## Description

Create a Unity on-device test suite for `Button` long-press and double-press detection running on real ESP32 hardware. Tests simulate button presses by directly manipulating the ISR and `millis()` context, and assert that the correct event flags are set.

## Acceptance Criteria

- [ ] `test/test_multipress_esp32/` directory created with a Unity test suite
- [ ] Test 1 — short press: ISR triggered, release before 500 ms → only `event()` fires, `holdDurationMs()` returns 0 after release
- [ ] Test 2 — long press: ISR triggered, release after 600 ms → `holdDurationMs() >= 500` during hold; flag set before release
- [ ] Test 3 — double press: two presses within 300 ms → `doublePressEvent()` returns `true`; `event()` does NOT also fire
- [ ] `make test-esp32-multipress` target added to `Makefile`
- [ ] Test requires: ESP32 (NodeMCU-32S) with button wired to a GPIO matching `builder_config.h`

## Files to Touch

- `test/test_multipress_esp32/` (new directory)
- `test/test_multipress_esp32/test_main.cpp` (new)
- `Makefile` — add `test-esp32-multipress` target

## Test Plan

**On-device tests** (`make test-esp32-multipress`):
- Requires ESP32 (NodeMCU-32S) connected via USB

## Prerequisites

- **TASK-104** — `Button` timing extensions must be in place before the on-device test can exercise them

## Notes

This task requires physical hardware. Mark as `[hardware-required]` in any CI documentation. Can be developed in parallel with TASK-105 through TASK-108 since it only depends on TASK-104.
