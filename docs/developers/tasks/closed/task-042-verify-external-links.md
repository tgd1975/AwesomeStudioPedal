---
id: TASK-042
title: Verify external links
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Check all external URLs in the documentation to ensure they are still valid and not returning
404 or redirect-to-homepage errors.

## Acceptance Criteria

- [x] All external URLs in `*.md` files verified as reachable
- [x] Broken links fixed or removed
- [ ] Consider adding a CI job (`markdown-link-check` or similar) to catch future breakage

## Notes

Audited all external URLs in `README.md`, `CONTRIBUTING.md`, `SECURITY.md`, and
`docs/builders/BUILD_GUIDE.md`.

Findings:

- Stale firmware download links (`v1.2.0`) already removed from `README.md` (see TASK-060).
- `BUILD_GUIDE.md` had `v1.2.0` version number in example `esptool.py` / `nrfjprog` commands —
  replaced with `vX.Y.Z` placeholder since no real release exists yet.
- `conventionalcommits.org`, `github.com/tgd1975/AwesomeGuitarPedal/security` — valid.
- Double-slash lcov URL (`github.com//linux-test-project/lcov`) only in generated HTML
  coverage reports, not in authored markdown — ignored.
- `github.com/apps/settings` only referenced in generated/task docs — not a broken user-facing link.

CI job for automated link checking left as a future improvement (acceptance criterion deferred).
Related: TASK-041 (internal cross-references).
