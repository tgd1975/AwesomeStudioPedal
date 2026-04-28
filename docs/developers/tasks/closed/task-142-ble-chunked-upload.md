---
id: TASK-142
title: BLE Service Layer — Chunked Upload Protocol
status: closed
closed: 2026-04-18
opened: 2026-04-18
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: No
epic: MobileApp
order: 4
prerequisites: [TASK-124]
---

## Description

Extend `BleService` with the chunked upload protocol for profiles and hardware config.
Chunking logic, sequence numbering, and status subscription must match the GATT spec
(TASK-117) and the Python CLI implementation (TASK-119) exactly.

## Acceptance Criteria

- [ ] `uploadProfiles(String json)` implemented in `BleService`
- [ ] `uploadConfig(String json)` implemented in `BleService`
- [ ] Both methods: `utf8.encode(json)` → split into 510-byte chunks → prepend 2-byte
  big-endian sequence counter starting at `0x0000`
- [ ] Final packet: sequence `0xFFFF`, empty payload (end-of-transfer signal per spec)
- [ ] Each chunk written to the correct characteristic (`CONFIG_WRITE` / `CONFIG_WRITE_HW`)
  with `withoutResponse: true`
- [ ] After last chunk: subscribes to `CONFIG_STATUS` notify; resolves with `UploadResult`
  on `OK` or `ERROR:<msg>`
- [ ] `UploadResult` type defined with `success`, optional `errorMessage`
- [ ] Replaces the `UnimplementedError` stubs left by TASK-124
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/services/ble_service.dart` — add upload methods
- `app/lib/models/upload_result.dart` (new)

## Test Plan

Unit test covered by TASK-130: 1 500-byte payload → 3 chunks, each with correct 2-byte
big-endian sequence numbers (0x0000, 0x0001, 0xFFFF end packet).

## Prerequisites

- **TASK-124** — BLE connectivity and `BleService` class must exist

## Notes

Chunking reference from TASK-117 spec:

```dart
// Encoding:
final bytes = utf8.encode(json);
// Chunk:
for (int i = 0; i * 510 < bytes.length; i++) {
  final payload = bytes.sublist(i * 510, min((i + 1) * 510, bytes.length));
  final seq = Uint8List(2)..buffer.asByteData().setUint16(0, i, Endian.big);
  await characteristic.write([...seq, ...payload], withoutResponse: true);
}
// End-of-transfer:
final eof = Uint8List(2)..buffer.asByteData().setUint16(0, 0xFFFF, Endian.big);
await characteristic.write(eof, withoutResponse: true);
```
