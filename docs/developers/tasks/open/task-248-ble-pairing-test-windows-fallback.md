---
id: TASK-248
title: BLE pairing test — Windows manual fallback (and macOS if a host appears)
opened: 2026-04-25
status: open
effort: Small (<2h)
complexity: Small
human-in-loop: Main
epic: feature_test
order: 2
prerequisites: [TASK-247]
---

## Description

[TASK-247](task-247-automate-ble-pairing-test-via-bluetoothctl.md) ships a
Linux-only `make test-esp32-ble-pairing` that automates the pairing-PIN
smoke test via `bluetoothctl`. Windows and macOS have no equivalent first-class
CLI shipped with the OS, so the same coverage there has to come from either:

1. A documented manual procedure that produces the same evidence, or
2. A separate small automation that uses whatever per-OS tooling is
   available — `blueutil` on macOS, PowerShell + `Windows.Devices.Bluetooth`
   on Windows.

We don't have a macOS dev host on the project today (project owner runs
Windows + Ubuntu only — see CLAUDE.md), so macOS coverage is a "if a host
appears" item. Windows coverage is the real gap.

## Acceptance Criteria

- [ ] Step-by-step Windows manual procedure documented in
      [TESTING.md](../../TESTING.md#test-esp32-ble-pairing) (or a sibling
      page, your choice). Procedure must specify: how to forget any
      previous bond, how to trigger pairing from Windows Settings, what
      the prompt looks like, how to verify reconnect reuses the bond, how
      to verify a wrong passkey is rejected.
- [ ] `make test-esp32-ble-pairing` on Windows (MSYS2 / Git Bash) detects
      the platform and either runs an automated PowerShell variant or
      prints a clear "see TESTING.md#... for the manual procedure" with a
      non-zero exit so CI doesn't silently pass.
- [ ] If a PowerShell automation is feasible within the effort budget,
      ship it and reuse the same scenario assertions as
      `test_ble_pairing_esp32/runner.py` (right-passkey, bond-reuse,
      wrong-passkey).
- [ ] macOS path: leave a one-liner stub (skip-with-message) and a note
      pointing at `blueutil` so a future contributor knows what to wire
      up. No need to implement until a macOS host exists.

## Test Plan

**Manual on Windows**:

- Follow the documented procedure on a clean Windows install (or after
  removing any existing pedal pairing). Confirm each step produces the
  documented outcome.
- Repeat with a wrong PIN; assert Windows surfaces the failure clearly.

**If PowerShell automation lands**:

- Same three scenarios as the Linux runner: right passkey, reconnect,
  wrong passkey. Cross-check that the exit codes match.

## Notes

- The `pairing_pin` value (`12345` → 6-digit display `012345`) and the
  Spaceballs reference are project conventions; mirror them in the
  Windows docs.
- NVS-erase on Windows still uses `esptool.py` from the PlatformIO Python
  env, so that part is platform-portable.
- If we ever take macOS seriously, `blueutil` ships via Homebrew; the
  pairing surface is `blueutil --pair <addr>` plus an interactive
  passkey prompt. Worth checking whether that prompt is scriptable.
