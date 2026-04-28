---
id: TASK-081
title: Senior software architecture review
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Senior
human-in-loop: Support
epic: Finalization
order: 3
---

## Description

Review the overall system architecture before the first public release. Evaluate layer
separation (`lib/PedalLogic/` vs `src/`), the action/profile abstraction, the BLE stack
integration, the config-loading pipeline, and the test infrastructure for structural soundness,
maintainability, and future extensibility. Identify any architectural debt — tight coupling,
missing abstractions, or design decisions that will be hard to change once external contributors
start working in the repo.

## Acceptance Criteria

- [ ] Layer boundaries documented and evaluated: `lib/PedalLogic/` (pure logic) vs `src/`
      (hardware HAL + BLE) are cleanly separated with no leaking Arduino or BLE types into
      the logic layer
- [ ] Action polymorphism and profile-management design assessed; any identified improvements
      filed as follow-up tasks
- [ ] Architecture document (`docs/developers/ARCHITECTURE.md`) updated to reflect any
      discrepancies between the documented design and the current implementation

## Test Plan

No automated tests required — this is a review and documentation task. Any structural changes
identified spawn their own implementation tasks.

## Notes

- Cross-reference the existing `docs/developers/ARCHITECTURE.md` against the current code;
  flag any sections that are outdated or missing.
- Pay attention to: `ILEDController` abstraction usage, `ConfigLoader` responsibility scope,
  `DelayedAction` ownership model (stack vs heap), and BLE callback thread-safety.
- Consider whether the nRF52840 target shares enough code with the ESP32 target to justify its
  current abstraction cost, or whether it should be explicitly marked unsupported.
- Coordinate with TASK-079 (senior code review) — architecture and code-level findings often
  overlap; run these in parallel and cross-reference.
