---
id: TASK-057
title: Check if GitHub CI pipeline is running flawlessly
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Audit all GitHub Actions workflows to ensure they are passing, not flaky, and cover all
intended checks. Fix any failing or misconfigured jobs.

## Acceptance Criteria

- [ ] All workflow runs on `main` are green
- [ ] No flaky jobs (spurious failures unrelated to code changes)
- [ ] Workflow matrix covers all intended platforms/configurations
- [ ] Any deprecated actions updated to current versions
- [ ] Results documented in `## Notes`

## Notes

Workflows to check (under `.github/workflows/`):

- `test.yml` — host unit tests + static analysis
- `codeql-analysis.yml` — CodeQL security scanning
- Any dependabot auto-merge or other automation workflows

Use `gh run list` and `gh run view` to inspect recent runs.
