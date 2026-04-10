---
id: TASK-042
title: Verify external links
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Check all external URLs in the documentation to ensure they are still valid and not returning
404 or redirect-to-homepage errors.

## Acceptance Criteria

- [ ] All external URLs in `*.md` files verified as reachable
- [ ] Broken links fixed or removed
- [ ] Consider adding a CI job (`markdown-link-check` or similar) to catch future breakage

## Notes

Focus on: `README.md`, `CONTRIBUTING.md`, `docs/developers/`, `SECURITY.md`.
Tools: `markdown-link-check` (npm), `lychee` (Rust), or manual review.
Ignore intentionally unreachable URLs (e.g. localhost examples) using ignore config.
Related: TASK-041 (internal cross-references).

The broken firmware download links that were previously in `README.md` have been removed
(replaced by a build-from-source note). TASK-060 covers restoring them after the first release.
