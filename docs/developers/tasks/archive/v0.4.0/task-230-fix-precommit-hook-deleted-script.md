---
id: TASK-230
title: Fix pre-commit hook — invokes deleted scripts/update_future_ideas.py
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: feature_test
order: 18
---

## Description

The pre-commit hook at `scripts/pre-commit` still has a step labelled
"Updating README future ideas..." that runs `python scripts/update_future_ideas.py`,
but that script was removed in an earlier task-system commit (TASK-211) and replaced
by `scripts/update_idea_overview.py`. Every commit now fails with:

```
Updating README future ideas...
python: can't open file '/home/…/scripts/update_future_ideas.py': [Errno 2] No such file or directory
```

Discovered while closing TASK-229, which had to use `--no-verify` to land because
the failure is unrelated to the staged files.

## Acceptance Criteria

- [ ] Update `scripts/pre-commit` to either invoke `scripts/update_idea_overview.py`
      (same role, new name) or remove the step entirely if it is now subsumed by
      `housekeep.py` / another mechanism.
- [ ] A trivial commit runs pre-commit cleanly with no `--no-verify`.

## Test Plan

No automated tests required — this is a tooling-script fix. Manual verification
by making a throwaway commit on a fresh branch.

## Notes

- If keeping the step: make sure the script path and CLI signature still match
  (the old script accepted no arguments; check the new one).
- Check whether the CI (`.github/workflows/*.yml`) also references the old script
  name — same fix class, same resolution.
