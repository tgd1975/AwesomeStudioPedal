# Task System — User Guide

A lightweight docs-as-code task and idea management system for small teams.
Tasks and ideas are Markdown files with YAML frontmatter; folder = status.
Claude Code skills provide the CLI. `housekeep.py` keeps everything in sync.

---

## Prerequisites

- Python 3.10+
- Claude Code CLI with `.vibe/config.toml` for skill registration

---

## Installation

1. **Copy the distribution artifacts** from `awesome-task-system/` into your repo:

   ```
   cp -r awesome-task-system/scripts/* scripts/
   cp -r awesome-task-system/skills/* .claude/skills/
   cp awesome-task-system/config/task-system.yaml docs/developers/
   ```

2. **Edit `docs/developers/task-system.yaml`** — set your `base_folder` paths:

   ```yaml
   tasks:
     base_folder: docs/developers/tasks   # or wherever you want tasks
   ideas:
     base_folder: docs/developers/ideas
   ```

3. **Register skills** in `.vibe/config.toml`:

   ```toml
   enabled_skills = ["ts-epic-list", "ts-epic-new", "ts-idea-archive",
                     "ts-idea-list", "ts-idea-new", "ts-task-active",
                     "ts-task-pause", "ts-task-reopen"]
   ```

4. **Create the folder structure**:

   ```bash
   python scripts/housekeep.py --init
   ```

5. **Generate overview files** (run after any file changes):

   ```bash
   python scripts/housekeep.py --apply
   ```

---

## Config reference (`task-system.yaml`)

| Key | Default | Effect |
|-----|---------|--------|
| `tasks.base_folder` | `docs/developers/tasks` | Root folder for task files |
| `tasks.active.enabled` | `true` | Create and scan `active/` subfolder |
| `tasks.epics.enabled` | `true` | Process epic files; generate `EPICS.md` |
| `tasks.releases.enabled` | `true` | Create `archive/` subfolder for releases |
| `ideas.enabled` | `true` | Create and scan ideas folder |
| `ideas.base_folder` | `docs/developers/ideas` | Root folder for idea files |
| `visualizations.epics.enabled` | `true` | Generate `EPICS.md` |
| `visualizations.epics.style` | `dependency-graph` | `dependency-graph` or `gantt` |
| `visualizations.kanban.enabled` | `true` | Generate `KANBAN.md` |

Override any key by setting `TASK_SYSTEM_CONFIG=/path/to/your-config.yaml`.

---

## Skill reference

| Skill | Usage | Effect |
|-------|-------|--------|
| `/ts-task-active TASK-NNN` | Mark task as active (or resume a paused one) | Moves file to `active/` (or clears resolved prerequisites for a resume), runs housekeep |
| `/ts-task-pause TASK-NNN` | Pause an active task that is blocked | Records the blocker in `prerequisites:` and a `## Paused` body note; **file stays in `active/`**, status stays `active` |
| `/ts-task-reopen TASK-NNN` | Reopen closed task | Moves file to `open/`, runs housekeep |
| `/ts-epic-new <name> "Title"` | Create epic | Scaffolds `EPIC-NNN` file in `open/` |
| `/ts-epic-list` | List epics | Shows all epics with status and task counts |
| `/ts-idea-new <id> "Title"` | Create idea | Scaffolds `IDEA-NNN` file in `ideas/open/` |
| `/ts-idea-list` | List ideas | Shows open ideas from `OVERVIEW.md` |
| `/ts-idea-archive IDEA-NNN` | Archive idea | Moves idea to `archived/`, commits |

---

## Task lifecycle

```
open/ ──[activate]──▶ active/ ──[close]──▶ closed/
                         ▲ │
                         │ └──[pause: stay active, add prerequisite]
                         └──[resume: clear closed prerequisites]

closed/ ──[release]──▶ archive/vX.Y.Z/
```

