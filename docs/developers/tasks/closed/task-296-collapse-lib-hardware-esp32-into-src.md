---
id: TASK-296
title: Phase 4a — collapse lib/hardware/esp32 into src/esp32
status: closed
closed: 2026-04-29
opened: 2026-04-28
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 8
prerequisites: [TASK-295]
---

## Description

First slice of Phase 4 of EPIC-020. Move every file currently in
`lib/hardware/esp32/` into `src/esp32/`, and update the build so
PlatformIO finds the platform code via `build_src_filter` rather
than the deprecated `lib_extra_dirs` + `lib_ignore` combination.

The current layout is a historical artefact: `lib/hardware/esp32/`
existed as a "private library" so `lib_ignore = HardwareNRF52840`
could keep the wrong target out of ESP32 builds. With per-target
entry-point subfolders already in place under `src/<target>/`
(TASK-261), `build_src_filter` does the same job with a single
mechanism — and `lib_extra_dirs` is deprecated as of PIO 6.0
anyway.

After this task:

- `lib/hardware/esp32/` is gone.
- All ESP32 platform sources live under `src/esp32/` (alongside
  the existing 8-line `main.cpp` shim).
- ESP32-specific headers live under `src/esp32/include/`, added to
  each ESP32 env's `build_flags` via `-Isrc/esp32/include`.
- `nodemcu-32s` (and its 5 test envs) drop the
  `lib_extra_dirs = lib/hardware` line and the
  `lib_ignore = HardwareNRF52840` line.
- `pio run -e nodemcu-32s` builds clean.
- `make test-host` still passes — `test/CMakeLists.txt` updates the
  paths it lists for `button.cpp`, `host_logger.cpp` consumers, etc.

`lib/hardware/nrf52840/` is **not** touched in this task; that is
TASK-297. After T2 finishes, the entire `lib/hardware/` tree can be
removed and `lib_extra_dirs` deleted from every env in one sweep.

## Acceptance Criteria

- [ ] `lib/hardware/esp32/` no longer exists. Every file in
      `include/` and `src/` of that directory has been `git mv`-ed
      under `src/esp32/`. The `library.json` is deleted.
- [ ] `src/esp32/include/` contains the headers (`button.h`,
      `led_controller.h`, `ble_keyboard_adapter.h`,
      `ble_config_service.h`, `builder_config.h`,
      `button_controller.h`).
- [ ] `src/esp32/` (root) contains the implementation files that
      were under `lib/hardware/esp32/src/`, plus the existing
      `main.cpp`. (The `esp32_pedal_app.{h,cpp}` and
      `esp32_file_system.cpp` come along too — they were already
      under `lib/hardware/esp32/`.)
- [ ] Each of the 6 ESP32 envs in `platformio.ini` has its
      `build_flags` extended with `-Isrc/esp32/include`. The
      `lib_extra_dirs = lib/hardware` and
      `lib_ignore = HardwareNRF52840` lines are removed from those
      envs (still present on nRF52840 envs until T2).
- [ ] `pio run -e nodemcu-32s` builds clean — no new warnings.
- [ ] `pio run -e feather-nrf52840` still builds clean (untouched
      this task).
- [ ] `make test-host` still passes (287 tests). The
      `test/CMakeLists.txt` `target_include_directories` line
      updates `lib/hardware/esp32/include` to `src/esp32/include`,
      and any `PROD_SOURCES` entries that pointed at
      `lib/hardware/esp32/src/<file>.cpp` are repointed at
      `src/esp32/<file>.cpp`.

## Test Plan

This is a build-system reorganization with no logic change.

**Host tests** (`make test-host`):

- The 287-test suite still passes after the move.

**On-device** — ESP32 production firmware should be byte-identical
modulo build-path strings; flash and confirm baseline behaviour
(BLE advertises, buttons fire actions, profile-select works).

## Prerequisites

- **TASK-295** — completes Phase 3 (no more `HOST_TEST_BUILD`).
  Without that done, the move would have to also chase guard-defined
  filesystem fakes around. Phase 3 first makes Phase 4 mechanical.

## Notes

- `lib/PedalLogic/` is **not** in scope for this phase. It is
  genuinely shared code and stays as a real PIO library.
- Each ESP32-target test env's `build_src_filter` already includes
  `+<esp32/>` from TASK-261 — that line covers the moved files
  automatically. Nothing needs to change in those filters.
- `src/esp32/include/` is the convention picked to mirror
  `lib/hardware/esp32/include/`'s previous role. An alternative
  (single `src/esp32/` flat dir with headers next to .cpp) is
  possible but bigger-blast-radius.
- After this task, the `lib_extra_dirs` and `lib_ignore` lines
  remain on the nRF52840 envs for one more task. T2 (TASK-297)
  is the symmetric move that lets us delete `lib/hardware/`
  entirely and drop both options from every env.
- Reference: PIO docs note `lib_extra_dirs` is deprecated as of
  6.0 — see the conversation around the EPIC-020 reassessment.
