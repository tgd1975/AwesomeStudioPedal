---
id: TASK-233
title: Defect — CLI upload and firmware both accept schema-invalid profiles.json
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: No
epic: feature_test
order: 21
---

## Description

The test plan FEATURE_TEST_PLAN.md §1.3 U-04 expects:

> Upload a profiles.json that fails schema validation → CLI reports error
> before connecting to pedal; no upload attempt.

Neither layer enforces this today:

1. **CLI** — `scripts/pedal_config.py upload` (`cmd_upload` →
   `_upload`) reads the file bytes, chunks them, and writes them to the
   Config characteristic **without any pre-flight validation**. The
   `validate` subcommand is separate and unrelated to the `upload`
   subcommand. A user running `upload foo.json` can push a completely
   invalid file and the CLI never complains.

2. **Firmware** — `BleConfigReassembler::applyTransfer` calls
   `ConfigLoader::loadFromString`, which parses the JSON loosely. A
   profile entry with no `buttons` field (a schema violation) loads
   without error; the firmware accepts the upload, responds `OK`, and
   the user is left with a profile that has no attached actions.

Demonstrated U-04 reproduction:

```bash
$ echo '{"profiles":[{"name":"x"}]}' > /tmp/bad.json
$ python scripts/pedal_config.py validate /tmp/bad.json
  profiles > 0: 'buttons' is a required property
FAIL: 1 schema error(s) in /tmp/bad.json       # exit 1 — good

$ python scripts/pedal_config.py upload /tmp/bad.json
Payload: 36 bytes → 2 chunks (incl. sentinel)
Scanning for pedal…
Found: AwesomeStudioPe (24:62:AB:D4:E0:D2)
Uploading 2 packets…
OK: upload successful                           # exit 0 — bug
```

## Acceptance Criteria

- [ ] `pedal_config.py upload` validates the input file against
      `data/profiles.schema.json` before opening the BLE connection. On
      failure, print the same error format as `validate` and exit with a
      non-zero code — no scan, no connect.
- [ ] `pedal_config.py upload-config` validates against
      `data/config.schema.json` before opening the BLE connection (same
      semantics). It already reads the hardware field; extend that to a
      full schema check.
- [ ] Firmware (`BleConfigReassembler::applyTransfer`) also rejects
      profiles that are schema-malformed, or at a minimum rejects profiles
      without a `buttons` field on each profile entry. Return
      `ERROR:schema` via the status notification so the CLI can surface it.
- [ ] Re-run FEATURE_TEST_PLAN.md U-04 — exit non-zero, no connect.

## Test Plan

**Host tests** (`make test-host`):

- Python: add to `scripts/tests/test_pedal_config.py` a case that
  mocks `_upload` (so no BLE is touched), calls `cmd_upload` with a
  schema-invalid profiles.json, and asserts exit 1 with the schema
  error in stderr. Add a matching case for `cmd_upload_config`.
- C++: extend `test/unit/test_config_loader.cpp` (or similar) with a
  case that asserts `loadFromString` on a buttons-less profile fails
  (once the firmware guard is added).

## Notes

- Surfaced during TASK-151 U-04, U-06, and TASK-152 C-03 feature test
  execution. All three are now **FAIL** in FEATURE_TEST_PLAN.md until
  this lands — same root cause: CLI skips validation and firmware is
  too permissive.
  - U-04 (missing `buttons` field on a profile): schema would catch it
    if CLI ran validate pre-upload; firmware also accepts.
  - U-06 (`{"profiles": []}`): schema catches; firmware accepts and
    ends up with zero profiles — no button does anything.
  - C-03 (`config.json` with `numButtons: 5` but 4 `buttonPins`):
    schema does NOT catch this (missing cross-field constraint) and
    firmware also accepts. This is the most dangerous of the three
    because the missing pin defaults to GPIO 0 — the ESP32 BOOT pin
    — and attaching an interrupt to it can interfere with boot.
- Fix also needs a schema change: `config.schema.json` should encode
  the `len(buttonPins) == numButtons`, `len(ledSelect) == numSelectLeds`
  invariants (e.g., via `if/then` or a separate cross-field validator in
  the CLI) so the validator catches these cross-field mismatches, not
  just structural ones.
- Related to TASK-184 (closed: "schema-defect action value/pin fields not
  required") in that both are schema-vs-firmware mismatches; after this
  fix lands, do a broader pass to close the gap between the JSON schema
  in `data/profiles.schema.json` and what `loadFromString` actually
  accepts.
