---
name: task-new
description: Scaffold a new task file in docs/developers/tasks/open/ and update OVERVIEW.md
---

# task-new

The user invokes this as `/task-new "Short task title"` (optionally with
`--effort S|M|L|XL`, `--complexity Junior|Medium|Senior`,
`--group <GroupName>`, and `--order <N>`).

Steps:

1. Determine the next task ID by reading `docs/developers/tasks/OVERVIEW.md` — the
   `<!-- GENERATED -->` block lists every open and closed task ID. Find the highest
   TASK-NNN number there and add 1. Do **not** scan individual task files for this;
   OVERVIEW.md is the authoritative index and is always up to date.
2. Build the filename slug from the title: lowercase, spaces/special chars → hyphens,
   max 50 chars, prefixed with the new ID (e.g. `task-059-my-new-task.md`).
3. Generate detailed suggestions for the task:
   - Expand the description with context and purpose
   - Propose 2-3 specific acceptance criteria
   - Determine the test plan (see below)
   - Add relevant notes about dependencies, risks, or considerations
4. Present the suggestions to the user for review and correction
5. Write the file to `docs/developers/tasks/open/` with this enhanced template:

```markdown
---
id: TASK-NNN
title: <title>
status: open
opened: <YYYY-MM-DD>
effort: <effort label>
complexity: <complexity>
human-in-loop: <hil value>
group: <GroupName>    # omit entirely if no group
order: <N>            # omit entirely if no group; integer execution order within the group
---

## Description

<expanded description with context and purpose>.

## Acceptance Criteria

- [ ] <specific criterion 1>
- [ ] <specific criterion 2>
- [ ] <specific criterion 3>

## Test Plan

<See rules below — fill in the appropriate section(s)>

## Notes

<dependencies, risks, considerations, and other context>
```

   Set `opened` to today's date in `YYYY-MM-DD` format.
   Effort labels (`--effort`): S → `Small (<2h)`, M → `Medium (2-8h)`, L → `Large (8-24h)`, XL → `Extra Large (24-40h)`.
   Complexity values (`--complexity`): `Junior`, `Medium`, `Senior`.
   `human-in-loop` values: `No`, `Clarification`, `Support`, `Main`.
   `group`: free-form group name (e.g. `Finalization`). Omit the field entirely if the task
   does not belong to a group. When provided, also set `order` to the integer execution
   sequence within the group (1 = first). Tasks in the same group are displayed together in
   OVERVIEW.md and `/tasks` output, sorted by `order`.
   If the user does not supply required values, infer them from context or ask. Do not leave
   these as `?` — they must be set so the overview table is meaningful.

<!-- markdownlint-disable MD029 -->
6. Run `python scripts/update_task_overview.py` to regenerate `OVERVIEW.md`.
7. Report the new task ID and file path.
<!-- markdownlint-enable MD029 -->

Do not commit.

---

## Test Plan rules

Every task **must** include a Test Plan section. Use the decision table below to fill it in.

### Decision table

| The task changes… | Test layer |
|---|---|
| Pure logic in `lib/PedalLogic/` with no Arduino/GPIO calls | Host tests only (`test/unit/`) |
| GPIO, ISRs, interrupts, pin state, hardware timing | On-device tests (`test/test_*_esp32/` or `test/test_*_nrf52840/`) |
| Both layers | Both |
| Only docs, config, scripts, or CI — no C++ logic | "No automated tests required — change is non-functional" |

### Host-only example

```markdown
## Test Plan

**Host tests** (`make test-host`):
- Add `test/unit/test_<feature>.cpp`
- Register in `test/CMakeLists.txt`
- Cover: <list key scenarios>
```

### On-device example

```markdown
## Test Plan

**On-device tests** (`make test-esp32-button`):
- Extend `test/test_buttons_esp32/test_main.cpp`
- Cover: <list key scenarios>
- Requires: ESP32 connected via USB
```

### Both layers example

```markdown
## Test Plan

**Host tests** (`make test-host`):
- Add `test/unit/test_<feature>.cpp`
- Cover: <logic scenarios>

**On-device tests** (`make test-esp32-button`):
- Extend `test/test_buttons_esp32/test_main.cpp`
- Cover: <hardware scenarios>
- Requires: ESP32 connected via USB
```
