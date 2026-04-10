---
id: TASK-002
title: Add .github/settings.yml for branch protection
status: closed
effort: Medium (2-8h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add `.github/settings.yml` to declare branch protection rules as code, requiring CI checks
and one review approval before merging to `main`.

## Acceptance Criteria

- [x] `.github/settings.yml` created with branch protection rules for `main`
- [x] Requires passing CI checks: `cpp-format`, `markdown-lint`, `mermaid-lint`, `unit-tests`
- [x] Requires 1 approving review, dismisses stale reviews
- [x] Force pushes and deletions disabled on `main`

## Notes

Delivered in commit `0c2febc`. See [.github/settings.yml](../../../.github/settings.yml).
