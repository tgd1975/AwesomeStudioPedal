---
id: TASK-238
title: On-device verification — BLE config integration test on production firmware + pairing PIN smoke test
status: closed
opened: 2026-04-24
closed: 2026-04-25
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Main
epic: feature_test
order: 24
prerequisites: [TASK-229, TASK-236, TASK-237]
---

## Description

TASK-236 retired the `BLE_CONFIG_TEST_BUILD` guard and reworked `runner.py` + the test
firmware to exercise the production `BleKeyboardAdapter + setOnStartedCallback` code
path. TASK-237 added the `pairing_pin` hardware-config field (null/absent = no auth,
integer 0–999999 = passkey auth) with a three-layer guardrail for test fixtures.

Both tasks were implemented and all host tests pass, but neither has been run on real
hardware yet. This follow-up covers the two on-device verifications that couldn't be
done in the implementation session.

## Acceptance Criteria

- [x] `make test-esp32-ble-config PORT=...` passes end-to-end against an ESP32 flashed
      with the production firmware path (no `BLE_CONFIG_TEST_BUILD`), including the
      test-fixture filesystem upload (verified 2026-04-25, 7/7 PASS — required
      build-fixture fixes: `-Itest/fakes` in the test env, include-order swap in
      `test_main.cpp`, `PLATFORMIO_DATA_DIR` env-var instead of broken
      `pio run -O "data_dir=…"`, and a serial-buffer drain in `runner.py`)
- [x] Flashing production `data/config.json` (with `pairing_pin: 12345`) and pairing
      prompts for the PIN, completes successfully, and subsequent BLE connects reuse
      the bond without re-prompting. Originally written as a manual phone-app step,
      now verified automatically by `make test-esp32-ble-pairing` (added in TASK-247
      after TASK-246 landed the MITM-bit fix). Verified 2026-04-25, 8/8 PASS:
      passkey prompt appears, correct PIN bonds successfully, reconnect reuses bond,
      wrong PIN is rejected with `Failed to pair`.
- [x] Flashing `pairing_pin: null` (test fixture) allows the CLI / runner.py to connect
      and upload profiles without any pairing prompt (verified 2026-04-25 as part of
      the `make test-esp32-ble-config` run above — runner connected and uploaded
      cleanly, no pairing UI involved)

## Test Plan

**On-device tests** (`make test-esp32-ble-config`):

- Run the existing BLE integration test harness against production firmware path
- Requires: ESP32 (NodeMCU-32S) connected via USB, Linux host with BlueZ, `bleak`
  Python package installed

**Manual pairing smoke test**:

- Flash firmware + `data/config.json` (production config with `pairing_pin: 12345`)
- Pair from Flutter app and/or nRF Connect; confirm passkey prompt shows and pairing
  succeeds
- Disconnect and reconnect; confirm bond is reused (no second passkey prompt)
- Reflash with `test/test_ble_config_esp32/data/config.json` (`pairing_pin: null`);
  confirm `make test-esp32-ble-config` runs without any pairing UI appearing

## Prerequisites

- **TASK-236** — delivers the production firmware path for the BLE config integration
  test and the updated runner.py name-prefix discovery that this task exercises
- **TASK-237** — delivers the `pairing_pin` config field and NimBLE security wiring
  that the pairing smoke test verifies

## Notes

This is pure verification work — no code changes expected. If either step reveals a
defect, file a separate defect task rather than fixing it here, so the feature_test
epic stays focused on verification.
