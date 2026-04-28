---
id: TASK-247
title: Automate BLE pairing smoke test via bluetoothctl (Linux)
opened: 2026-04-25
closed: 2026-04-25
status: closed
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Small
human-in-loop: Verify
epic: feature_test
order: 27
prerequisites: [TASK-246]
---

## Description

The current TASK-238 verification flow has two halves:

1. **Automated** — `make test-esp32-ble-config` drives the BLE upload protocol
   end-to-end via `runner.py` against the production firmware path. No human
   in the loop. Works on Linux (BlueZ + bleak).
2. **Manual** — the pairing PIN smoke test (with `pairing_pin: 12345`)
   currently expects the verifier to open a phone, find the pedal, tap pair,
   type the PIN, then disconnect and reconnect to confirm bond reuse. Slow,
   error-prone, and impossible to run in CI.

During TASK-238 verification on 2026-04-25 we drove the manual half from
`bluetoothctl` instead of a phone. Linux exposes the entire pair / enter
passkey / verify-bond / remove-bond cycle as scriptable shell commands:

```bash
# Clean both sides
bluetoothctl remove 24:62:AB:D4:E0:D2
python3 esptool.py --port /dev/ttyUSB0 erase_region 0x9000 0x5000   # NimBLE bond store

# Pair with passkey-supplying agent
{
  echo 'agent KeyboardOnly'; echo 'default-agent'
  echo 'pair 24:62:AB:D4:E0:D2'
  sleep 8; echo '012345'                  # passkey, fed when prompted
  sleep 5; echo 'info 24:62:AB:D4:E0:D2'  # assert Paired: yes, Bonded: yes
  echo 'quit'
} | bluetoothctl
```

That recipe — once cleaned up and wrapped — gives us a fully automated
pairing smoke test on Linux with no human in the loop. macOS has
`blueutil` which exposes a similar surface; Windows has no first-class
BLE-pairing CLI shipped with the OS, so it stays a documented manual
procedure there.

## Acceptance Criteria

- [x] New `make` target `test-esp32-ble-pairing` runs the full pairing
      cycle on Linux via `bluetoothctl`:
      1. Erases the host bond (`bluetoothctl remove`).
      2. Erases the pedal NVS (`esptool erase_region`) so NimBLE forgets
         its side of the bond.
      3. Reflashes production firmware + filesystem (`pairing_pin: 12345`).
      4. Pairs via a `bluetoothctl` agent that supplies passkey `012345`;
         asserts `Pairing successful`.
      5. Asserts `Paired: yes, Bonded: yes` from `bluetoothctl info`.
      6. Disconnects and reconnects; asserts no second passkey prompt
         (bond reused).
      7. Re-runs the pair flow with a *wrong* passkey; asserts
         `Failed to pair` and that no bond was created.
      8. Teardown: removes the bond, leaves the host clean.
- [x] On Linux without `bluetoothctl` or `pexpect`, the runner prints a
      clear install message and exits non-zero — does not silently succeed.
- [x] [TESTING.md](../../TESTING.md) gains a "test-esp32-ble-pairing"
      section describing what the runner does and what it requires.

Windows / macOS coverage is tracked in **TASK-248** and is out of scope here.

## Test Plan

**Linux (automated)**:

- Run on a clean ESP32 with both sides un-paired; assert exit 0 and
  `bluetoothctl info` shows `Paired: yes, Bonded: yes` after step 5.
- Run a second time without cleanup; assert the script's own `remove` /
  `erase_region` step succeeds even when the device was already paired.
- The wrong-passkey scenario is part of the runner itself (step 7) — its
  failure to reject would surface as a runner exit code 1.

## Notes

- This task depends on **TASK-246** because the current firmware never
  raises the MITM bit, so a passkey-pairing test would pass Just-Works
  and silently report success. Land TASK-246 first, then this work has
  something real to assert against.
- The `bluetoothctl` agent's passkey input via piped stdin worked in the
  TASK-238 session but the prompt timing is fragile. Consider using
  `dbus-send` or the `bluezero` Python package (which speaks BlueZ DBus
  directly) for a cleaner version that doesn't depend on `sleep` between
  commands.
- The NVS erase recipe (`erase_region 0x9000 0x5000`) matches the layout
  in `config/esp32/partitions.csv`; if that file changes, the offsets here
  must move with it.
- TASK-238's manual smoke-test wording can be retired (or at least
  shortened to "see TASK-247 automated target") once this lands.
