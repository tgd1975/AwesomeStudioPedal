---
id: EPIC-020
name: config-driven-runtime-customisation
title: Config-driven runtime customisation
status: open
opened: 2026-04-29
closed:
assigned:
---

# Config-driven runtime customisation

Extend `config.json` and `profiles.json` so users can shape pedal behaviour
without recompiling firmware: a top-level `independentActions` block that
fires alongside whatever profile is active, and a configurable BLE device
name with a deterministic auto-generated fallback.

Seeded by IDEA-024 (Profile-Independent Actions) and IDEA-025 (Configurable
BLE Device Name).

## Tasks

Tasks belonging to this epic reference it via
`epic: config-driven-runtime-customisation` in their frontmatter.
