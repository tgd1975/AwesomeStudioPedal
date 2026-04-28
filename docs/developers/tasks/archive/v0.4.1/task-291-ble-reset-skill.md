---
id: TASK-291
title: Add /ble-reset skill encapsulating the flaky-pairing recovery dance
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Junior
human-in-loop: No
epic: agent-collaboration-skills
order: 8
---

## Description

Chat `1a30c9b9-03bf-4cac-8197-1e481bb30024` shows the agent ran
`bluetoothctl disconnect 24:62:AB:D4:E0:D2 2>&1 | tail -1` **11 times**
in a single session, alongside 8 invocations of
`.venv/bin/python scripts/pedal_config.py scan` — clearly fighting flaky
BLE pairing recovery with ad-hoc retries.

A small `/ble-reset` skill would replace the freehand retry dance with
one canonical recovery sequence:

1. `bluetoothctl disconnect <MAC>`
2. wait briefly
3. `bluetoothctl remove <MAC>` (force fresh pair)
4. `bluetoothctl scan on` with timeout
5. `bluetoothctl pair <MAC>` / `connect <MAC>`
6. verify with `pedal_config.py scan`

The skill encodes the timeout values and reports the first step that
fails, so when pairing breaks I have one tool to reach for instead of
guessing.

## Acceptance Criteria

- [x] `.claude/skills/ble-reset/SKILL.md` exists and is registered in
      `.vibe/config.toml`.
- [x] Skill reads the pedal MAC from the same env var introduced in
      TASK-290 (do not hard-code `24:62:AB:D4:E0:D2`).
- [x] Skill exits non-zero with a clear message naming the failed step
      if any step fails.
- [x] CLAUDE.md or the BLE testing docs gain a one-liner: "when BLE
      pairing flakes, use `/ble-reset` rather than retrying manually."

## Test Plan

No automated tests required — the skill exercises real Bluetooth stack
state and is fundamentally hardware-flaky. Validate manually by
intentionally breaking pairing (e.g. power-cycling the pedal mid-scan)
and running the skill.

## Notes

- Source signal: chat `1a30c9b9` — 11 raw `bluetoothctl disconnect` calls.
- Depends on TASK-290 for the env-var-driven pedal MAC source.
- This is reactive tooling for hardware flakiness — won't fix the
  underlying flake, but standardises recovery.
