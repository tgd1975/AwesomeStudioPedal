---
id: TASK-092
title: Load example data as default in simulator and builders
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
epic: Simulator
order: 7
---

## Description

All three web tools — the simulator, the profile builder, and the configuration builder —
currently open with an empty/blank state. A first-time user sees nothing and may not
understand what the tool does until they load or create data.

Load a built-in example as the default state on page open so every tool is immediately
useful out of the box.

| Tool | Default example |
|---|---|
| Simulator | `example.json` (existing profiles.json example) |
| Profile Builder | A single example profile with four buttons, one per action type |
| Configuration Builder | Default ESP32 wiring matching `src/config_esp32.cpp` |

The example must be **inline** (not fetched from the network) to work offline and avoid the
NetworkError bug fixed by TASK-086.

A visible **"Example loaded — load your own file to replace"** notice should appear until
the user loads their own file, so it is clear the data shown is not theirs.

## Acceptance Criteria

- [ ] **Simulator**: opens with the example profile pre-loaded; toolbar shows
      "example (default) — load your own profiles.json to replace" until a custom file is
      loaded; `Load Example` button still works to reset back to the example
- [ ] **Profile Builder**: opens with a single example profile pre-populated in the form and
      JSON preview; `Load existing file` replaces it
- [ ] **Configuration Builder** (TASK-088): opens with the default ESP32 wiring
      pre-populated; `Load existing file` replaces it
- [ ] The "example" notice disappears once a user-supplied file is loaded
- [ ] All example data is inline JS — no `fetch()` calls, works offline
- [ ] No regressions to load-file and download flows

## Test Plan

1. Open each tool offline (disconnect network).
2. Verify each tool shows meaningful data immediately without any user action.
3. Load a custom file → verify the example-notice disappears and the file data replaces
   the example.
4. Reload the page → example is shown again (not the last loaded file — no persistence).

## Notes

Depends on TASK-086 (load-example fix) for shared approach patterns.
Depends on TASK-088 for the configuration builder default.

The profile builder's example profile should demonstrate at least one of each common action
type (SendKeyAction, SendMediaKeyAction, SerialOutputAction) so it doubles as a reference.
