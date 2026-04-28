---
id: TASK-285
title: Add /commit skill encoding the --no-verify decision protocol
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Medium
human-in-loop: Clarification
epic: agent-collaboration-skills
order: 3
---

## Description

The user explicitly requested this skill in chat
`760f71a9-c084-4f06-b362-c9761aba64f7`:

> "add/update the commit skill - if, for example, the commit contains only
> md files, and local cpp files are not fulfilling the pre-commit hook
> (files with changes totally unrelated/pre existing changes/etc. to the
> work done on the md files), then maybe (not always) it is ok to bypass
> the hook. but this is a case by case decision."

The decision protocol already exists in CLAUDE.md ("Pre-commit hook failures
on unrelated changes" section) — three checks: staged-files-only,
pre-existing breakage, no silent regression — but it lives only as prose.
Lifting it into a `/commit` skill makes the protocol consistent and
auditable.

The transcript scan shows `git commit --no-verify` ran 10+ times in the
corpus. A skill that runs the protocol every time would either confirm
each bypass was justified, or surface cases where the agent skipped the
checks.

## Acceptance Criteria

- [x] `.claude/skills/commit/SKILL.md` exists and is registered in
      `.vibe/config.toml`.
- [x] When invoked with a pending commit, the skill (a) stages user-named
      files only, (b) attempts the commit, (c) on hook failure, runs the
      three CLAUDE.md checks and presents the standard message asking the
      user before suggesting `--no-verify`.
- [x] The skill never adds `--no-verify` silently — explicit user approval
      is required, and the failing check is named in the prompt.
- [x] CLAUDE.md "Pre-commit hook failures" section gains a one-liner:
      "use `/commit` to apply this protocol consistently."

## Test Plan

No automated tests required — change is skill scaffolding plus CLAUDE.md
prose. Validate manually by triggering a hook failure on an unrelated file
and checking the skill follows the protocol end-to-end.

## Notes

- Source signal: chat `760f71a9` (explicit user request) and 10+ raw
  `git commit --no-verify` invocations across transcripts.
- The protocol itself is already written down in CLAUDE.md — this task is
  about *executing* it consistently, not designing it.
