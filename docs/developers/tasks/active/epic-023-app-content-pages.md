---
id: EPIC-023
name: app-content-pages
title: App content pages and connection visibility
status: open
opened: 2026-04-30
closed:
assigned:
branch: feature/config-extensions
---

# App content pages and connection visibility

Grow the Flutter app from configurator-only into a self-explanatory app:
short Info/About, How-To, Profiles explainer, Troubleshooting, and Legal
pages; a startup splash; persistent connection visibility; a Connected-
Pedal read-only summary page; and a Live-keystroke diagnostic page driven
by a generated HID usage display table.

Seeded by IDEA-037 (App content pages — Info/About, How-To, and
supporting screens).

The epic groups three layers of work that share scope but differ in cost:

1. **Pure client-side pages** (Info/About, How-To, Profiles explainer,
   Troubleshooting, Legal, splash, connection status strip) — ship
   without firmware changes.
2. **Connected-Pedal page** — partially shippable today (board identity
   from `kHwIdentityUuid`); the firmware-version, readable-config, and
   live-active-profile parts wait on (3).
3. **Firmware / BLE surfaces** — new read characteristic for firmware
   version, readable config (or chunked readback), active-profile
   notify, and notify-on-send for live keystroke. Splits cleanly into
   its own task and unblocks the Connected-Pedal and Live-keystroke
   pages.

A decision task lands first to resolve the open questions in IDEA-037
(content source of truth, i18n scaffolding, context-aware How-To,
first-run flow) before any UI is built.

## Tasks

Tasks are listed automatically in the Task Epics section of
`docs/developers/tasks/OVERVIEW.md` and in `EPICS.md` / `KANBAN.md`.
