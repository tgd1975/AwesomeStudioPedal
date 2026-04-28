---
id: TASK-079
title: Senior code review
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Large (8-24h)
complexity: Senior
human-in-loop: Support
epic: Finalization
order: 2
---

## Description

Conduct a full senior-level code review of all project source before the first public release.
Covers `lib/PedalLogic/`, `src/`, `test/`, `docs/tools/` (simulator + config builder),
`scripts/`, and `.github/workflows/`. Goals: catch memory-safety issues, undefined behaviour,
logic errors, security holes (especially in web tools), dead code, and any deviation from the
project's coding conventions (naming, test coverage, comment policy).

## Acceptance Criteria

- [ ] All C++ files reviewed for memory safety, UB, RAII correctness, and bare-pointer leaks
- [ ] Web tools (simulator, config builder) reviewed for XSS, unvalidated input, and logic errors
- [ ] All findings documented (GitHub issues or inline `// TODO` comments); critical findings
      resolved or explicitly deferred before the first release tag

## Test Plan

No automated tests required — this is a review task. Each actionable finding spawns its own
fix task or PR.

## Notes

- Focus especially on `lib/PedalLogic/src/config_loader.cpp` (JSON parsing, untrusted input)
  and `src/main.cpp` / BLE stack integration (callback safety, ISR context).
- Web tools run entirely client-side; XSS via user-supplied JSON is the primary attack surface.
- Cross-reference against the testing policy in `CLAUDE.md` — flag any logic paths that lack
  host-test coverage.
- Consider running `clang-tidy` (`/clang-tidy`) and `/security-review` as structured starting
  points rather than a blank-page review.
