---
id: TASK-147
title: Android Emulator Setup Guide for App Development
status: closed
closed: 2026-04-18
opened: 2026-04-18
effort: Small (<2h)
complexity: Junior
human-in-loop: No
prerequisites: [TASK-123]
---

## Description

Write a developer guide covering how to install and configure an Android emulator
(Android Studio / AVD Manager) to run and test the Flutter app on a virtual device.
The guide targets Linux and Windows 11 host machines and addresses the key difference
between emulator-based UI development and real-device BLE testing.

The guide lives at `app/docs/EMULATOR_SETUP.md` and is linked from `app/README.md`.

## Acceptance Criteria

- [ ] `app/docs/EMULATOR_SETUP.md` created with separate Linux and Windows 11 sections
- [ ] Covers: Android Studio install, AVD creation (API 34, x86_64 image), enabling
  hardware acceleration (KVM on Linux, HAXM or Hyper-V on Windows), and running
  `flutter run` targeting the emulator
- [ ] Covers BLE limitation: BLE is not available in the Android emulator — document
  the mock `BleService` workaround so developers can still test the full UI flow
  without physical hardware
- [ ] `app/README.md` updated with a "Development" section that links to
  `app/docs/EMULATOR_SETUP.md`

## Test Plan

No automated tests required — change is non-functional (documentation only).

## Prerequisites

- **TASK-123** — Flutter project scaffold must exist; `app/README.md` must be in place
  before it can be updated with the new link.

## Notes

BLE workaround reference: `BleService` can be replaced at the provider level with a
mock implementation that simulates a successful scan result and upload. This is already
used in `test/integration/app_flow_test.dart` via `MockBleService` — the guide should
show how to swap it in during development by using a `kDebugMode` flag or a compile-time
`--dart-define`.

Hardware acceleration notes:

- **Linux (KVM):** `sudo apt install qemu-kvm` + add user to `kvm` group; verify with
  `kvm-ok`. Most modern Linux distros on x86_64 hardware support this out of the box.
- **Windows 11:** Hyper-V is preferred (built-in, no install needed if enabled);
  HAXM is an alternative for machines where Hyper-V conflicts with other hypervisors.
  The guide should note that WSL2 users may need to check Hyper-V is already enabled.

## Documentation

- `app/docs/EMULATOR_SETUP.md` — new file (this is the primary deliverable)
- `app/README.md` — add "Development" section with link to the new guide
