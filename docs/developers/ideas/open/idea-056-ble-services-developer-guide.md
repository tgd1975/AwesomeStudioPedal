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
- Three existing BLE docs that overlap in places and are silent in others:
  - [BLE_CONFIG_PROTOCOL.md](../../BLE_CONFIG_PROTOCOL.md)
  - [BLE_CONFIG_IMPLEMENTATION_NOTES.md](../../BLE_CONFIG_IMPLEMENTATION_NOTES.md)
  - [BLE_READBACK_IMPACT.md](../../BLE_READBACK_IMPACT.md)
- The host/app-side code that consumes characteristics.
- The on-device and host tests.
- A long tail of decisions, gotchas, and "why did we do it that way" rationale that lives only in commit messages and closed-task postmortems.

When a service or characteristic is added or changed (most recently TASK-357 around `MAX_CONFIG_BYTES` reconciliation, and TASK-354 firmware-version exposure — currently blocked and re-homed to EPIC-026), there is no single document a developer can read first. This means:

- The same questions get re-answered ("what UUID convention do we use?", "READ vs NOTIFY?", "where does the byte-layout doc live?", "what test layers must be touched?").
- Easy-to-miss steps (updating the protocol doc, the reassembler header, the host parser, any BLE-specific documentation that touches the changed surface, the on-device test) are missed.
- Cross-cutting invariants (max payload sizes, MTU assumptions, endianness, version negotiation) are rediscovered rather than referenced.

## Goal

A single authoritative developer guide — somewhere under `docs/developers/` — that answers, for this project specifically:

- Which services and characteristics exist today, with UUIDs and a one-line purpose each (or a pointer to the canonical list).
- The end-to-end recipe for adding or changing a service/characteristic: every file that must change, every test layer that must be touched, every doc that must be updated.
- The conventions: UUID assignment, READ/WRITE/NOTIFY choice, payload framing, max-size constants, version handling, error semantics.
- The gotchas: known platform quirks (ESP32 NimBLE vs Bluefruit), MTU negotiation, pairing/bonding behaviour, parallel-session test interference.
- Pointers to the relevant tests on each layer (host fakes, on-device, app integration).

## Canonical vs derived — split before drafting

The biggest drift risk is the per-service catalog (UUIDs, characteristics, lengths). Apply the awesome-task-system "canonical source vs generated artifact" lens (see CLAUDE.md):

- **Hand-written, durable**: recipe, conventions, gotchas, references. These rarely change and benefit from human prose.
- **Generated, derived**: the service/characteristic catalog. Ideally produced from a YAML manifest the firmware also consumes, or extracted from the firmware code, so the guide cannot disagree with what the device actually advertises.

This split needs to be decided **before drafting** — not at review time. A hand-rolled catalog will rot within a couple of release cycles.

## Decide consolidation up front

The three existing BLE docs already cover parts of this surface. Before drafting, decide for each:

| Doc | Keep / fold into guide / replace? |
|---|---|
| `BLE_CONFIG_PROTOCOL.md` | ? |
| `BLE_CONFIG_IMPLEMENTATION_NOTES.md` | ? |
| `BLE_READBACK_IMPACT.md` | ? |

Otherwise the new guide either duplicates content (drift in two places) or sits next to them as a fourth file with no clear precedence.

## Inputs to draw on

- **Codebase** — current BLE service code, the three existing BLE docs above, the host parsers, the test files. Source of truth for what *is*.
- **Closed-task postmortems and commit messages** — recent BLE tasks (TASK-357, the EPIC-026 cluster, IDEA-046 around the copy loop) carry the rationale that never made it into a committed doc. Durable, version-controlled, mineable. (Chat history is per-session and not reliably retrievable — don't depend on it.)
- **External references** — Bluetooth SIG conventions, NimBLE and Bluefruit documentation. Cite, don't copy.

## Success criterion

One concrete fail-condition, picked before promoting to a task. Candidates:

- A new dev can add a characteristic to an existing service following only the guide, without asking questions.
- The next BLE-touching task references the guide in its description and checks off its recipe items.
- The guide's recipe checklist catches at least one of the missed-step classes that doc/code reconciliation work like TASK-357 has historically surfaced (e.g. "host parser updated but on-device test not extended").

## Out of scope for this idea

To keep the idea shippable, these are deferred:

- A new `ble-implementation` skill that triggers on BLE-file edits — promote to a follow-on idea once the guide exists and a checklist has settled.
- A CI / pre-commit staleness check (warn when BLE code changes without touching the guide) — bring back only if drift actually bites.

## Open questions

- Where should the guide live? `docs/developers/BLE_SERVICES_GUIDE.md`, or a folder with sub-pages per service?
- If the catalog is generated (per *Canonical vs derived*), does it live as a separate file alongside the guide, or as a generated section inside the single guide file?

## Rough first cut

1. Inventory the three existing BLE docs and answer the consolidation table above.
2. Decide canonical-vs-derived for the service catalog (YAML manifest, code extraction, or hand-written with accepted drift).
3. Draft the guide as a single file with sections: *Service catalog*, *Recipe: add/change a service*, *Conventions*, *Gotchas*, *Tests*, *References*.
4. Mine recent BLE-related closed tasks and commits for rationale that belongs in *Gotchas* or *Conventions*.
5. Pick one success criterion from the list above and verify against it before closing the resulting task.
