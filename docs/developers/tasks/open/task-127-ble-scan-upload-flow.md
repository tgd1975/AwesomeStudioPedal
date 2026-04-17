---
id: TASK-127
title: BLE Scan Screen and Upload Flow with Progress
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: MobileApp
order: 6
prerequisites: [TASK-126, TASK-118]
---

## Description

Implement `ScannerScreen` (BLE device discovery) and `UploadScreen` (upload progress and status). Both screens consume `BleService` and provide actionable error states for Bluetooth-off and permission-denied scenarios.

## Acceptance Criteria

### ScannerScreen
- [ ] Starts BLE scan on mount, filtered by pedal service UUID
- [ ] Shows device name, RSSI bar, and "Connect" button per result
- [ ] On connect: navigates to `HomeScreen` with `BleService.isConnected == true`
- [ ] Handles "Bluetooth off" with an actionable card (link to system Bluetooth settings)
- [ ] Handles "permission denied" with an actionable card (link to app permissions settings)

### UploadScreen
- [ ] Shows current `profiles.json` summary (profile count, last-modified timestamp)
- [ ] "Validate before upload" runs `SchemaService.validateProfiles()` — upload button disabled if invalid
- [ ] Upload progress: linear progress bar updated per chunk sent; chunk count / total shown
- [ ] On `CONFIG_STATUS OK`: success snackbar
- [ ] On `CONFIG_STATUS ERROR:<msg>`: error dialog with firmware's message
- [ ] "Upload hardware config" secondary button uploads `config.json` to `CONFIG_WRITE_HW`

## Files to Touch

- `app/lib/screens/scanner_screen.dart` (new)
- `app/lib/screens/upload_screen.dart` (new)

## Test Plan

**Widget tests**: covered by TASK-130.
Manual: scan for pedal → connect → upload → verify success snackbar and LED blink on device.

## Prerequisites

- **TASK-126** — `HomeScreen` must exist as the navigation target after connect
- **TASK-118** — firmware BLE service must be live for end-to-end upload testing

## Notes

Use `BleService` as a `ChangeNotifier` via `Provider` — screens listen to `isConnected` and re-build accordingly. For development before TASK-118 is complete, use a mock `BleService` that simulates a successful upload after 2 seconds.
