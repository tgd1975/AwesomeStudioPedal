---
id: TASK-124
title: BLE Service Layer — Scan, Connect, Disconnect
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Medium (2-8h)
effort_actual: Large (8-24h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: MobileApp
order: 3
prerequisites: [TASK-123, TASK-117]
---

## Description

Implement the connectivity half of `BleService` — a `ChangeNotifier` that wraps
`flutter_blue_plus` and exposes scan, connect, disconnect, and status stream operations.
UUIDs must match the GATT spec from TASK-117 exactly.

The chunked upload protocol is implemented separately in TASK-142.

## Acceptance Criteria

- [ ] `app/lib/services/ble_service.dart` created with the interface below
- [ ] `app/lib/constants/ble_constants.dart` created — defines service UUID, `CONFIG_WRITE`,
  `CONFIG_STATUS`, `CONFIG_WRITE_HW` characteristic UUIDs from TASK-117 spec
- [ ] `scan()` lists BLE devices advertising the pedal service UUID; filters out non-pedal devices
- [ ] `connect(device)` establishes a BLE connection and discovers services
- [ ] `disconnect()` cleanly disconnects
- [ ] `statusStream` exposes a `Stream<String>` of `CONFIG_STATUS` notifications
- [ ] `isConnected` getter returns current connection state
- [ ] Handles "Bluetooth off" and "permission denied" gracefully — exposes error state via
  `notifyListeners()`
- [ ] `flutter analyze` passes

## Interface

```dart
class BleService extends ChangeNotifier {
  Future<List<ScanResult>> scan({Duration timeout});
  Future<void> connect(BluetoothDevice device);
  void disconnect();
  Stream<String> get statusStream;
  bool get isConnected;
}
```

## Files to Touch

- `app/lib/services/ble_service.dart` (new)
- `app/lib/constants/ble_constants.dart` (new)

## Test Plan

Unit tests for connectivity behaviour covered by TASK-130. Manual: scan shows pedal in list,
connect transitions `isConnected` to true.

## Prerequisites

- **TASK-123** — Flutter project must exist
- **TASK-117** — UUID constants from GATT spec

## Notes

Upload methods (`uploadProfiles`, `uploadConfig`) are stubbed out or left as `TODO` here —
they are implemented in TASK-142. The stub should throw `UnimplementedError` so callers fail
clearly rather than silently.