There are only three statuses (`open`, `active`, `closed`) and one of
them — `active` — does double duty for "in progress" and "paused
because blocked". The pause is recorded in the task body, not the
status. See [Pausing a task](#pausing-a-task) below.

### When `active` is mandatory vs. optional

- **One-shot tasks** that are completed in a single sitting **may**
  bypass `active/` entirely — go straight from `open/` to `closed/`
  via `/ts-task-done`. Forcing every trivial task through `active`
  would add ceremony with no value.
- **Tasks that are started and then paused** (e.g. because executing
  them surfaced a defect) **must** be `active` first, because pausing
  preserves the "we already started this" signal — see below.

### Pausing a task

When work on a task is interrupted by a blocker (a defect that must be
fixed first, an external dependency, etc.):

1. The task **stays in `active/`** with `status: active`. We do not
   move it back to `open/` — that would erase the fact that work
   has already begun.
2. The blocker is recorded in the `prerequisites:` frontmatter field.
   For a defect, this is typically the new defect task's ID
   (e.g. `prerequisites: [TASK-250]`).
3. A short `## Paused` section is added to the task body explaining
   what we are waiting for and the date.

Use `/ts-task-pause TASK-NNN` to do this — it prompts for the
blocking task ID(s) and the reason, then updates the file in place.

When the blocker closes, run `/ts-task-active TASK-NNN` again — the
skill detects the resume case (file already in `active/`, prerequisites
now closed) and clears the stale prerequisite entries.

Real-world examples: TASK-153 and TASK-155 each surfaced app defects
mid-execution; they should remain `active` while waiting on the
matching defect tasks (e.g. TASK-250) to close.

**Task frontmatter fields:**

| Field | Required | Description |
|-------|----------|-------------|
| `id` | yes | `TASK-NNN` (auto-incremented) |
| `title` | yes | Short human-readable title |
| `status` | yes | `open` / `active` / `closed` |
| `opened` | yes | `YYYY-MM-DD` |
| `effort` | no | `Trivial` / `Small (<2h)` / `Medium (2-8h)` / `Large` |
| `complexity` | no | `Junior` / `Medium` / `Senior` |
| `epic` | no | Epic name this task belongs to |
| `order` | no | Sort order within the epic |
| `prerequisites` | no | `[TASK-NNN, TASK-MMM]` — feeds the epic dependency graph; also used to record blockers when a task is paused |
| `assigned` | no | Username of the owner |
| `human-in-loop` | no | `Main` / `Support` / `Clarification` / `None` |

---

## Idea lifecycle

```
ideas/open/ ──[convert to tasks]──▶ (archived after conversion)
ideas/open/ ──[deprioritise]──▶ ideas/archived/
```

**Idea frontmatter fields:** `id`, `title`, `description` (optional).

---

## Epic lifecycle

Epics are derived automatically: `housekeep.py` sets their status based on their tasks.

- Any task `active` → epic is `active`
- All tasks `closed` → epic is `closed`
- Otherwise → epic is `open`

---

## Generated files

`housekeep.py --apply` regenerates three files automatically:

| File | Contents |
|------|----------|
| [OVERVIEW.md](tasks/OVERVIEW.md) | Counts, active/open/closed task tables |
| [EPICS.md](tasks/EPICS.md) | Index + one section per epic with dependency graph and task table |
| [KANBAN.md](tasks/KANBAN.md) | Mermaid kanban board with Open / Active / Closed columns |
| [Ideas OVERVIEW.md](ideas/OVERVIEW.md) | All open ideas with descriptions |

These files are always regenerated from source — do not edit them manually.

---

## Updating the task system

The task system is distributed as a directory snapshot. To update:

1. Pull the latest `awesome-task-system/` from the source repo.
2. Copy the updated scripts and skills over your existing ones.
3. Re-run `python scripts/housekeep.py --init` (safe — it is idempotent).
4. Re-run `python scripts/housekeep.py --apply` to regenerate views.
