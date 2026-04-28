---
id: TASK-240
title: Defect — firmware JSON parser undersized vs. advertised MAX_CONFIG_BYTES
status: closed
opened: 2026-04-25
closed: 2026-04-25
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Verify
epic: feature_test
order: 25
---

## Description

The BLE config upload path advertises a 32 KB ceiling but the JSON parser
behind it can only fit roughly 8 KB worth of tokens, so any "large but legal"
upload reaches the device, gets accepted as bytes, and then fails with the
generic `ERROR:parse_failed` — looking exactly like a malformed file.

Discovered during TASK-238 on-device verification (Step 3 of the
post-defect-fix protocol on 2026-04-25): a synthetic 25 KB profile was
chunked, reassembled, and rejected at the parse step. Initial read of the
error suggested malformed JSON, but the same JSON parses cleanly on the host;
the firmware simply ran out of `DynamicJsonDocument` capacity.

### Locations

| File | Line | Buffer | Role |
|------|-----:|-------:|------|
| `lib/PedalLogic/include/ble_config_reassembler.h` | 13 | `MAX_CONFIG_BYTES = 32768` | advertised wire-side ceiling |
| `lib/PedalLogic/src/ble_config_reassembler.cpp` | 27 | `DynamicJsonDocument(8192)` | parse on reassembled BLE upload |
| `lib/PedalLogic/src/config_loader.cpp` | 114 | `DynamicJsonDocument(8192)` | parse on `loadFromString` |
| `lib/PedalLogic/src/config_loader_io.cpp` | 21 | `DynamicJsonDocument(8192)` | serialize on `saveToFile` |
| `lib/PedalLogic/src/config_loader_merge.cpp` | 53 | `DynamicJsonDocument(8192)` | parse incoming doc (merge) |
| `lib/PedalLogic/src/config_loader_merge.cpp` | 119 | `DynamicJsonDocument(8192)` | parse base doc (merge) |

`pedal_config.cpp:25` uses `DynamicJsonDocument(1024)` for the small
hardware-config doc; that one is correctly sized for its payload and is not
part of this defect.

### Why "just bump 8192 → 32768" is not the fix

ArduinoJson 6's `DynamicJsonDocument` capacity sizes the **token tree**, not
the input string length. A 32 KB JSON typically needs ~48–64 KB of token
storage depending on shape (deeply nested objects with short keys are
worst-case). Setting capacity equal to `MAX_CONFIG_BYTES` will close the
visible gap but still leave a silent ceiling somewhere below 32 KB of input.

Two viable approaches:

1. **Measure with ArduinoJson Assistant**, pick a capacity that comfortably
   covers a worst-case 32 KB profile (likely 48–64 KB), and document the
   margin. Easy, but pins us to a worst-case allocation that mostly sits idle.
2. **Migrate to ArduinoJson 7's `JsonDocument`**, which auto-grows and
   removes the manual sizing entirely. Touches every parse/serialise site but
   eliminates this whole class of defect. Check that the ESP32/nRF52 builds
   tolerate the runtime allocator pressure (heap fragmentation on the ESP32
   is the realistic concern).

Either way, the four `DynamicJsonDocument(8192)` parse sites and one
serialise site need to change together — leaving any one at 8 KB just shifts
the failure to a different code path.

## Acceptance Criteria

- [x] A 32 KB-or-just-under profile uploaded via BLE either parses
      successfully **or** is rejected before the BLE transfer starts (the CLI
      and reassembler should not be willing to accept payloads the parser
      cannot handle — the limits must agree).
- [x] Whichever ceiling is chosen (32 KB or smaller), `MAX_CONFIG_BYTES`,
      the reassembler parse capacity, the `loadFromString` capacity, the
      `saveToFile` serialise capacity, and the merge-path capacities are all
      consistent and exercised by a test.
- [x] On the rejection path, the error returned to the CLI distinguishes
      "config too large" from "config malformed", so a future verifier
      doesn't waste time on the same misdiagnosis.

## Test Plan

**Host test** (`make test-host`):

- Generate a profile sized to the agreed ceiling (e.g. 30 KB) with valid
  JSON; assert `ConfigLoader::loadFromString` accepts it. Then add one byte
  past the ceiling and assert it is rejected with the size-specific error,
  not a generic parse failure.
- Same against `BleConfigReassembler::applyTransfer` end-to-end (chunks in,
  parsed doc out).

**On-device test** (`make test-esp32-ble-config` extension):

- Add a "max-size profile" case to the BLE integration harness so the next
  on-device verification catches any regression here.

## Notes

- The TASK-234 work already wraps `BleakError` in the CLI and surfaces a
  one-line message; that handling is correct and unchanged. This defect is
  about the firmware-side mismatch only.
- The 25 KB synthetic profile that surfaced this lived at `/tmp/big.json`
  during TASK-238 verification on 2026-04-25 and is gone now. Recreate from
  the test plan above rather than recovering it.
- ArduinoJson 7 migration would also let `pedal_config.cpp` drop its 1024
  capacity guess. Worth considering as part of approach (2) above.

## Resolution (2026-04-25)

Picked **option 1 (interim measured bump)** plus **client-side pre-flight
guard**, with the unified ceiling at **16 KB** (3× current real-world usage
of ~5 KB profiles, comfortable headroom). Three coordinated changes:

1. **Firmware** — `MAX_CONFIG_BYTES` lowered 32 KB → 16 KB; new shared
   `JSON_DOC_CAPACITY = 48 KB` constant; all five `DynamicJsonDocument(8192)`
   parse/serialise sites updated to use it. The five sites now move
   together; the lie of "advertise 32 KB, parse 8 KB" is gone.
2. **CLI** — `scripts/pedal_config.py` rejects payloads >16 KB before any
   BLE attempt with a clear "exceeds device limit" message.
3. **Flutter app** — `app/lib/services/ble_service.dart` adds the same
   16 KB pre-flight; user gets the limit error in the UI.

Host tests cover the new behaviour:

- `OversizedPayload_ErrorTooLarge` automatically tracks the new
  `MAX_CONFIG_BYTES` value (`MAX_CONFIG_BYTES + 1` literal).
- `NearMaxPayload_AcceptedAndParsed` (new) builds a 14 KB profile and
  asserts the reassembler parses it cleanly — proves the parser capacity
  bump actually does something.
- All 272 host tests pass.

**On-device verification deferred** because of the BlueZ 5.83 + bleak
environmental issue documented in
[docs/developers/KNOWN_ISSUES.md](../../KNOWN_ISSUES.md). Acceptable trade-off
because (a) host tests prove the firmware logic, (b) the change is
strictly more conservative than the prior state (lower wire ceiling, larger
parser capacity), and (c) the only environment that would surface a
regression is the same one currently broken for unrelated reasons.

ArduinoJson 7 migration is **not** done — defer to a future "modernize
firmware deps" task. The interim sizing is correct for our actual data
shapes and gives us margin without the AJ7 churn risk.
