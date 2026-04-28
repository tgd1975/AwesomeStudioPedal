---
id: TASK-226
title: Feature Test — CLI scan with two pedals (S-04)
status: paused
opened: 2026-04-23
effort: Small (<2h)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 1
prerequisites: [TASK-150]
---

## Description

Execute test **S-04** from `docs/developers/FEATURE_TEST_PLAN.md` Part 1.2:
run `python scripts/pedal_config.py scan` with **two** pedal boards powered on
simultaneously and confirm both appear in the output with distinct MAC addresses
and the `← pedal` marker.

This test was split off from TASK-150 because only a single physical pedal board
is currently available. The rest of the scan tests (S-01, S-02, S-03, CU-03)
were executed and recorded under TASK-150.

## Acceptance Criteria

- [ ] Two ESP32 pedal boards flashed with current firmware and powered on simultaneously
- [ ] `pedal_config.py scan` lists both devices within 5 s, each with the `← pedal` marker
- [ ] MAC addresses are distinct (no duplicates, no one device masking the other)
- [ ] Result recorded in the S-04 row of `FEATURE_TEST_PLAN.md`

## Test Plan

No automated tests required — this is a manual feature test that depends on
physical hardware. The underlying CLI code is covered by TASK-150 results.

## Prerequisites

- **TASK-150** — delivered the scan-command fixes (bleak 3.x compatibility,
  graceful BLE-off handling) that S-04 depends on.

## Notes

- Blocked until a second pedal board is built/available.
- Simple execution once hardware is on hand: power both, run scan, record result.

## Paused

- 2026-04-27: Paused — only one physical pedal unit is currently on hand.
  S-04 specifically requires two pedals running simultaneously, so the test
  cannot be executed until a second board is built. Resume with
  `/ts-task-active TASK-226` once the second pedal is available.
