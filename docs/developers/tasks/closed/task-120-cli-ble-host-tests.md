---
id: TASK-120
title: Host and Unit Tests for CLI and BLE Reassembly
status: closed
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: BLEConfig
order: 6
prerequisites: [TASK-115, TASK-116, TASK-119]
---

## Description

Write host-side tests for two components: (1) the Python CLI tool's chunking and schema validation logic (no BLE hardware required), and (2) `BleConfigService`'s reassembly, error-handling, and LED blink logic using GoogleTest with fake dependencies.

## Acceptance Criteria

### Python CLI tests (`scripts/tests/test_pedal_config.py`)
- [ ] Schema validation accepts the shipped `data/profiles.json`
- [ ] Schema validation rejects a profile with a missing `type` field
- [ ] Schema validation rejects a profile with an invalid action type string
- [ ] Chunked write splits a 1 KB payload into correct MTU-sized packets with sequential 2-byte big-endian sequence numbers
- [ ] `upload-config` targets the hardware config characteristic (not profiles characteristic)

### C++ `BleConfigService` host tests (`test/unit/test_ble_config_service.cpp`)
- [ ] **Happy path:** feed N sequential chunks; assert `isApplying()` true during transfer and false after last chunk; assert `configureProfiles()` called exactly once with reassembled JSON
- [ ] **Out-of-order chunk:** inject chunk with wrong sequence number; assert `ERROR:bad_sequence` notified; transfer resets
- [ ] **Oversized payload:** feed more chunks than `MAX_CONFIG_BYTES` allows; assert `ERROR:too_large`; no `configureProfiles()` call
- [ ] **Invalid JSON:** complete transfer with malformed JSON; assert `ERROR:parse_failed`
- [ ] **Concurrent upload rejection:** begin transfer, start second transfer before completion; assert `BUSY` notified
- [ ] **LED blink — success:** after successful apply, 3 on/off cycles verified, then `updateLEDs()` called
- [ ] **LED blink — failure:** single long blink on parse failure
- [ ] All C++ tests pass under `make test-host`

## Files to Touch

- `scripts/tests/test_pedal_config.py` (new)
- `test/unit/test_ble_config_service.cpp` (new)
- `test/CMakeLists.txt` — add `test_ble_config_service.cpp` and `ble_config_service.cpp` to pedal_tests

## Test Plan

Python: `python -m pytest scripts/tests/` — all cases pass.
C++: `make test-host` — all cases pass.

## Prerequisites

- **TASK-115** — schema fixtures needed for Python tests
- **TASK-116** — hardware config schema fixtures needed for `upload-config` test
- **TASK-119** — CLI must be implemented before its tests are written

## Notes

Use a fake `IFileSystem` and a `MockProfileManager` stub (GoogleTest mock) for the C++ tests. The fake filesystem's `write()` captures the JSON string for assertion. Mock the LED controller to verify blink call counts and timing.
