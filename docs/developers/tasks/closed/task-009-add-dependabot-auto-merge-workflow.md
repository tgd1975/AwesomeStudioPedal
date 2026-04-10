---
id: TASK-009
title: Add dependabot auto-merge workflow
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Add a GitHub Actions workflow to automatically merge Dependabot PRs that only bump
patch versions, after all CI checks pass.

## Acceptance Criteria

- [x] `.github/workflows/dependabot-automerge.yml` created
- [x] Auto-merges patch-level Dependabot PRs after CI passes
- [x] Does not auto-merge minor or major version bumps

## Notes

Delivered in commit `42d02f6`. See [.github/workflows/dependabot-automerge.yml](../../../.github/workflows/dependabot-automerge.yml).
