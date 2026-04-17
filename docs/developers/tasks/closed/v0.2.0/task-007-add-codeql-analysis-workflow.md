---
id: TASK-007
title: Add CodeQL analysis workflow
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Large (8-24h)
complexity: Senior
human-in-loop: No
---

## Description

Add a GitHub Actions workflow to run CodeQL security analysis on C++ source code on every
push to `main` and on pull requests.

## Acceptance Criteria

- [x] `.github/workflows/codeql-analysis.yml` created
- [x] Runs on push to `main`, on PRs, and on a weekly schedule
- [x] Analyzes C++ with `security-extended` queries

## Notes

Delivered in commit `42d02f6`. See [.github/workflows/codeql-analysis.yml](../../../.github/workflows/codeql-analysis.yml).
