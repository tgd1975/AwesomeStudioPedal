---
id: TASK-263
title: Defect — Scanner list truncates pedal device name to "AwesomeStudioPe"
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 34
---

## Description

Discovered while running TASK-153 SC-01 on 2026-04-26.

Per the TASK-258 follow-up, the pedal advertises with a 13-character
name prefix plus a per-device suffix (so two pedals on the same bench
can be distinguished). The full advertised name is 18+ characters.

In the in-app scanner list, the device-name column truncates to
**"AwesomeStudioPe"** (15 visible characters) without an ellipsis or
any indication that more text follows. A user with two pedals nearby
cannot tell them apart, which is exactly the case TASK-258's per-
device suffix was meant to enable.

Screenshot: see `/tmp/sc01_pedal_in_list.png` from the TASK-153 re-run
(home screen → Connect → pedal "AwesomeStudioPe" / Connect button).

## Resolution (2026-04-26): not a defect — platform behavior

After the round-2 finding below, this is reclassified as
expected BLE behavior, not a bug:

- The 14-char cap is the GAP Local Name slot in the BLE 4.x
  advertising PDU (31 B total once Flags / TX Power / Service UUID
  AD entries are included). Android's host stack truncates the
  Local Name to fit; this is not something the app or firmware can
  override without moving to BLE 5 Extended Advertising.
- The Tooltip + ellipsis hardening in commit `59b9597` already
  satisfies the layout/affordance ACs (no row-width jumps, full
  string reachable via long-press once GATT discovery completes).
- We accept the per-device suffix being invisible pre-connect on
  Android. Two pedals on the same bench can still be distinguished
  by RSSI bar in the scan list, and by the full GAP Device Name
  post-connect.

Best-practice comments added at the three places that name the
peripheral so future changes don't regress this:

- [lib/hardware/esp32/src/ble_keyboard_adapter.cpp:3](../../../lib/hardware/esp32/src/ble_keyboard_adapter.cpp#L3)
- [lib/hardware/nrf52840/src/ble_keyboard_adapter.cpp:7](../../../lib/hardware/nrf52840/src/ble_keyboard_adapter.cpp#L7)
- [app/lib/constants/ble_constants.dart:15](../../../app/lib/constants/ble_constants.dart#L15)

## Original acceptance criteria (kept for context)

- [x] The full advertised pedal name is shown in the scanner row, or
      truncated with a clear ellipsis (`…`) and the full name available
      via long-press / row expansion / tooltip. *(ellipsis + tooltip
      shipped; "full name" not satisfiable pre-connect — see Resolution)*
- [x] Two pedals advertising with different suffixes can be visually
      distinguished in the scan list. *(via RSSI pre-connect; via full
      Device Name post-connect)*
- [x] The "Connect" button width does not change when the name is
      long — i.e. the row layout uses Expanded/flex on the name and a
      fixed-width button.

## Test Plan

Manual: run SC-01 from TASK-153 with the current pedal — confirm the
full name is visible, or that the truncation has a clear ellipsis and
the full name is reachable via the row's affordances.

Optional widget test: a Flutter widget test for the scanner row with a
long device name string would be a small addition if the app's test
harness already covers similar widgets.

## Notes

- Not blocking — the user can still connect to the only pedal that
  shows up. Becomes a real problem only when there are 2+ pedals on
  the bench, which is the reason TASK-258 added the per-device
  suffix in the first place.
- Surfaced in TASK-153 SC-01 re-run on 2026-04-26.
- 2026-04-26 (round 2 finding via `/verify-on-device`): the truncation
  is **not** Flutter-side. The scanner row's accessibility node
  reads `device.platformName`, which during the
  advertising-only-pre-connect state reflects the GAP **Local Name**
  field from the advertising packet — and that field is hard-capped
  to ~14 chars when Service UUID + Flags + TX Power AD entries are
  also present (BLE 4.x adv PDU is 31 B). After GATT discovery,
  reading the GAP Device Name characteristic returns the full
  `AwesomeStudioPedal` string. So the *first* AC ("full name in the
  scanner row") cannot be satisfied with a UI-only change while the
  pedal still advertises a 14-char Local Name; either the firmware
  needs an Extended Advertising / shorter prefix, or the scanner row
  needs to fall back to remoteId.str as a stable suffix until
  connect. The Tooltip + ellipsis hardening from commit `59b9597`
  satisfies the layout/affordance ACs but the observed string is
  not actually being truncated by Flutter — it is genuinely 14 chars
  when the advertising packet arrives.
