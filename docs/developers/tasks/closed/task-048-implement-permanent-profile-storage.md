---
id: TASK-048
title: Implement permanent profile storage
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
---

## Description

Persist the active profile selection to non-volatile storage so the pedal remembers the
last-used profile across power cycles.

## Acceptance Criteria

- [x] Active profile index saved to NVS/Preferences on every profile change
- [x] Saved profile loaded on boot
- [x] Falls back to profile 0 if no saved value exists
- [x] ESP32-specific `Preferences` API guarded behind `#ifdef ESP32`
- [x] Host unit tests pass (storage calls mocked via interface)

## Notes

Delivered in commit `4a8f8d6`. The `#ifdef ESP32` guard (commit `4c8c461`) was needed to
keep the host build compiling without the ESP32 SDK.
Uses the `IProfileStorage` interface to allow host-side testing with a fake.
