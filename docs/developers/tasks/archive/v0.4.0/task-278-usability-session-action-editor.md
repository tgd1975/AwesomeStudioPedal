---
id: TASK-278
title: Usability session — Action Editor with non-developer tester (AE-U1, AE-U2)
status: closed
closed: 2026-04-27
opened: 2026-04-27
effort: Small (2-4h)
effort_actual: Small (<2h)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 44
---

## Description

The Action Editor usability checks AE-U1 (timing) and AE-U2
(terminology) defined in
[FEATURE_TEST_PLAN.md Part 2.5](../../FEATURE_TEST_PLAN.md) require
a non-developer tester. They were deferred during TASK-155 rounds 1-3
because every round was driven by an automated `/verify-on-device`
session — which can confirm correctness but cannot answer "is this
intuitive?".

This task tracks the human-led usability session needed to close out
those two checks.

## Tests to execute

| Test ID | Description |
|---------|-------------|
| AE-U1 | Hand the tester an unfamiliar phone with the app open on Profile List → tap Profile 01 → tap Button A. Ask them to set Button A to "Play/Pause media" without further guidance. Time how long they take from "go" to a saved action that produces `SendMediaKeyAction` / `MEDIA_PLAY_PAUSE`. Target: under 60 seconds. |
| AE-U2 | After AE-U1, ask the tester to read the Action Type dropdown and explain in their own words what each of "Key (named)", "Key (raw HID)", and "Media Key" means. Note any confusion, miscategorisations, or "I'd guess but I'm not sure" answers. |

## Acceptance Criteria

- [ ] AE-U1 timing recorded in FEATURE_TEST_PLAN.md (round-3 row's
      placeholder updated with the actual time and pass/fail vs the
      60 s target).
- [ ] AE-U2 findings recorded in FEATURE_TEST_PLAN.md (one or two
      sentences per terminology item; explicit list of confusion
      points).
- [ ] If either check fails, file a UX-improvement child task with
      a concrete proposal (rename, tooltip, reorder, …). Do not
      attempt to fix in this task — its scope is observation only.

## Pre-conditions

- A non-developer tester (musician, family member, friend — not
  someone who has worked on this codebase).
- A test device with the current app build and at least one profile
  loaded (so a button row exists to tap into).
- Note-taking method (paper, voice memo, second device).

## Notes

- AE-01..AE-15 functional checks are all ✓ as of TASK-155 round 3
  (2026-04-27); the only AE-* coverage still missing is the human
  judgement that AE-U1/U2 require.
- TASK-277 (misleading `0x28` hint) is likely to surface again as
  AE-U2 confusion — that is fine; it is already filed. Do not
  re-file it.
- Cross-reference: TASK-155 (now closed), FEATURE_TEST_PLAN.md
  Part 2.5 round-3 result summary.
- Memory: enthusiastic, one-off — this is a single sit-down session,
  not a recurring research programme. No campaign / community work
  follows from it.
