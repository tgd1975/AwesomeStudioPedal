---
id: TASK-045
title: Add Mermaid validation script
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Create `scripts/validate_mermaid.py` to extract and validate Mermaid code blocks from
markdown files, used by both the pre-commit hook and the CI `mermaid-lint` job.

## Acceptance Criteria

- [x] `scripts/validate_mermaid.py` created
- [x] Extracts all ` ```mermaid ` blocks from provided markdown files
- [x] Validates syntax (catches malformed diagrams)
- [x] Used by CI workflow (TASK-039)
- [x] Exit code non-zero on any validation failure

## Notes

Delivered in commit `ddfa7dd`, improved in `9b00e3f`.
See [scripts/validate_mermaid.py](../../../scripts/validate_mermaid.py).
