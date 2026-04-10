---
id: TASK-039
title: Add Mermaid diagram linting
status: closed
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
---

## Description

Add Mermaid diagram syntax validation to the CI pipeline so invalid diagrams fail the build.

## Acceptance Criteria

- [x] CI job `mermaid-lint` added to `.github/workflows/`
- [x] All Mermaid code blocks in `*.md` files validated
- [x] CI check required before merge to `main`

## Notes

Delivered across commits `88047e1` and `9b00e3f`. The `mermaid-lint` CI check uses the
`scripts/validate_mermaid.py` validation script (TASK-045).
See [.github/workflows/docs.yml](../../../.github/workflows/docs.yml).
