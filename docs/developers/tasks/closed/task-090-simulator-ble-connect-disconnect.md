---
id: TASK-090
title: Add BLE connect/disconnect toggle to simulator
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
group: Simulator
order: 5
---

## Description

The simulator currently has no concept of a BLE connection state. The BLE LED lights up when
a profile is loaded, but there is no user control to simulate connecting or disconnecting
Bluetooth. On real hardware the firmware suppresses BLE keycodes when no host is connected.

Add a **Connect / Disconnect** toggle button to the simulator so users can test the
"no BLE host connected" state and understand what the pedal does in that condition.

When simulated BLE is disconnected:

- Button presses that produce BLE keycodes are silently dropped (or logged with a warning)
- The BLE LED (`#ble-led`) is shown as off
- The BLE output panel shows a status message (e.g. "⚠ BLE disconnected — keystrokes not sent")

When connected (default after loading a profile):

- Behaviour is unchanged from current implementation
- The BLE LED is lit

## Acceptance Criteria

- [ ] A **Connect BLE** / **Disconnect BLE** toggle button is visible in the toolbar or
      pedal-panel area
- [ ] Initial state after page load is **disconnected** (BLE LED off, no profile loaded)
- [ ] After loading a profile, state transitions to **connected** (BLE LED lit)
- [ ] The toggle button label reflects the current state ("Connect BLE" when disconnected,
      "Disconnect BLE" when connected)
- [ ] With BLE disconnected, pressing a pedal button that would send a BLE keycode logs
      a warning in the BLE output panel instead of the normal keycode
- [ ] `SerialOutputAction` and `PinAction` presses still execute when BLE is disconnected
      (serial log unaffected)
- [ ] Toggling reconnects without needing to reload the profile

## Test Plan

1. Load example profile.
2. Click **Disconnect BLE** → BLE LED goes dark; toggle label becomes "Connect BLE".
3. Press pedal button A → BLE output shows a warning, not a keycode.
4. Click **Connect BLE** → BLE LED lights; toggle label becomes "Disconnect BLE".
5. Press pedal button A → BLE output shows the normal keycode.

## Notes

On real firmware, BLE-only actions are suppressed when `BLEHIDDevice` has no connected host.
`SerialOutputAction` is independent of BLE. This task mirrors that firmware behaviour in the
simulator so users build an accurate mental model before using real hardware.
