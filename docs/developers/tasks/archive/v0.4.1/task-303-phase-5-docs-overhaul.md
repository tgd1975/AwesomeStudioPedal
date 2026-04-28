---
id: TASK-303
title: Phase 5 — docs overhaul to match the post-EPIC-020 architecture
status: closed
opened: 2026-04-28
closed: 2026-04-29
effort: Medium (2-8h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 15
prerequisites: [TASK-302]
---

## Description

Final phase of EPIC-020. Rewrite the developer documentation to match
the architecture and layout established in Phases 1–4. The docs that
need updating still describe the pre-refactor world:

- Class hierarchy was implicit (`main.cpp` at the centre, hardware
  classes called directly). Now there is an explicit
  `PedalApp` → `BlePedalApp` → concrete subclass tree, with `HostPedalApp`
  as a third platform peer.
- Hardware code lived under `lib/hardware/<target>/`. It now lives
  under `src/<target>/{include,src}/` (except for nRF52840's
  framework-bundled BLE libs gotcha — see TASK-297 post-mortem).
- `HOST_TEST_BUILD` preprocessor flag was the host-test gate. It is
  gone; host tests link against `HostPedalApp` + `host_logger.cpp` +
  `host_file_system.cpp` + `host_config.cpp` chosen at the CMake
  layer, not via `#ifdef`.
- Integration tests can now drive `BlePedalApp::loop()` end-to-end
  on host using `MockBleKeyboard` + per-pin `fake_gpio` — that
  capability didn't exist before.

## Files to update

- `docs/developers/ARCHITECTURE.md`
  - Replace the high-level component diagram with a `PedalApp`
    class-hierarchy diagram. Keep the action class hierarchy
    section as-is.
  - Update the component table: paths move from
    `lib/hardware/esp32/` to `src/esp32/{include,src}/`. Add rows
    for `PedalApp`, `BlePedalApp`, `Esp32PedalApp`,
    `Nrf52840PedalApp`, `HostPedalApp`.
  - Replace the "Hardware abstraction seam" section: drop the
    `HOST_TEST_BUILD` paragraph; describe the post-refactor layout
    (link-time platform selection via CMake, `IFileSystem` /
    `IBleKeyboard` / `ILogger` / `ILEDController` / `IButtonController`
    interfaces).
  - Add a brief note on the asymmetry between ESP32 (NimBLE via
    `lib_deps` registry) and nRF52840 (framework-bundled libs need
    a direct `.cpp` `#include <bluefruit.h>` to activate the LDF).

- `docs/developers/TESTING.md`
  - Drop the `HOST_TEST_BUILD` paragraph entirely. Replace with a
    short note: "Host tests link against `HostPedalApp` and the
    host-side platform implementations under `src/host/`. No
    preprocessor flag is involved."
  - Add a "Writing an integration test" section that shows the
    `HostPedalApp(mockKb)` + `setup()` + `loop()` pattern from
    `test/unit/test_app_integration.cpp`.
  - Update the "Two test worlds" section to mention that
    integration tests (running the full `BlePedalApp::loop()`) are
    now possible on host.
  - Remove the stale `MockButtonController` row from the mock
    table if it no longer exists. Mention the per-pin
    `fake_gpio::setPinState` API.

- `docs/developers/ADDING_HARDWARE.md`
  - Rewrite the "Adding a new microcontroller target" section
    around the new layout. New steps:
    1. Add `[env:new-board]` to `platformio.ini`.
    2. Create `src/<target>/{include,src}/` with `main.cpp` shim,
       `<target>_pedal_app.{h,cpp}`, `<target>_file_system.cpp`,
       `ble_keyboard_adapter.{h,cpp}`, `button.{h,cpp}`,
       `button_controller.{h,cpp}`, `led_controller.{h,cpp}`,
       `config.cpp`, `builder_config.h`.
    3. Add `build_src_filter = -<*> +<<target>/> +<pedal_app.cpp>
       +<ble_pedal_app.cpp>` and `-Isrc/<target>/include` to the
       env's `build_flags`.
    4. If the framework provides BLE / persistence libs that are
       *not* in the PlatformIO registry, add a direct `#include
       <theirheader.h>` to one of your `.cpp` files so the LDF
       activates them (see `src/nrf52840/src/ble_keyboard_adapter.cpp`).
    5. Add Make targets and an on-device test env.
  - Drop all references to `lib_ignore`, `lib_extra_dirs`, and
    `lib/hardware/`.

- `docs/developers/CI_PIPELINE.md`
  - Update the Doxygen input list: `lib/hardware/esp32` /
    `lib/hardware/nrf52840` → `src/esp32` / `src/nrf52840` /
    `src/host`.

- `Doxyfile`
  - Same input-list update.
  - Drop `HOST_TEST_BUILD` from `PREDEFINED` (no longer used
    anywhere in the codebase).

- Inline code comments
  - Run `grep -rn "HOST_TEST_BUILD\|lib/hardware\|#ifdef ESP32"
    src/ lib/ include/` (excluding `.git`/`build`) and clean up
    any stale references in code comments.

## Acceptance Criteria

- [ ] Each of the four .md files above is updated; no remaining
      references to `lib/hardware/`, `lib_extra_dirs`, `lib_ignore`,
      or `HOST_TEST_BUILD` outside of historical task records.
- [ ] `Doxyfile` INPUT list updated; `HOST_TEST_BUILD` dropped from
      `PREDEFINED`.
- [ ] Inline-comment sweep done: `grep -rn "HOST_TEST_BUILD\|
      lib/hardware\|#ifdef ESP32" src/ lib/ include/` returns no
      stale rationale.
- [ ] `make test-host` still passes (291 tests).
- [ ] Both production builds still pass.

## Test Plan

Documentation-only change. No new tests.

**Host tests** (`make test-host`):

- The 291-test suite still passes — pure docs/comment edits cannot
  affect compiled behaviour.

**Lint check:**

- `make lint` (or `/lint`) reports no new markdown-lint regressions
  in the changed files.

## Prerequisites

- **TASK-302** — closes the implementation half of EPIC-020. With
  the integration-test scaffolding in place, the docs can describe
  it accurately rather than as future work.

## Notes

- This is a "make the docs match reality" task, not a "rewrite
  everything from scratch" task. Preserve the existing structure
  and tone where possible; only rewrite sections that drifted.
- The TASK-297 post-mortem is the canonical reference for the
  `lib/hardware/` history; ARCHITECTURE.md / ADDING_HARDWARE.md
  should link to it where relevant rather than re-tell the story.
- Do **not** touch `FEATURE_TEST_PLAN.md` or `BLE_CONFIG_*.md` —
  those are out of scope.
- After this task, EPIC-020 is fully done.
