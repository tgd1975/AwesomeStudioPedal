---
id: TASK-127
title: BLE Scanner Screen
status: open
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: MobileApp
order: 9
prerequisites: [TASK-124, TASK-126]
---

## Description

Implement `ScannerScreen` — BLE device discovery UI that consumes `BleService` and provides
actionable error states for Bluetooth-off and permission-denied scenarios. The upload screen
is implemented separately in TASK-145.

## Acceptance Criteria

- [ ] `app/lib/screens/scanner_screen.dart` created
- [ ] Starts BLE scan on mount, filtered by pedal service UUID
- [ ] Shows device name, RSSI bar, and "Connect" button per result
- [ ] On connect: navigates to `HomeScreen` with `BleService.isConnected == true`
- [ ] Handles "Bluetooth off" with an actionable card (link to system Bluetooth settings)
- [ ] Handles "permission denied" with an actionable card (link to app permissions settings)
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/screens/scanner_screen.dart` (new)

## Test Plan

Widget tests covered by TASK-130. Manual: tap "Connect to pedal" on HomeScreen → scan screen
appears → pedal appears in list → tap Connect → HomeScreen shows connected state.

## Prerequisites

- **TASK-124** — BLE connectivity (scan/connect) must exist
- **TASK-126** — `HomeScreen` must exist as the navigation target after connect
