---
id: TASK-085
title: Project management review
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Large (8-24h)
complexity: Senior
human-in-loop: Support
group: Finalization
order: 1
---

## Description

Audit every closed task against the current codebase to verify that what was committed
actually matches what the task required. Produce a **diff list** — a structured report
of three categories:

1. **Fully implemented** — acceptance criteria satisfied, no drift detected.
2. **Partially implemented or drifted** — the task was closed but a later task, an
   untracked change, or a refactor overwrote, broke, or superseded part of the original
   work. Describe what changed and what (if anything) needs to be restored or re-evaluated.
3. **Implemented without a task** — features or fixes present in the codebase that have no
   corresponding closed task (i.e. the user committed directly or Claude made changes outside
   the task workflow). List the commit(s) and affected files.

This review is a pre-release sanity check to ensure the project's task history accurately
reflects the actual state of the code and nothing slipped through the cracks.

## Acceptance Criteria

- [ ] Every closed task (37 at time of writing) has been checked against its acceptance
      criteria and assigned one of: Fully implemented / Drifted / Superseded / N/A
- [ ] A written diff list is produced (Markdown table or bullet list) and committed to
      `docs/developers/PROJECT_REVIEW.md`
- [ ] Any drifted or missing implementations are filed as new open tasks or explicitly
      documented as intentionally deferred

## Test Plan

No automated tests required — this is a project-management and audit task.

## Notes

### How to approach the audit

1. Start with `git log --oneline` to get the full commit history.
2. For each closed task file in `docs/developers/tasks/closed/`, read its acceptance
   criteria and cross-reference against the relevant source files and git history
   (`git log --all --grep="TASK-NNN"` and `git show <commit>`).
3. Flag any task whose commit touches files that were later modified by a different task
   or an untracked commit — that is a candidate for drift.
4. Run `git log --oneline --all` and compare against the list of closed task commit
   messages to surface commits that do not reference any task ID.

### Key risk areas (tasks known to interact)

- **TASK-031 (simulator) + TASK-032 (config builder)** — share `schema.json` and the
  `docs.yml` deploy step; verify both still deploy correctly after the simulator was added.
- **TASK-051 (Printables link) + README** — README has been touched by multiple tasks;
  verify all links and rows are intact.
- **Firmware Group A tasks** — any task that touched `profile_manager.cpp` or
  `config_loader.cpp` may have interacted with others; check for unintended overwrites.
- **`docs.yml` workflow** — modified by at least TASK-032 and TASK-031; verify the final
  state is correct and not a partial merge of two versions.

### Output format suggestion

```markdown
## Diff List

| Task      | Title                          | Status              | Notes |
|-----------|-------------------------------|---------------------|-------|
| TASK-001  | ...                           | Fully implemented   |       |
| TASK-015  | ...                           | Drifted             | Overwritten by TASK-032 commit abc1234 |
| —         | Add Printables link (no task) | Implemented w/o task| Commit def5678 |
```
