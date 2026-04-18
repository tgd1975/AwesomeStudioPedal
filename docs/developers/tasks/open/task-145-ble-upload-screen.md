---
id: TASK-145
title: BLE Upload Screen with Progress
status: open
opened: 2026-04-18
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: MobileApp
order: 10
prerequisites: [TASK-125, TASK-142, TASK-127, TASK-144]
---

## Description

Implement `UploadScreen` — shows current profile summary, validates before upload, tracks
chunked upload progress, and handles firmware status responses. Consumes `BleService`
(upload methods from TASK-142) and `SchemaService` (TASK-125).

## Acceptance Criteria

- [ ] `app/lib/screens/upload_screen.dart` created
- [ ] Shows current `profiles.json` summary (profile count, last-modified timestamp)
- [ ] "Validate before upload" runs `SchemaService.validateProfiles()` — upload button
  disabled if invalid
- [ ] Upload progress: linear progress bar updated per chunk sent; chunk count / total shown
- [ ] On `CONFIG_STATUS OK`: success snackbar
- [ ] On `CONFIG_STATUS ERROR:<msg>`: error dialog with firmware's message
- [ ] "Upload hardware config" secondary button uploads `config.json` to `CONFIG_WRITE_HW`
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/screens/upload_screen.dart` (new)

## Test Plan

Widget tests covered by TASK-130. Manual: connect via `ScannerScreen` → tap Upload →
verify progress bar advances per chunk → success snackbar appears → LED blinks on device.

## Prerequisites

- **TASK-125** — `SchemaService` for pre-upload validation
- **TASK-142** — chunked upload methods on `BleService`
- **TASK-127** — `ScannerScreen` provides the connected state that gates this screen
- **TASK-144** — `JsonPreviewScreen` provides the JSON preview linked from this screen

## Notes

Use a mock `BleService` during development (before hardware is available) that simulates a
successful 3-chunk upload after 2 seconds with an `OK` status response.
