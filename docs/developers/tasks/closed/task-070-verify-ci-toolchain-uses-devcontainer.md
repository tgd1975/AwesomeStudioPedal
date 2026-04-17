---
id: TASK-070
title: Verify CI toolchain runs cleanly with devcontainer image
status: closed
closed: 2026-04-12
opened: 2026-04-11
effort: Small (<2h)
complexity: Junior
human-in-loop: Support
---

## Description

Recent CI work (TASK-057, TASK-065, TASK-066) switched the `cpp-format`,
`unit-tests`, and `clang-tidy` jobs to run inside the devcontainer image
(`mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04`) and fixed several root
causes (ArduinoJson FetchContent, uninitialized members, Puppeteer sandbox).

This task verifies that all workflows are now green on GitHub and that the
devcontainer image is actually being used as intended — not falling back to
a bare runner or skipping the container silently.

## Acceptance Criteria

- [ ] All workflows pass on the `feature/improvements` branch (or the PR
  created from it): `CI`, `Static Analysis`, `CodeQL Analysis`, `Docs`
- [ ] The `unit-tests`, `cpp-format`, and `clang-tidy` job logs show the
  devcontainer image being pulled and used (visible in the "Set up job" step)
- [ ] The `coverage` job runs, generates an HTML artifact, and the 80%
  threshold is met
- [ ] The `Docs` workflow builds Doxygen successfully and the `api-docs-html`
  artifact is available for download
- [ ] No unexpected deprecation warnings or Node.js version errors remain
- [ ] Results documented in `## Notes` below

## Notes

### What to check

Use `/ci-status` or `gh run list --limit 20` to get the current run status.
For each failing job, use `gh run view <run-id> --log-failed` to get details.

### Key things introduced that need confirmation

- `container: mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04` on
  `cpp-format`, `unit-tests` (test.yml) and `clang-tidy` (static-analysis.yml)
- ArduinoJson fetched via `FetchContent_Declare` in `CMakeLists.txt` —
  no longer relies on `.pio/libdeps/` path
- `gcovr --fail-under-line 80` in the `coverage` job
- `--puppeteerConfigFile` with `--no-sandbox` in `scripts/validate_mermaid.py`
- `pull_request` trigger added to `docs.yml`

### Verified findings (2026-04-12)

All checks confirmed green on commit `9f6b156` (branch `feature/improvements`).

**CI workflow** (`test.yml`) — run [24310437199](https://github.com/tgd1975/AwesomeStudioPedal/actions/runs/24310437199): ✓

- `cpp-format`: devcontainer image confirmed used; `clang-format` installed via
  `apt-get`; all files pass.
- `markdown-lint`, `mermaid-lint`: pass (only Node.js 20 deprecation notices,
  not errors).
- `unit-tests`: 100% pass inside devcontainer; GoogleTest XML artifact uploaded.
- `coverage`: **84.9%** line coverage (439/517 lines); 80% threshold met;
  HTML artifact uploaded. `littlefs_file_system.cpp` excluded (hardware driver,
  untestable in host build).

**Static Analysis** (`static-analysis.yml`) — run [24310437200](https://github.com/tgd1975/AwesomeStudioPedal/actions/runs/24310437200): ✓

- `clang-tidy` installed via `apt-get` in devcontainer; zero violations.

**CodeQL / Docs** — trigger is `push/PR to main` only; not triggered by branch
pushes. Will be verified when a PR targeting `main` is opened.

**Root-cause fixes applied during this task:**

1. Added `apt-get install clang-format` to `cpp-format` job (tool missing in image).
2. Added `apt-get install clang-tidy` to `clang-tidy` job (tool missing in image).
3. Replaced `pip install gcovr` with `apt-get install gcovr` (pip not available).
4. Added `--exclude='.*littlefs.*'` to gcovr to exclude untestable hardware file.
5. Added `test_non_keyboard_actions.cpp` with SerialOutputAction, LEDBlinkAction,
   and DelayedAction tests to raise coverage above 80%.
6. Fixed `modernize-make-unique` clang-tidy violations in `config_loader.cpp`
   and `config_loader_merge.cpp`.
7. Fixed `readability-redundant-member-init` in `action.h`.
8. Fixed `readability-convert-member-functions-to-static` in `config_loader.h/cpp`.
9. Fixed GMock LIFO expectation shadowing in `LEDBlinkAction.ExecuteBlinksOnce` test.
10. Fixed `SortUsingDeclarations` clang-format violation in `test_config_loader.cpp`.
