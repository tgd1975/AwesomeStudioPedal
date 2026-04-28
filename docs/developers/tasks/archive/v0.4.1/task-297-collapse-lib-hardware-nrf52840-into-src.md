---
id: TASK-297
title: Phase 4b — collapse lib/hardware/nrf52840 into src/nrf52840; delete lib/hardware/
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 9
prerequisites: [TASK-296]
---

## Description

Symmetric companion to TASK-296. Move every file from
`lib/hardware/nrf52840/` into `src/nrf52840/` and delete `lib/hardware/`
entirely. Drop `lib_extra_dirs = lib/hardware` (deprecated as of
PIO 6.0) and `lib_ignore = HardwareESP32` from every nRF env in
`platformio.ini`.

After this task:

- `lib/hardware/` directory no longer exists.
- All nRF52840 platform sources live under `src/nrf52840/`.
- Every env in `platformio.ini` is free of `lib_extra_dirs` and
  `lib_ignore` lines for hardware libs.

## Outcome — done

Closed successfully on the second attempt. The first attempt was
reverted after `pio run -e feather-nrf52840` failed to find
`bluefruit.h`; the cause and fix are documented below for future
reference.

### What broke on the first attempt

PIO's Library Dependency Finder (LDF) in default `chain` mode only
follows direct `#include` lines from `.cpp` files. It does **not**
recurse into headers' transitive includes. The pre-move layout had
this structure:

```cpp
// lib/hardware/nrf52840/src/ble_keyboard_adapter.cpp
#include "ble_keyboard_adapter.h"
// (uses Bluefruit. via the header)

// lib/hardware/nrf52840/include/ble_keyboard_adapter.h
#include <bluefruit.h>     // ← only place bluefruit.h appears
```

When this code was a private library, PIO's LDF scanned the library's
own sources differently — it picked up `<bluefruit.h>` from the header
chain and activated the framework-bundled Bluefruit library. Once the
files moved to `src/`, that activation chain stopped firing.

### What fixes it

Compare to ESP32 (which works clean from `src/`): every framework lib
is referenced by a *direct* `.cpp` `#include`. For example:

- `src/esp32/esp32_pedal_app.cpp` directly: `#include <Preferences.h>`
- `src/esp32/esp32_file_system.cpp` directly: `#include <LittleFS.h>`

So the LDF auto-activates `Preferences` and `LittleFS` from those
`.cpp` files. The nRF case lacked an equivalent direct-include site
for Bluefruit / nRFCrypto.

The fix is one block at the top of `src/nrf52840/ble_keyboard_adapter.cpp`:

```cpp
// Direct framework-bundled includes so PIO's LDF activates the
// libraries from src/. The LDF in chain mode follows only direct .cpp
// #include lines; without these, Bluefruit and nRFCrypto would only
// be referenced transitively via ble_keyboard_adapter.h and stay
// unactivated.
#include <Adafruit_nRFCrypto.h>
#include <bluefruit.h>

#include "ble_keyboard_adapter.h"
```

`Adafruit_LittleFS` and `InternalFileSystem` already had a direct
.cpp include in `nrf52840_file_system.cpp`, so they activated without
help.

After this fix, `pio run -e feather-nrf52840` builds cleanly and the
dependency graph shows all four framework-bundled libraries activated:

```
Dependency Graph
|-- ArduinoJson @ 6.21.6
|-- PedalLogic @ 1.0.0
|-- Adafruit nRFCrypto
|-- Adafruit Bluefruit nRF52 Libraries
|-- Adafruit Little File System Libraries @ 0.11.0
|-- Adafruit Internal File System on Bluefruit nRF52 @ 0.11.0
```

## Acceptance Criteria

- [x] `lib/hardware/` no longer exists. `ls lib/` shows only `PedalLogic/`.
- [x] `src/nrf52840/include/` contains the 6 nRF headers; `src/nrf52840/`
      contains the 7 .cpp files plus `main.cpp`.
- [x] All 4 nRF envs in `platformio.ini` had `lib_extra_dirs` and
      `lib_ignore` removed; `-Isrc/nrf52840/include` added to
      `build_flags`. The `feather-nrf52840-leds-test` env was also
      updated to point its custom `build_src_filter` at the new path.
- [x] `pio run -e nodemcu-32s` builds clean (~15s).
- [x] `pio run -e feather-nrf52840` builds clean (~13s).
- [x] `make test-host` passes (287 tests).
- [x] **Phase 4 milestone:** `grep -n "lib_extra_dirs\|lib_ignore"
      platformio.ini` returns only the comment line in the leds-test
      env description — no active config uses either option.
- [ ] On-device smoke test on nRF52840 — manual; verify BLE pairing,
      button actions, profile switch.

## Test Plan

Build-system reorg with no logic change. Host tests confirm the move
did not affect shared logic; production builds confirm the LDF
activation chain works post-move.

## Prerequisites

- **TASK-296** — moved the ESP32 half. The diagnosis above came from
  comparing the working ESP32 dep graph to the failing nRF one.

## Notes — for future maintainers

- **PIO LDF gotcha:** if a framework-bundled library is referenced
  only via a header chain (i.e. `.cpp` includes a project header
  that includes the framework header), the LDF in default `chain`
  mode will not activate it. The fix is a direct `#include` at the
  top of one of the `.cpp` files. `lib_ldf_mode = deep` / `deep+`
  are documented to recurse, but in practice did not help for this
  specific case (transitive header chain crossing project-relative
  → angle-bracket include style).

- **Why `lib_extra_dirs` / `lib_ignore` were originally needed:** the
  pre-EPIC-020 layout used `lib/hardware/<target>/` as a private PIO
  library. `lib_extra_dirs = lib/hardware` told PIO to scan that
  directory for libraries; `lib_ignore` excluded the wrong-target
  lib for each env. With the platform code moved into `src/<target>/`
  and `build_src_filter` doing per-target source selection, both
  options become redundant.

- **`lib/PedalLogic/` stays.** It is genuinely shared, has a clear
  identity as the platform-agnostic core, and benefits from PIO's
  library mechanism (auto-discovered, separate compilation,
  independent include path). Exactly what `lib/` is for.

- **Reference:** the conversation thread that surfaced this — search
  for "lib_extra_dirs deprecation" / "PIO 6.0" / "framework-bundled
  library activation".
