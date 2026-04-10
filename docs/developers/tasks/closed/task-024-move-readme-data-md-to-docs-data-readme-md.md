---
id: TASK-024
title: Move README_data.md to docs/data/README.md
status: closed
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Move `README_data.md` from the repository root to `docs/data/README.md` to reduce root
clutter and keep data-related documentation co-located with the data files.

## Acceptance Criteria

- [x] `README_data.md` removed from repository root
- [x] Content moved to `docs/data/README.md`
- [x] Any links to the old path updated

## Notes

Delivered in commit `9f2e684`. The file was temporarily moved to the root again in a follow-up
commit (`a3c3044`) to exclude it from LittleFS upload, then settled at `docs/data/README.md`.
See [docs/data/README.md](../../../docs/data/README.md).
