---
id: TASK-356
title: Firmware — active-profile-index notify characteristic
status: open
opened: 2026-05-01
effort: Small (<2h)
complexity: Medium
human-in-loop: Clarification
epic: app-content-pages
order: 14
prerequisites: [TASK-353]
---

## Description

Add a small read+notify BLE characteristic that tracks the active
profile index, so the app can reflect SELECT-button profile
switches in real time without re-reading the whole config.

What ships:

- Read+notify char with a single-byte payload (active profile
  index, 0..N-1).
- Notification fires within one BLE connection-interval of the
  profile actually changing in firmware. The hook lives at
  whatever event boundary already exists for "active profile
  changed" (the SELECT button's profile-switch handler).

App-side:

- Subscribe to the notify on connect; expose
  `BleService.activeProfileStream` so the configurator and the
  Connected-Pedal page can observe live profile state.

## Acceptance Criteria

- [ ] Read+notify characteristic exists on both ESP32 and nRF52840.
- [ ] Notification fires within one BLE connection-interval of a
      profile switch (verified via on-device test + observed
      timestamp delta).
- [ ] App `BleService` exposes a stream of active-profile indices
      that the rest of the app can `context.watch` against.
- [ ] No new consumer UI is required for this task to land — it's
      infrastructure for whichever surface needs it next. The
      Connected-Pedal page's "current profile" row may pick this
      up in a follow-up; not in scope here.

## Prerequisites

- **TASK-353** — feasibility analysis covers the per-platform
  notify mechanics and the firmware event-boundary location.

## Test Plan

**On-device tests** — extend the existing BLE test on each target
to subscribe to the notify, trigger a profile switch (via the
test rig's button-press injection or whatever the existing
profile-switch test uses), and assert the notification arrives
with the correct payload within the one-connection-interval
budget.

**Host tests** — the notification-emission logic in firmware is
mostly hardware-bound; don't force a host-side test here.

**App-side unit test** — extend `BleService` tests to verify
`activeProfileStream` re-emits on subsequent notify events
without leaking subscriptions.

## Notes

- This was carved out of the original TASK-337 — independent of
  the version-char and config-readback work, low risk, but with
  no immediate UI consumer. Sized Small because the firmware
  side is a small char + a one-line emit at the existing
  profile-switch site, and the app side is a stream subscription.
- Could legitimately defer until a UI consumer exists. Decided at
  TASK-353 time whether to ship now or park.
- Hardware: both ESP32 and nRF52840 connected via USB for the
  matching test runs.
