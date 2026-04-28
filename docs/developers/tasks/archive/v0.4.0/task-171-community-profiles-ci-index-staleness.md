---
id: TASK-171
title: CI — index.json staleness check
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 6
---

## Description

Add a CI step that regenerates `profiles/index.json` using
`npm run generate-profiles-index` (TASK-168) and diffs the output against the committed
file. If they differ, fail the PR with a message telling the contributor to run
`npm run generate-profiles-index` and commit the result.

## Acceptance Criteria

- [ ] CI step runs `npm run generate-profiles-index` into a temp location (or in-place) and diffs against the committed `profiles/index.json`
- [ ] PR fails with a clear message ("index.json is stale — run `npm run generate-profiles-index` and commit the result") if the files differ
- [ ] PR passes when the committed index matches the generated output
- [ ] Runs as part of the same workflow as TASK-169 and TASK-170
- [ ] Passes on the 12 starter profiles from TASK-167 with their committed index

## Notes

Diff must ignore the `generated` timestamp field, or the generator must produce a
deterministic timestamp (e.g. use the git commit date, or omit the field during CI
diffing). Decide which approach and document it in the script.

Depends on: TASK-168, TASK-169.
