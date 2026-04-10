---
id: TASK-018
title: Create code smell detection scripts
status: closed
effort: Large (8-24h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Create `scripts/check_code_smells.py` to detect C++ code smells that static analysis
doesn't catch: magic numbers, overly long functions, and deeply nested control flow.

## Acceptance Criteria

- [x] `scripts/check_code_smells.py` created and executable
- [x] Detects magic numbers (numeric literals not in named constants)
- [x] Detects functions exceeding a configurable line length threshold
- [x] Detects nesting depth exceeding a configurable threshold
- [x] Integrated into CI pipeline (fails on violations)
- [x] All existing violations resolved before enabling in CI

## Notes

Delivered in commits `d6f9e90` and `4c84d89` (with follow-up fixes in `bb1174c` and `4c8c461`).
See [scripts/check_code_smells.py](../../../scripts/check_code_smells.py).
The refactoring required to pass the smell checks resulted in splitting `config_loader.cpp`.
