---
name: nrf5-task-routing
description: Use BEFORE invoking /ts-task-new whenever the new task touches the nRF52840 hardware target — any change under src/nrf52840/, any new test under test/test_*_nrf52840/, any nRF52840 PlatformIO env or build target, or any AC requiring on-device verification on the Adafruit Feather nRF52840 (BLE via Bluefruit, GPIO, ISRs). Routes all such tasks to EPIC-025 (nrf52840-blocked) instead of the feature epic that would otherwise own them by scope. Active until the device is available again — removed by TASK-359 at that point.
---

# nrf5-task-routing

A temporary routing rule. While this skill exists in
`.claude/skills/nrf5-task-routing/`, **every newly-scaffolded task that
touches the nRF52840 firmware target must be assigned to
`epic: nrf52840-blocked`** rather than to the feature epic that would
otherwise own it.

## Why

The Adafruit Feather nRF52840 hardware is not currently available, so
any new nRF5-touching task cannot be on-device verified. Routing all
such work into a single hardware-blocked epic mirrors
[EPIC-019 iphone-app](../../../docs/developers/tasks/active/epic-019-iphone-app.md):
the iPhone hardware is similarly out of reach, and that epic holds all
iPhone-dependent work in one place rather than scattering it across
feature epics where it would block per-epic progress.

Without this routing, nRF5 tasks would land in their natural feature
epics (e.g. `app-content-pages`, `ble-config`) and clutter those epics'
queues with paused-on-hardware items. With it, the feature epics stay
clean and the hardware-blocked queue is one focused list.

## When this applies

A task "touches nRF52840 hardware" if **any** of the following is true:

- It modifies, adds, or deletes any file under `src/nrf52840/`.
- It adds or extends a test under `test/test_*_nrf52840/`.
- It adds or modifies an nRF52840 PlatformIO env or build target
  (e.g. `feather-nrf52840`, `make build-nrf52840`,
  `make test-nrf52840-*`).
- Its acceptance criteria require on-device verification on the
  Adafruit Feather nRF52840 (BLE via Bluefruit, GPIO, ISR timing,
  filesystem on InternalFS, `Nrf52840PedalApp`).
- Its description mentions "nRF52840", "nRF5", "Bluefruit", or
  "Feather nRF52840" in a way that implies hardware-side work
  (not merely "this is also planned for nRF52840 later").

Pure ESP32 tasks are unaffected. Tasks that touch only platform-
agnostic code in `lib/PedalLogic/` are unaffected. Tasks that
*reference* nRF52840 in passing (e.g. cross-platform docs) are
unaffected — only tasks where the work itself targets nRF52840.

## How to apply

1. **Decide whether the routing applies** using the criteria above. If
   it does not, skip — invoke `/ts-task-new` normally.

2. **Determine the next `order:` within EPIC-025.** Scan tasks with
   `epic: nrf52840-blocked` across `open/`, `active/`, `paused/`, and
   `closed/`. Take the maximum `order` value, add 1.

   ```bash
   grep -l "^epic: nrf52840-blocked$" docs/developers/tasks/*/*.md \
     | xargs grep -h "^order:" | awk '{print $2}' | sort -n | tail -1
   ```

3. **Invoke `/ts-task-new`** with `--epic nrf52840-blocked` and
   `--order <next>`. Pick `--effort` and `--complexity` based on the
   actual work as usual. Default `human-in-loop: Clarification`
   unless the task body says otherwise.

4. **Set `status: paused`** on the resulting file (the default from
   `/ts-task-new` is `open` — manually flip to `paused` and run
   `/housekeep` so the file lands in `paused/`). Hardware blockers do
   not use a task-system `prerequisites:` field; instead, add a
   `## Paused` section in the body explaining the blocker, following
   the pattern of [task-158](../../../docs/developers/tasks/paused/task-158-feature-test-ios-build-deploy.md).

   Example:

   ```markdown
   ## Paused

   - YYYY-MM-DD: Paused — no Adafruit Feather nRF52840 device is
     currently on hand. <one-line summary of which AC item requires
     hardware>. Resume with `/ts-task-active TASK-NNN` once a device
     is available.
   ```

5. **Record the original feature epic in the Notes section** so the
   re-homing performed by TASK-359 (when hardware returns) is
   mechanical:

   ```markdown
   ## Notes

   - Originally belongs to feature epic `<feature-epic>` by scope;
     routed to EPIC-025 nrf52840-blocked while nRF52840 hardware is
     unavailable. Once TASK-359 closes, this task can be re-homed to
     `<feature-epic>`.
   ```

   The `<feature-epic>` value is whatever epic the task would have
   belonged to without this routing — i.e. the natural feature epic
   for its scope. If the task does not have a natural feature epic
   (genuinely cross-cutting infra), say so in the Notes line and
   omit the re-homing step from the AC of the future cleanup.

## When this skill is removed

[TASK-359](../../../docs/developers/tasks/open/task-359-remove-nrf5-task-routing-skill.md)
removes this skill once hardware is back. At that point this directory
is deleted and `nrf5-task-routing` comes off the `enabled_skills` list
in [.vibe/config.toml](../../../.vibe/config.toml). Until then, the
skill is active and the routing rule above is mandatory for every
nRF5-touching task.
