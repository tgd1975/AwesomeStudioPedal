---
id: TASK-116
title: Create config.schema.json and data/config.json
status: closed
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: BLEConfig
order: 2
prerequisites: [TASK-115]
---

## Description

Introduce a device-level hardware config file (`data/config.json`) that captures pin assignments currently hardcoded in `builder_config.h`. Add a JSON Schema to validate it and extend the firmware's config loader to read it on boot, overriding compiled-in defaults if present.

## Acceptance Criteria

- [ ] `data/config.json` created with current hardware pin values from `builder_config.h`
- [ ] `data/config.schema.json` created (JSON Schema draft-07) validating all fields
- [ ] Schema validates: all pin fields are integers 0–39; `numButtons` matches length of `buttonPins`; `numSelectLeds` matches length of `ledSelect`; `numProfiles` is integer ≥ 1
- [ ] Cross-field constraint (`numProfiles ≤ 2^numSelectLeds − 1`) enforced by a dedicated check in `scripts/pre-commit` (cannot be expressed in JSON Schema draft-07)
- [ ] `scripts/pre-commit` validates `data/config.json` against `data/config.schema.json`
- [ ] Firmware reads `data/config.json` on boot (via LittleFS) if present; falls back to compiled-in `hardwareConfig` defaults if absent — full backwards compatibility
- [ ] `docs/data/README.md` updated to reference `config.schema.json`

## Files to Touch

- `data/config.json` (new)
- `data/config.schema.json` (new)
- `docs/data/README.md` (updated)
- `scripts/pre-commit` — add `config.json` validation
- `lib/PedalLogic/src/config_loader.cpp` — add `loadHardwareConfig()` or extend `configureProfiles()`

## Test Plan

Manual: boot firmware without `config.json` on LittleFS → defaults used. Boot with valid `config.json` → overrides applied. Pre-commit hook rejects a config with `numButtons: 3` but `buttonPins` with 4 entries.

## Prerequisites

- **TASK-115** — pre-commit validation infrastructure must exist before adding more validators

## Notes

Example `data/config.json`:
```json
{
  "numProfiles": 7,
  "numSelectLeds": 3,
  "numButtons": 4,
  "ledBluetooth": 26,
  "ledPower": 25,
  "ledSelect": [5, 18, 19],
  "buttonSelect": 21,
  "buttonPins": [13, 12, 27, 14]
}
```

The cross-field `numProfiles ≤ 2^numSelectLeds − 1` constraint: with 3 select LEDs you can encode 7 profiles (2³ − 1). Exceeding this silently wraps the LED display. Enforce in the pre-commit script with a small Python snippet.
