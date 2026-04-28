---
id: TASK-298
title: Phase 4c — mirror include/ + src/ layout under each target
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: XS (<30m)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: Support
epic: hal-refactor
order: 10
prerequisites: [TASK-297]
---

## Description

Cosmetic follow-up to TASK-296 / TASK-297. Each per-target folder
under `src/` already has an `include/` subdirectory; mirror that with
a `src/` subdirectory for the implementation files. Result:

```
src/
  esp32/
    include/        ← target-specific headers (already exists)
    src/            ← target-specific .cpp files (this task)
    main.cpp        ← 8-line entry-point shim (stays at module root)
  nrf52840/
    include/
    src/
    main.cpp
```

Rationale: the `include/` + `src/` pair mirrors the convention in
`lib/PedalLogic/`, makes each per-target folder self-documenting, and
clearly separates the entry-point shim (`main.cpp`) from the class
implementations. The cost is one extra path component per file —
"doppelt gemoppelt" — accepted as a clarity tax.

`build_src_filter = -<*> +<esp32/>` already recurses, so no env-level
filter change is needed for the production / pio-test envs. Only the
diagnostic envs that cherry-pick a single file need their paths
updated (`+<esp32/config.cpp>` → `+<esp32/src/config.cpp>` etc.).

## Acceptance Criteria

- [ ] `src/esp32/src/` contains the 8 ESP32 implementation files
      (ble_config_service, ble_keyboard_adapter, button,
      button_controller, config, esp32_file_system, esp32_pedal_app,
      led_controller).
- [ ] `src/nrf52840/src/` contains the 7 nRF52840 implementation
      files (ble_keyboard_adapter, button, button_controller, config,
      led_controller, nrf52840_file_system, nrf52840_pedal_app).
- [ ] Both `main.cpp` files stay at their respective module root
      (`src/esp32/main.cpp`, `src/nrf52840/main.cpp`).
- [ ] `nodemcu-32s-leds-test` and `feather-nrf52840-leds-test`
      `build_src_filter` lines update their `+<esp32/config.cpp>` /
      `+<nrf52840/config.cpp>` to point at the new path.
- [ ] `nodemcu-32s-ble-config-test` `build_src_filter`'s
      `-<esp32/main.cpp>` exclusion is unchanged (main.cpp stays
      at root).
- [ ] `test/CMakeLists.txt` updates the `button.cpp` path from
      `src/esp32/button.cpp` to `src/esp32/src/button.cpp`.
- [ ] `pio run -e nodemcu-32s` and `pio run -e feather-nrf52840`
      build clean.
- [ ] `make test-host` passes (287 tests).

## Test Plan

Pure layout reorganization, no logic change.

**Host tests** (`make test-host`):

- The 287-test suite still passes.

**On-device** — both production firmware images should be byte-
identical modulo build-path strings.

## Prerequisites

- **TASK-297** — completed Phase 4b. With both targets under `src/`,
  this is the symmetric polish step.

## Notes

- Header includes (`#include "ble_keyboard_adapter.h"` etc.) are
  unaffected — `-Isrc/<target>/include` is already on the build
  flags, so the implementation files find their headers through
  the same path regardless of the .cpp's own location.
- The single direct-framework-include block at the top of
  `src/nrf52840/src/ble_keyboard_adapter.cpp` (added in TASK-297
  for the LDF activation chain) carries along unchanged.
