---
id: TASK-336
title: Add Connected-Pedal page (board only; firmware/config sections placeholdered)
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
epic: app-content-pages
order: 7
---

## Description

Add a read-only "Connected pedal" page that answers "what is on the
pedal right now?". Initial scope is what the **current** firmware
already exposes; firmware-version, current-config readback, and
storage usage are placeholdered until TASK-337 ships them.

What ships in this task:

- **Board** — identity string from the HW Identity characteristic
  (`kHwIdentityUuid`), already exposed today (`esp32` / `nrf52840`).
- **Firmware** — placeholder `—` row with a tooltip "available after
  firmware update". Filled in by TASK-337's follow-up.
- **Current configuration** — placeholder `—` row, same tooltip.
- **Storage** — placeholder `—` row, same tooltip.

Reachable from the connection status strip / details sheet (TASK-335)
and from the Info/About page.

## Acceptance Criteria

- [ ] Page reachable from at least the connection details sheet and
      Info/About.
- [ ] Board row renders the identity string from `kHwIdentityUuid` and
      handles the disconnected state by showing "—".
- [ ] Placeholder rows are present but clearly marked as
      pending-firmware so users know this is intentional, not broken.
- [ ] Page fits one phone screen on Pixel 9.

## Test Plan

**Host tests** (Flutter widget tests):

- Add `app/test/pages/connected_pedal_page_test.dart`.
- Cover: connected → board row matches the mocked identity value;
  disconnected → all rows show "—"; placeholder rows render the
  pending-firmware tooltip.

## Notes

- The Connected-Pedal page is intentionally split from the firmware
  work in TASK-337 so app-side progress doesn't block on firmware
  changes (per IDEA-037: "the Connected-Pedal page is the part that
  pulls in firmware work, and is worth splitting into its own
  follow-up task").
- TASK-337's follow-up will return here to wire the firmware/config/
  storage rows once the BLE characteristics exist; that is a small
  re-visit, not a rewrite.
