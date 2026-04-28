---
id: TASK-299
title: Phase 4d — consolidate host platform implementations under src/host/
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: XS (<30m)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: Support
epic: hal-refactor
order: 11
prerequisites: [TASK-298]
---

## Description

Make `host` a first-class platform peer alongside `esp32` and
`nrf52840`. Move every file under `test/fakes/` that is *actually*
a host implementation of a project interface into `src/host/` with
the same `include/` + `src/` layout the on-device targets use.

```
src/
  esp32/      include/  src/  main.cpp
  nrf52840/   include/  src/  main.cpp
  host/       include/  src/        ← no main.cpp; CMake-only consumer
```

PIO never sees `src/host/` — no env's `build_src_filter` mentions
it. CMake's host test build is the only consumer.

## Files moved

- `test/fakes/host_pedal_app.h`     → `src/host/include/host_pedal_app.h`
- `test/fakes/host_logger.cpp`      → `src/host/src/host_logger.cpp`
- `test/fakes/host_file_system.cpp` → `src/host/src/host_file_system.cpp`
- `test/fakes/hardware_config_fake.cpp` → `src/host/src/host_config.cpp`
  (rename to match the `config.cpp` naming used in
  `src/esp32/src/` and `src/nrf52840/src/`)

## Files that stay in `test/fakes/`

These are testing infrastructure, not host-platform implementations:

- `arduino_shim.{h,cpp}` and `Arduino.h` — Arduino framework
  shim for compiling on host
- `null_logger.h`, `null_led_controller.h` — generic null helpers
  (interface-agnostic, not host-specific)

## Acceptance Criteria

- [ ] The 4 files are moved (via `git mv`) to their new locations.
      `hardware_config_fake.cpp` is renamed to `host_config.cpp`.
- [ ] `test/CMakeLists.txt` updates:
  - `PROD_SOURCES` paths for `host_logger.cpp`, `host_file_system.cpp`,
    and the renamed `host_config.cpp`.
  - `target_include_directories` adds `src/host/include`.
- [ ] `make test-host` passes (287 tests).
- [ ] `pio run -e nodemcu-32s` and `pio run -e feather-nrf52840`
      build clean (no impact expected — they don't see
      `test/fakes/` or `src/host/`).

## Test Plan

Pure layout reorganization. No logic change.

**Host tests** (`make test-host`):

- The 287-test suite still passes after the move.

## Prerequisites

- **TASK-298** — established the `src/<target>/{include,src}/`
  convention. This task applies it to the third platform peer.

## Notes

- After this task, the `host` platform has the same shape as
  `esp32` and `nrf52840` (minus `main.cpp` — there is no PIO
  entry point for it).
- TASK-300 will then refactor `HostPedalApp` to be useful for
  end-to-end loop tests (currently inherits `PedalApp`; should
  inherit `BlePedalApp` and accept an injected `IBleKeyboard*`).
- The class-hierarchy diagram in `ARCHITECTURE.md` (Phase 5
  docs) will show `HostPedalApp` as the third peer; the
  `src/host/` layout makes that diagram match the filesystem.
