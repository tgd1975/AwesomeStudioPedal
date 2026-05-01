---
id: IDEA-053
title: Context-sensitive helper system across the Flutter app
description: Replace the static How-To page with a context-aware helper that surfaces the right guidance for the user's current state (no pedal paired, send failed, profile mismatch, etc.).
category: 📱 apps
---

# Context-sensitive helper system across the Flutter app

EPIC-023 ships a static How-To walkthrough — KISS, easier to write,
easier to test. This idea is the deferred follow-up: a helper layer
that watches the app's state and points the user at the next sensible
action.

## Motivation

A static How-To answers "how do I use this app in general". A
context-sensitive helper answers "what is wrong *right now* and what
do I do next". Examples:

| State | Static How-To says | Context helper would say |
|---|---|---|
| No pedal paired | "Step 1 of 5: pair your pedal…" | A persistent "Tap to pair" prompt on the main screen |
| Send failed mid-write | (nothing — user opens How-To and reads from the top) | Inline banner: "Send interrupted. Reconnect and retry?" with a one-tap retry |
| Profile count mismatch with firmware | (nothing) | Inline note: "Pedal supports 4 profiles, this config has 6 — last 2 will be ignored" |
| First launch, BLE permission denied | (nothing useful) | Direct link to OS settings + one-line explainer |

## Rough approach

- Lightweight state-aware helper widget that subscribes to the same
  app-state streams the configurator already reads (connection status,
  pairing state, last send result, validation errors).
- Each "state" has a short message + zero-or-more action buttons.
- Coexists with — does not replace — the static How-To. The static
  page stays as the "I want to read the whole thing top to bottom"
  surface.
- Probably a `HelperBanner` widget at the top of each screen that
  collapses to a single-line "ⓘ" affordance when there's nothing
  urgent.

## Open questions

- How many distinct helper states are worth modelling vs falling back
  to "open How-To"?
- Should the helper include analytics so we learn which states
  actually fire in the wild? (See project_owner_style — passive
  assets only, no telemetry.)
- Animation / dismissal model — sticky until resolved, or
  user-dismissible?
- Localization interplay — every helper string needs translation
  (see IDEA-052).

## Why deferred

The static How-To is the cheap, KISS path for the initial content-page
ship. The context-sensitive variant requires app-wide state plumbing
and a state catalogue we haven't built yet — easier to add once we
know which "stuck" states real users actually hit.

Decided in TASK-330 (EPIC-023 decision doc).
