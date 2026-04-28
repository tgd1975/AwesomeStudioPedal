# Known Issues

This page collects environmental quirks, version-pinning gotchas, and platform
caveats that don't fit into a single task or feature doc but are worth
preserving so the next person hitting them doesn't have to re-investigate from
scratch.

For per-feature implementation details, see the other docs in this directory.
For task-tracked work items, see [tasks/OVERVIEW.md](tasks/OVERVIEW.md).

Each entry should describe **symptom**, **affects**, **cause**, **workaround**,
and **status**. If you fix something, delete the entry — this file is for live
issues, not history.

---

## BLE config integration test disconnects on Linux with BlueZ 5.83

**Symptom**

`make test-esp32-ble-config` (and likely `pedal_config.py upload …` over BLE)
fails on the host shortly after `[BLE_TEST] READY` with one of:

```
bleak.exc.BleakDBusError: [org.freedesktop.DBus.Error.NoReply]
    Message recipient disconnected from message bus without replying

bleak.exc.BleakGATTProtocolError: (UNLIKELY_ERROR: 14, 'GATT Protocol Error: Unlikely Error')

bleak.exc.BleakError: failed to discover services, device disconnected
```

The exact form depends on which client-side knobs are in play. The pedal
serial log shows clean boot and `BleConfigService: GATT service registered`,
then nothing — confirming the device never sees the CCC subscribe land.

**Affects**

- Linux hosts running BlueZ ≥ 5.83 (we observed it on Ubuntu's
  `5.83-1~exp1ubuntu0.1` from the experimental pocket).
- `make test-esp32-ble-config` (the bleak-based integration test).
- Likely `scripts/pedal_config.py upload …` from the same host (same path).

**Does NOT affect:**

- The Flutter mobile app (Android/iOS use native BLE stacks, not BlueZ).
- TASK-247's `make test-esp32-ble-pairing` (drives `bluetoothctl` directly,
  bypasses bleak's GATT path).
- Production firmware functionality on the device — boots, loads config,
  buttons fire HID events.
- USB-based filesystem upload (`make uploadfs-esp32`) — bypasses BLE.

**Cause**

A two-layered race in BlueZ + bleak against our specific firmware surface:

1. We advertise the HID service UUID (0x1812) via the BleKeyboard library
   *plus* our custom Config service. BlueZ auto-loads its HID-over-GATT
   profile on connect and reads PnP ID, report references, etc.
2. NimBLE returns ATT 0x0E ("Unlikely Error") for some of those reads while
   the HID profile is still initialising. BlueZ treats 0x0E as a permanent
   failure and tears the link down.
3. Independently, bleak ≥ 2.0 changed CCC subscribe to use `AcquireNotify`
   instead of `StartNotify`; NimBLE peripherals report `NotifyAcquired`
   support unreliably, and AcquireNotify against this firmware can also
   trigger a link drop.

Either layer alone explains some of the failure modes; both stacked is what
makes this slippery.

**Workaround (none reliable yet)**

We tried, and reverted, all of: `bluez={"use_start_notify": True}`,
`services=[SERVICE_UUID]` filter, pre-subscribe `asyncio.sleep`, retry +
reconnect. Each changed the failure mode without fixing it on a Linux box
running BlueZ 5.83. They've been kept out of `runner.py` and `pedal_config.py`
to avoid speculative ballast that hides real future bugs. Re-add them with
evidence if any actually clears the failure on a specific environment.

The closest thing to a clean workaround would be to stop advertising the HID
UUID from the test firmware (so BlueZ doesn't auto-load HID), but that breaks
TASK-236's "test against the production firmware path" property.

**Status**

- **Not blocking the product.** Mobile app users are unaffected, the device
  works, host tests cover the firmware logic. No user-facing capability is
  missing.
- **Reproducible on the maintainer's Linux laptop**, blocking automated
  on-device verification *from that machine*. Pairing test (TASK-247) still
  passes via `bluetoothctl`, so basic BLE Linux↔device works.
- **Not currently filed as a task.** It's an environmental upstream issue, not
  scoped work; if/when someone has bandwidth to dig further, file a task at
  that point with fresh context. Don't pre-create one here just to mark it
  "future" — that's noise.

**References**

- [bluez/bluez#1911](https://github.com/bluez/bluez/issues/1911) — BlueZ
  treats 0x0E as a permanent failure during HID setup; the HID profile
  remains half-initialised and downstream operations fail.
- [hbldh/bleak#1885](https://github.com/hbldh/bleak/issues/1885) — bleak 2.0
  AcquireNotify regression against NimBLE; `use_start_notify` knob added in
  bleak 2.1.0.
- [hbldh/bleak#1075](https://github.com/hbldh/bleak/issues/1075) — same ATT
  0x0E surface from a user-facing angle.
- [zmkfirmware/zmk#3309](https://github.com/zmkfirmware/zmk/issues/3309) —
  ZMK (another NimBLE-based BLE keyboard) hit the same class of issue after
  S3 resume.
