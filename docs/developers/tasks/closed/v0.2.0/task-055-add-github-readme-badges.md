---
id: TASK-055
title: Add GitHub README badges
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add status badges to `README.md` to give visitors an at-a-glance view of CI health,
test coverage, and code quality.

## Acceptance Criteria

- [x] CI workflow badge added (links to the GitHub Actions workflow)
- [x] Test coverage badge added (links to coverage report)
- [x] Static analysis / clang-tidy badge added if a dedicated workflow exists
- [x] CodeQL badge added (TASK-007 workflow)
- [ ] All badges verified as rendering correctly on GitHub

## Notes

Added a "Status" section to `README.md` replacing the old "Test Coverage" section.
Four badges added: CI (`test.yml`), CodeQL (`codeql-analysis.yml`),
Static Analysis (`static-analysis.yml`), and the existing shields.io coverage badge.

Verification on GitHub requires the branch to be pushed/merged; mark the final
acceptance criterion once confirmed live.
