---
id: TASK-290
title: Project-level env-var setup (direnv or equivalent) to kill source/cd churn
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Junior
human-in-loop: Clarification
epic: agent-collaboration-skills
order: 7
---

## Description

The agent re-establishes shell state on almost every Bash call because each
invocation runs in a fresh shell:

- `cd /home/tobias/Dokumente/Projekte/AwesomeStudioPedal` — **288 times**
  across 148 transcripts (one session: 125×).
- `source /tmp/voe.sh` — 43× in a single session, re-sourcing the same
  env-setup script.
- The Pixel device serial `4C200DLAQ0056N` — 135× hard-coded into bash in
  one UI-test session.

A single project-level env-var mechanism would replace all three patterns.
Two viable approaches:

1. **`direnv` with a checked-in `.envrc`** — env vars auto-load whenever
   the shell enters the repo. Standard, well-supported, no per-call cost.
2. **A `Makefile`/`scripts/env.sh` that the canonical skills source
   internally** — keeps the env-vars discoverable in-repo without adding
   a tool dependency. Tighter coupling but zero new tooling.

This task picks one and implements it. Variables to consolidate at minimum:
the Pixel device serial (`ASP_PIXEL_DEVICE`), the ESP32 port
(`ASP_ESP32_PORT`), the BLE pedal MAC (`ASP_PEDAL_MAC`), and any other
re-typed hex strings the audit surfaces.

## Acceptance Criteria

- [x] Decision recorded between `direnv` (option 1) and in-repo
      `scripts/env.sh` (option 2). Note the reason in the task notes.
- [x] All hard-coded device IDs in skills and scripts read from the
      chosen mechanism.
- [x] The mechanism is documented for new contributors in
      `docs/developers/` (one short section, not a separate page).
- [x] CLAUDE.md gains a one-liner pointing at it: "use `$ASP_*` env vars,
      do not retype device serials inline."

## Test Plan

No automated tests required — change is configuration plus a CLAUDE.md
prose addition. Validate by running an existing skill (e.g.
`/verify-on-device`) without manually exporting any env vars and
confirming it picks up the values.

## Notes

- Source signal: 288 `cd`, 43 `source /tmp/voe.sh`, 135 raw Pixel hex
  uses in transcript corpus.
- Related: TASK-283 (UI-driving skill) already plans to read `$DEV` from
  a single source — this task creates that source.
- `direnv` adds a tool dependency but is the lower-friction option once
  installed; the in-repo script approach avoids the dependency at the
  cost of explicit `source` calls in skills.

### Design choice: option (1) — direnv with `.envrc.example` checked in, `.envrc` gitignored

Picked **direnv**. Rationale:

- The audit's signal was per-call shell churn — `cd` 288×, `source /tmp/voe.sh`
  43×, the same hex 135× in one session. direnv eliminates the per-call cost
  entirely (the env vars are present whenever the shell is in the repo);
  option 2 still requires every skill to remember to `source scripts/env.sh`.
- direnv is one apt-install on Ubuntu and one brew-install on macOS — low
  friction for the only two platforms this project actually targets.
- Per-developer device serials shouldn't be checked in: ship `.envrc.example`
  with placeholders + setup instructions, gitignore the real `.envrc`. This
  also future-proofs for a second developer / a CI host where the serial
  differs.
- Skills (`/ui-dump`, `/verify-on-device`, `/ble-reset`) all read the env vars
  with sensible single-device fallbacks, so a developer who skips direnv
  isn't locked out — the env vars are a strict requirement only when there
  are multiple devices plugged in.

Files added in this task:

- [`.envrc.example`](../../../../.envrc.example) — template with the four
  `ASP_*` vars and `ANDROID_SERIAL` mirror.
- `.gitignore` entry for `.envrc`.
- New "Project env vars (direnv)" section in
  [DEVELOPMENT_SETUP.md](../../DEVELOPMENT_SETUP.md#project-env-vars-direnv).
- One-liner in CLAUDE.md pointing at the above.

No live skills or scripts had hard-coded device IDs that needed migration —
the audit's signal was about agent transcripts re-typing them in ad-hoc bash,
not about checked-in code re-using them. Going forward, `/ui-dump` and the
upcoming `/ble-reset` (TASK-291) read from `$ASP_*` and never inline the
literals.
