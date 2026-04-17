---
id: TASK-059
title: Document the scripts/ folder
status: closed
opened: 2026-04-10
closed: 2026-04-11
effort: Medium (2-8h)
complexity: Junior
human-in-loop: No
---

## Description

The `scripts/` folder contains seven tools used for development, CI, and maintenance,
but none of them have documentation. Add a `scripts/README.md` covering all scripts
and add inline docstrings/comments to each script file where missing.

Scripts to document:

- `check_code_smells.py` — static analysis for magic numbers, long functions, deep nesting
- `update_task_overview.py` — regenerates `docs/developers/tasks/OVERVIEW.md` from task files
- `migrate_tasks.py` — one-time migration tool (historical, keep for reference)
- `validate_mermaid.py` — validates Mermaid diagrams in markdown files
- `serial_monitor.py` — serial port monitor for on-device debugging
- `format-code.sh` — runs clang-format on all C++ source files
- `pre-commit` — pre-commit hook script

## Acceptance Criteria

- [ ] `scripts/README.md` exists with a table listing each script, its purpose, and usage example
- [ ] Each Python script has a module-level docstring describing purpose, usage, and arguments
- [ ] `format-code.sh` and `pre-commit` have a comment header explaining purpose and usage
- [ ] `scripts/README.md` passes markdownlint

## Notes
