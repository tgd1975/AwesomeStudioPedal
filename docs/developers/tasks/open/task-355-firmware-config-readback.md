---
id: TASK-355
title: Firmware — config readback (option chosen in TASK-353)
status: open
opened: 2026-05-01
effort: Medium (2-8h)
complexity: Senior
human-in-loop: Clarification
epic: app-content-pages
order: 13
prerequisites: [TASK-353]
---

## Description

Make the pedal's currently-running configuration readable over BLE
so the Connected-Pedal page (TASK-336) can fill its
currently-placeholdered Configuration and Storage rows.

The implementation path — option (a) make existing config
characteristic readable, retaining the canonical JSON in RAM, vs
option (b) inverted chunked reassembler with no resident copy — is
chosen in TASK-353. That decision is reflected here at activation,
not now.

App-side:

- Wire `BleService.readActiveConfig()` (or equivalent) and surface
  the result on the Connected-Pedal page's Configuration row,
  replacing the `_PendingRow` widget. Storage row uses the same
  data (size of the resident config / total flash budget for
  config) — same call, different presentation.

## Acceptance Criteria

- [ ] Config readback works on both ESP32 and nRF52840 per the
      option chosen in TASK-353.
- [ ] App reads back exactly what was written for at least the
      example configs in `profiles/`.
- [ ] nRF52840 RAM headroom is at or above the threshold documented
      in TASK-353 — no regression beyond the budget.
- [ ] Connected-Pedal page Configuration row is no longer a
      placeholder; Storage row renders something useful (size of
      resident config / total budget), or stays "—" with a
      one-line rationale committed in this task's Notes.
- [ ] On-device test verifies a round-trip (write → read → equal)
      on at least one target.

## Prerequisites

- **TASK-353** — feasibility analysis pinpoints option (a vs b)
  and the nRF52840 RAM headroom number this task must respect.

## Test Plan

**On-device tests** — `make test-esp32-ble` / matching nRF52840
target. Extend with a "round-trip equality" test: write a known
config, read it back, assert byte-equal at the JSON level.

**Host tests** — if option (b) is chosen, the chunked
reassembler logic is host-testable behind `HOST_TEST_BUILD`. Add
`test/unit/test_config_readback.cpp`. If option (a) is chosen,
host coverage is limited to "resident copy stays correct after
write" which is mostly covered by existing tests; document the
gap.

**App-side widget test** — extend
`test/widget/connected_pedal_screen_test.dart` to assert the
Configuration row reflects mocked `readActiveConfig()` data.

## Notes

- This was carved out of the original TASK-337 — the largest of
  the three deliverables and the one with the real design
  decision (option a vs b) and the real risk surface (nRF52840
  RAM budget).
- Hardware: both ESP32 and nRF52840 connected via USB for the
  matching test runs.
