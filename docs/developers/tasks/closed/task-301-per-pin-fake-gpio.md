---
id: TASK-301
title: Phase 4f — per-pin fake_gpio for multi-button host tests
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 13
prerequisites: [TASK-300]
---

## Description

Today `test/fakes/arduino_shim.h` exposes a single global
`fake_gpio::pin_state` — one int representing the state of "the
button." That works for unit tests that exercise one button at a
time, but is not enough for end-to-end pedal tests where multiple
buttons interact (e.g. a long-press on A while B is held).

Replace the singletons with a per-pin map:

```cpp
namespace fake_gpio
{
    void setPinState(uint8_t pin, int state);
    int  getPinState(uint8_t pin);                  // default LOW
    int  lastWrittenPin();
    int  lastWrittenValue();
    void reset();                                   // clear all state
}
```

`digitalRead(pin)` reads the per-pin state; `digitalWrite(pin, val)`
records both into per-pin storage AND continues to update the
last-written pair (preserves existing test semantics).

Existing tests that rely on the singleton API
(`fake_gpio::pin_state = LOW`) keep working — the shim provides a
thin compatibility layer that aliases pin 0 (or all pins) until
those tests are migrated. Final removal of the compatibility shim
is out of scope.

## Acceptance Criteria

- [ ] `arduino_shim.h` exposes the per-pin API above.
- [ ] `arduino_shim.cpp` backs it with `std::unordered_map<uint8_t, int>`
      (or simple array of size 64).
- [ ] `digitalRead(pin)` returns the per-pin state, default `LOW`.
- [ ] `digitalWrite(pin, value)` updates per-pin state AND the
      legacy `last_written_pin` / `last_written_value` globals.
- [ ] Existing tests pass without modification — the legacy
      `fake_gpio::pin_state` global continues to work for tests
      that haven't been migrated (acts as pin 0).
- [ ] `make test-host` passes (287 tests).

## Test Plan

**Host tests** (`make test-host`):

- All 287 existing tests still pass — no test changes required.
- Optionally: a tiny sanity test that two pins can hold
  independent states, e.g. `setPinState(2, HIGH); setPinState(3, LOW);
  EXPECT_EQ(digitalRead(2), HIGH); EXPECT_EQ(digitalRead(3), LOW);`.

## Prerequisites

- **TASK-300** — `HostPedalApp` ready for end-to-end tests; per-pin
  GPIO is the missing piece for multi-button scenarios.

## Notes

- The per-pin storage is reset between tests via `fake_gpio::reset()`
  in test fixtures' `SetUp()`. Existing tests that don't reset will
  still work because they use only pin 0 (the legacy alias).
- TASK-302 will be the first consumer of the new per-pin API.
