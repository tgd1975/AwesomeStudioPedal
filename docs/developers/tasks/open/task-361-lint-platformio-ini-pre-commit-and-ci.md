---
id: TASK-361
title: Lint platformio.ini in pre-commit and CI
status: open
opened: 2026-05-01
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add a static lint of `platformio.ini` that runs both in the pre-commit hook and
in GitHub Actions CI, so a malformed or typo'd env definition is caught before
it can break a developer's build or land on `main`.

Today, a typo in an env name, an invalid key, or a malformed value in
`platformio.ini` only surfaces when someone tries to build that env — which may
be hours or days after the offending commit lands, and on a different machine
than the one that introduced it. A cheap static check at commit time and in CI
closes that gap without needing hardware or a full `pio run`.

Scope is **lint only**: validate the schema and structure of `platformio.ini`.
Building (`pio run`) and on-device tests (`pio test`) are explicitly out of
scope for this task — they belong to a separate, larger CI effort. nRF52840
envs are in scope for the lint, since linting is static and does not need the
device (consistent with EPIC-025, which only blocks runtime on-device work).

## Acceptance Criteria

- [ ] A lint command exists (e.g. `pio project config` or equivalent) that
      exits non-zero on any malformed `platformio.ini`, and is documented in
      the dev docs.
- [ ] The pre-commit hook invokes the lint when `platformio.ini` is in the
      changeset; verified by deliberately introducing a typo locally and
      observing the hook block the commit, then reverting.
- [ ] A GitHub Actions workflow runs the same lint on PRs and pushes to `main`;
      verified by a failing CI run on a throwaway branch with a deliberately
      broken `platformio.ini`.
- [ ] The lint covers every env defined in `platformio.ini` (ESP32 build envs,
      ESP32 on-device test envs, nRF52840 envs, any host/test envs) — not a
      hardcoded subset.
- [ ] `docs/developers/DEVELOPMENT_SETUP.md` (or wherever pre-commit hooks are
      documented) gets a short note pointing at the new check.

## Test Plan

No automated tests required — change is non-functional (CI / tooling only).

The acceptance criteria above are the de-facto test: the lint must pass on the
current `platformio.ini`, fail on a deliberately broken one in both pre-commit
and CI, and that behaviour must be demonstrated before the task is closed.

## Documentation

- `docs/developers/DEVELOPMENT_SETUP.md` — note the new pre-commit lint step
  and how to reproduce a CI failure locally.

## Notes

- No hardware required. This task can be completed and closed with the
  nRF52840 device unavailable (EPIC-025).
- Tool choice: `pio project config` (built into PlatformIO) is the obvious
  candidate and probably sufficient. If it turns out to under-report (e.g.
  silently accepts unknown keys), consider a small companion check — but pick
  the lightest option that catches the typos we actually see in practice. Do
  not build a custom INI parser unless `pio project config` is genuinely
  inadequate.
- Pre-commit hook should only run the lint when `platformio.ini` is in the
  staged pathspec — no need to re-lint on every commit.
- CI workflow: extend an existing workflow if a suitable one exists, otherwise
  add a small dedicated `platformio-lint.yml`. Keep the runner lean (no
  toolchain install beyond what `pio project config` needs).
