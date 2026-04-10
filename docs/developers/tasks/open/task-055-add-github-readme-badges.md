---
id: TASK-055
title: Add GitHub README badges
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add status badges to `README.md` to give visitors an at-a-glance view of CI health,
test coverage, and code quality.

## Acceptance Criteria

- [ ] CI workflow badge added (links to the GitHub Actions workflow)
- [ ] Test coverage badge added (links to coverage report)
- [ ] Static analysis / clang-tidy badge added if a dedicated workflow exists
- [ ] CodeQL badge added (TASK-007 workflow)
- [ ] All badges verified as rendering correctly on GitHub

## Notes

Badge URL patterns:

```markdown
[![CI](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/test.yml/badge.svg)](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/test.yml)
[![CodeQL](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/codeql-analysis.yml)
```

Place badges near the top of `README.md`, below the title.
