---
id: TASK-335
title: Add connection status strip and details sheet
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
epic: app-content-pages
order: 6
---

## Description

Make the app's current BLE connection always visible. From IDEA-037:

- Persistent status strip under the app bar showing device name, short
  ID/MAC suffix, and connection state (scanning / connected /
  disconnected).
- Tapping the strip opens a "Connection details" bottom sheet with:
  - Device name and full identifier.
  - Firmware version (from the firmware version characteristic; falls
    back to "—" until TASK-337 lands).
  - Signal strength (RSSI) if cheap to obtain.
  - Disconnect / forget actions.
- Same info reachable from the Info/About page (TASK-331) so users can
  copy it into a bug report.

Works with what BLE exposes today; the firmware-version field is
optional until TASK-337.

## Acceptance Criteria

- [ ] Status strip renders in all three states (scanning, connected,
      disconnected) and updates from the existing connection-state
      stream.
- [ ] Tapping the strip opens the details sheet; sheet shows all
      available fields and gracefully marks unavailable ones (e.g.
      firmware version pre-TASK-337) with "—" rather than crashing.
- [ ] Disconnect and forget actions invoke the existing BLE service
      methods (no new API surface) and reflect their result in the
      connection state.
- [ ] Info/About page (TASK-331) gains a "Connection" subsection or
      link that mirrors the same fields.

## Test Plan

**Host tests** (Flutter widget tests):

- Add `app/test/widgets/connection_status_strip_test.dart` and
  `connection_details_sheet_test.dart`.
- Cover: each connection state renders the right strip; tapping opens
  the sheet; disconnect button calls the mocked BLE service exactly
  once; missing firmware-version field renders "—" without throwing.

**On-device verification**:

- `/verify-on-device` happy-path: connect, observe strip → open sheet
  → disconnect from sheet → observe strip transitions to disconnected.

## Notes

- Pre-TASK-337, the firmware-version row stays "—" — do not block this
  task on the firmware work.
- RSSI is optional. If the current BLE stack doesn't expose it cheaply,
  drop the row rather than adding a polling timer.
