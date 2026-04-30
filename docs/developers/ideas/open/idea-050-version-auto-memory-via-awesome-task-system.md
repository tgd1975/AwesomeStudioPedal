---
id: IDEA-050
title: Version auto-memory via the awesome-task-system mirror pattern
description: Mirror Claude's auto-memory through awesome-task-system so memory edits become reviewable, versioned commits instead of out-of-repo files.
category: 🛠️ tooling
---

# Version auto-memory via the awesome-task-system mirror pattern

Claude's auto-memory currently lives at
`~/.claude/projects/<project-hash>/memory/` — outside the repo, not
versioned, not reviewable, not portable across machines. When the user
asks "commit that memory update", the request fails because the file
isn't in any git repo.

The `awesome-task-system/` package already solves the same shape of
problem for skills, scripts, and config: package source under
`awesome-task-system/`, live copy at the runtime location, sync script
that mirrors package → live, pre-commit hook that rejects divergence.
Apply the same pattern to memory.

## Motivation

- **Reviewable history** — memory edits become real commits with
  diffs and PR review.
- **Recoverable** — accidental memory deletions are recoverable from
  git, not gone forever.
- **Portable** — Tobias works on Windows 11 and Ubuntu; today memory
  is per-machine, not synced.
- **Closes a known gap** — sessions hit "can't commit, not in a repo"
  when asked to persist memory edits.

## Rough approach

- Add `awesome-task-system/memory/` as the package source.
- Extend `scripts/sync_task_system.py` (or add a sibling
  `sync_memory.py`) to mirror package → live.
- Add the memory paths to the `MIRRORS` list and the pre-commit
  divergence check.
- Document the workflow in `awesome-task-system/LAYOUT.md` and CLAUDE.md.

## Open questions

- **Which memory types to mirror?** `feedback_*` and `project_*` are
  shareable. `user_*` is personal — probably should *not* land in a
  collaborator's checkout. Filter by frontmatter `type:` field?
- **Sync direction.** One-way (package → live, edits go in package
  first) is consistent with the rest of awesome-task-system — but
  Claude writes memory live during sessions, not in the package. So
  either: (a) Claude writes to package and lets sync push to live, or
  (b) sync becomes bidirectional with a "live is newer, propagate
  back" rule. (a) is simpler but requires retraining the auto-memory
  write path.
- **`MEMORY.md` index.** Auto-memory expects this file at the live
  location and rewrites it on every memory write. Mirror it? Generate
  it from package contents at sync time?
- **Scope.** Project-specific memory only, or extend to user-global
  (`~/.claude/memory/`) too? Probably project-only — user-global is
  cross-project by design.
- **Multi-user implications.** If memory is versioned, two people on
  the same repo see each other's memory. Mostly fine for `project_*`
  (shared facts about the work), surprising for `feedback_*` (one
  person's preferences applied to another's session). Decide before
  rolling out.

## Related

- Versioning gap surfaced during a session about parallel-Claude
  commit safety — when the user asked to commit a memory update, the
  attempt failed because `~/.claude/projects/.../memory/` isn't a git
  repo. CLAUDE.md is currently silent on memory entirely.
- Sibling pattern already in use: see
  [awesome-task-system/LAYOUT.md](../../../../awesome-task-system/LAYOUT.md)
  and the `MIRRORS` list at the top of
  [scripts/sync_task_system.py](../../../../scripts/sync_task_system.py).
