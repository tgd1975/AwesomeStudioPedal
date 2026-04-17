---
id: TASK-124
title: BLE Service Layer (Scan, Connect, Chunked Upload)
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: MobileApp
order: 3
prerequisites: [TASK-123, TASK-117]
---

## Description

Implement `BleService` — a `ChangeNotifier` that wraps `flutter_blue_plus` and exposes scan, connect, and chunked upload operations. UUIDs must match the GATT spec from TASK-117 exactly.

## Acceptance Criteria

- [ ] `app/lib/services/ble_service.dart` created with the interface below
- [ ] `app/lib/constants/ble_constants.dart` created — defines service UUID, `CONFIG_WRITE`, `CONFIG_STATUS`, `CONFIG_WRITE_HW` characteristic UUIDs from TASK-117 spec
- [ ] `scan()` lists BLE devices advertising the pedal service UUID; filters out non-pedal devices
- [ ] `connect(device)` establishes a BLE connection and discovers services
- [ ] `disconnect()` cleanly disconnects
- [ ] `uploadProfiles(json)` chunks the JSON string into 510-byte payloads, prepends 2-byte big-endian sequence numbers, writes each chunk to `CONFIG_WRITE`, then subscribes to `CONFIG_STATUS` for `OK` / `ERROR:<msg>`
- [ ] `uploadConfig(json)` same but targets `CONFIG_WRITE_HW`
- [ ] `statusStream` exposes a `Stream<String>` of `CONFIG_STATUS` notifications
- [ ] `isConnected` getter returns current connection state
- [ ] Handles "Bluetooth off" and "permission denied" gracefully — exposes error state via `notifyListeners()`
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/services/ble_service.dart` (new)
- `app/lib/constants/ble_constants.dart` (new)

## Test Plan

**Unit tests**: covered by TASK-130 (chunking logic tested with mock BLE).

## Prerequisites

- **TASK-123** — Flutter project must exist
- **TASK-117** — UUID constants from GATT spec

## Notes

```dart
class BleService extends ChangeNotifier {
  Future<List<ScanResult>> scan({Duration timeout});
  Future<void> connect(BluetoothDevice device);
  void disconnect();
  Future<UploadResult> uploadProfiles(String json);
  Future<UploadResult> uploadConfig(String json);
  Stream<String> get statusStream;
  bool get isConnected;
}
```

Chunking: `utf8.encode(json)` → split into 510-byte chunks → prepend `Uint8List(2)` big-endian sequence counter → write via `characteristic.write(chunk, withoutResponse: true)`.
