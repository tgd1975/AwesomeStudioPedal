---
id: TASK-186
title: Fix stale libdeps cache for local hardware libs in test environments
status: closed
closed: 2026-04-22
opened: 2026-04-21
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
---

## Description

The seven test environments in `platformio.ini` declare the local hardware libraries via
`file://lib/hardware/esp32` (and `nrf52840`) in `lib_deps`. PlatformIO copies these into
`.pio/libdeps/<env>/HardwareESP32` on first install and does not re-copy when source files
change — so any edit to `lib/hardware/` is silently ignored by test builds until the cache
is manually deleted. This has caused repeated build failures where the test env compiled
against a stale snapshot of `button.h`, `config.cpp`, etc.

The firmware envs (`nodemcu-32s`, `feather-nrf52840`) already avoid this by using
`lib_extra_dirs = lib/hardware` instead, which references files in-place with no caching.
The fix is to remove the `file://` entries from all test `lib_deps` and rely solely on
`lib_extra_dirs` for those envs too.

## Acceptance Criteria

- [ ] No test environment in `platformio.ini` has `file://lib/hardware/esp32` or
      `file://lib/hardware/nrf52840` in `lib_deps`
- [ ] All affected test envs resolve the hardware lib from `lib/hardware/` in-place via
      `lib_extra_dirs`
- [ ] `make test-esp32-serial`, `make test-esp32-button`, and `make test-esp32-profilemanager`
      build and pass after a source change to `lib/hardware/esp32/` without needing to
      delete `.pio/libdeps/`

## Test Plan

No automated tests required — change is non-functional (build config only).

Manually verify: edit any file in `lib/hardware/esp32/`, run a test env build, confirm the
change is picked up without deleting `.pio/libdeps/`.

## Notes

The `lib_ldf_mode = deep` setting used by several test envs may need to be kept or adjusted
— verify that removing `file://` does not break dependency scanning for Unity or NimBLE.
Also delete any existing `.pio/libdeps/*/HardwareESP32` and `.pio/libdeps/*/HardwareNRF52840`
stale caches as part of this task so they don't interfere after the fix.
