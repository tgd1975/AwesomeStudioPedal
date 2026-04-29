---
id: TASK-310
title: Configurable BLE device name — firmware + schema
status: open
opened: 2026-04-29
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
epic: config-driven-runtime-customisation
order: 5
---

## Description

Make the advertised BLE device name configurable via `config.json`. Add an
optional `ble.deviceName` field. When present, the advertised name is
`AwesomeStudioPedal_{deviceName}`. When absent or empty, the firmware
auto-generates `ASP_{numButtons}_{numProfiles}` from the existing config
fields (e.g. `ASP_4_7` for the default config). No persistent counter,
no NVS plumbing — the fallback is purely derived from the loaded config.

This task covers the firmware change and both schemas. Web config builder
and Flutter app updates are tracked as separate sibling tasks under the
same epic.

## Acceptance Criteria

- [ ] `ConfigLoader` parses optional `ble.deviceName` and round-trips it through `saveToFile` and `mergeConfig`.
- [ ] BLE init advertises `AwesomeStudioPedal_{deviceName}` when the field is set and non-empty.
- [ ] BLE init advertises `ASP_{numButtons}_{numProfiles}` when the field is absent or empty.
- [ ] `data/config.schema.json` and `app/assets/config.schema.json` describe the new optional field.

## Test Plan

**Host tests** (`make test-host`):

- Add `test/unit/test_ble_device_name.cpp` (or extend the existing config test).
- Cover: explicit name produces `AwesomeStudioPedal_{name}`; missing key falls back to `ASP_{numButtons}_{numProfiles}`; empty string also falls back; sane behaviour for `numButtons`/`numProfiles` at edge values (0, large).

**On-device verification** (manual, not automated):

- Flash to ESP32 with default config, scan from a phone, confirm `ASP_4_7` shows up.
- Flash with `ble.deviceName: "MyPedal"`, confirm `AwesomeStudioPedal_MyPedal` shows up.

## Notes

- Length budget: BLE advertising name is limited (typically 29 bytes for the local-name AD field). Decide what to do if `AwesomeStudioPedal_{deviceName}` exceeds the limit — truncate, reject at config load, or warn? Resolve during implementation.
- The fallback uses `numButtons` and `numProfiles` from the same `config.json` — both already validated by the schema, so no extra error paths needed.

## Documentation

- `docs/builders/HARDWARE_CONFIG.md` — document the new `ble.deviceName` field and the auto-generated fallback rule.
