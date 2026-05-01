---
id: IDEA-056
title: BLE services developer guide — authoritative doc for implementing/changing BLE services
description: Single source of truth for adding/changing BLE services in this project, ideally surfaced by an implementation-aware skill.
category: 📖 docs
---

# BLE services developer guide — authoritative doc for implementing/changing BLE services

## Motivation

BLE work in this project is currently scattered across several places:

- The firmware-side service code (`src/esp32/`, `src/ble_pedal_app.cpp`).
- The protocol/byte-format docs under `docs/developers/`.
- The host/app-side code that consumes characteristics.
- The on-device and host tests.
- A long tail of decisions, gotchas, and "why did we do it that way" rationale that lives only in commit messages, closed tasks, and Claude Code chat history.

When a service or characteristic is added or changed (most recently TASK-354 / TASK-357 around firmware-version exposure and `MAX_CONFIG_BYTES` reconciliation), there is no single document a developer can read first. This means:

- The same questions get re-answered ("what UUID convention do we use?", "READ vs NOTIFY?", "where does the byte-layout doc live?", "what test layers must be touched?").
- Easy-to-miss steps (updating the protocol doc, the reassembler header, the host parser, the BLE README, the on-device test) are missed.
- Cross-cutting invariants (max payload sizes, MTU assumptions, endianness, version negotiation) are rediscovered rather than referenced.

## Goal

A single authoritative developer guide — somewhere under `docs/developers/` — that answers, for this project specifically:

- Which services and characteristics exist today, with UUIDs and a one-line purpose each (or a pointer to the canonical list).
- The end-to-end recipe for adding or changing a service/characteristic: every file that must change, every test layer that must be touched, every doc that must be updated.
- The conventions: UUID assignment, READ/WRITE/NOTIFY choice, payload framing, max-size constants, version handling, error semantics.
- The gotchas: known platform quirks (ESP32 NimBLE vs Bluefruit), MTU negotiation, pairing/bonding behaviour, parallel-session test interference.
- Pointers to the relevant tests on each layer (host fakes, on-device, app integration).

## Inputs to draw on

- **Codebase** — current BLE service code, the protocol docs, the host parsers, the test files. This is the source of truth for what *is*.
- **Claude Code chat history** — recent BLE tasks (TASK-354, TASK-357, EPIC-? BLE-config work, IDEA-046 around the copy loop) contain the rationale that never made it into a committed doc. Worth mining for "why" content.
- **Internet / external references** — Bluetooth SIG conventions, NimBLE and Bluefruit documentation, common pitfalls. Cite, don't copy.

## Stretch — make a skill aware of it

A skill (e.g. `ble-implementation` or an extension to an existing one) should know the guide exists and surface it whenever a developer is about to touch BLE code:

- Triggered by edits under `src/esp32/` BLE files, `src/host/.../ble_*`, the protocol doc, or new BLE tests.
- Output: "Before changing BLE behaviour, read `docs/developers/BLE_SERVICES_GUIDE.md` — checklist of files/tests/docs to touch."
- Optional: a skill-side checklist that mirrors the guide's recipe section, so the developer can tick items off.

## Open questions

- Where should the guide live? `docs/developers/BLE_SERVICES_GUIDE.md`, or a folder with sub-pages per service?
- Should the per-service catalog (UUIDs, characteristics) be hand-written in the guide, or generated from the firmware code / a YAML manifest? The latter avoids drift but is more infrastructure.
- Is this one doc, or two — a *recipe* for changing services and a *reference* listing the current ones? (They have different update cadences.)
- Which existing skill should be extended vs creating a new one? `ble-reset` is for pairing recovery, not implementation; a new skill might be cleaner.
- How do we keep the guide from going stale? CI check that BLE code changes touch the guide? Pre-commit nudge? Or accept that authoritative ≠ exhaustive and rely on review.
- Worth consolidating with or replacing parts of the existing protocol docs, or purely additive?

## Rough first cut

1. Survey what's already documented (protocol docs, READMEs, header comments) and inventory the gaps.
2. Draft the guide as a single file with sections: *Service catalog*, *Recipe: add/change a service*, *Conventions*, *Gotchas*, *Tests*, *References*.
3. Mine recent BLE-related closed tasks and chat history for rationale that belongs in *Gotchas* or *Conventions*.
4. Decide on the skill integration — at minimum, a SKILL.md that points at the guide and lists the recipe checklist inline.
5. Add a lightweight staleness check (touched-BLE-without-touching-guide warning) only if it proves necessary.
