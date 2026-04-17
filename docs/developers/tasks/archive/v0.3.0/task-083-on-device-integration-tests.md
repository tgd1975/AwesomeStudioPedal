---
id: TASK-083
title: On-device integration tests
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Senior
human-in-loop: Main
group: Finalization
order: 7
---

## Description

Run the full on-device test suite on real ESP32 hardware and manually verify all firmware
features end-to-end before the first public release. This goes beyond unit tests: flash the
firmware with the real `data/profiles.json`, pair over BLE to a host, and confirm that every
action type, every profile, and the SELECT button all behave exactly as the User Guide
describes. Any failures block the release.

## Acceptance Criteria

- [ ] All PlatformIO on-device tests pass (`make test-esp32-*`) on a physical NodeMCU-32S
- [ ] Manual smoke test confirms: BLE pairing, all 4 buttons across all 7 profiles, SELECT
      cycling, profile LED binary encoding, and DelayedAction countdown on real hardware
- [ ] Any hardware-only bugs found are filed as issues and either fixed or explicitly deferred
      with a documented rationale before the release tag is cut

## Test Plan

**On-device tests** (run via `/test-device` or `make test-esp32-*`):

- Flash firmware to NodeMCU-32S and run the full PlatformIO test suite
- Manual verification checklist:
  - BLE pairs to a desktop/mobile host without errors
  - Each button (A–D) in each profile sends the correct keystroke / string / media key
  - SELECT button cycles profiles 0–6 and profile LEDs show the correct binary encoding
  - DelayedAction (Profile 2, Button A) fires after the configured delay
  - SerialOutputAction (Profile 7, Button A) emits to the serial monitor
  - Power-on loads the correct default profile
- Requires: ESP32 NodeMCU-32S connected via USB, Bluetooth-capable host for pairing

## Notes

- This task is intentionally human-in-loop: automated tests cannot verify BLE HID output
  received by a host OS — a human must confirm keystrokes land correctly.
- If the nRF52840 target is to be officially supported, repeat the checklist on that hardware
  too; otherwise document it as untested in the release notes.
- Coordinate with TASK-080 (test coverage audit) — any gaps found here should feed back into
  the automated test suite.
- Run `/test-device esp32-button` (and other targets) first; manual smoke test is the final
  gate, not a replacement for the automated suite.
