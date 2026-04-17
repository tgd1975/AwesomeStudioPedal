# Task Overview

- [Open Tasks](#open-tasks)
- [Task Groups](#task-groups)
- [Closed Tasks](#closed-tasks)
- [Task Lifecycle](#task-lifecycle)
- [Column Reference](#column-reference)
- [Writing a Task File](#writing-a-task-file)

<!-- GENERATED -->

**Open: 7** | **Closed: 0** | **Total: 7**

## Open Tasks

### All Open Tasks

| ID | Title | Effort | Complexity |
|----|-------|--------|------------|
| [TASK-033](open/task-033-create-setup-installation-demo-video.md) | Create Setup/Installation Demo Video | Large (8-24h) | Medium |
| [TASK-034](open/task-034-create-button-configuration-demo-video.md) | Create Button Configuration Demo Video | Large (8-24h) | Medium |
| [TASK-035](open/task-035-create-builder-workflow-demo-video.md) | Create Builder Workflow Demo Video | Large (8-24h) | Medium |
| [TASK-036](open/task-036-create-advanced-features-demo-video.md) | Create Advanced Features Demo Video | Extra Large (24-40h) | Senior |
| [TASK-037](open/task-037-create-real-world-usage-demo-video.md) | Create Real-World Usage Demo Video | Extra Large (24-40h) | Senior |
| [TASK-038](open/task-038-create-troubleshooting-demo-video.md) | Create Troubleshooting Demo Video | Large (8-24h) | Medium |
| [TASK-049](open/task-049-setup-video-platform-channel.md) | Setup video platform channel | Small (<2h) | Junior |

## Task Groups

Groups with at least one open task are shown below. Each group lists all its tasks — open and closed. Closed tasks are ~~struck through~~.
_If no sub-sections appear here, there are currently no open tasks assigned to a group._

## Closed Tasks

| ID | Title | Effort |
|----|-------|--------|

## Archived Releases

- [v0.2.0](closed/v0.2.0/OVERVIEW.md)
- [v0.3.0](closed/v0.3.0/OVERVIEW.md)
<!-- END GENERATED -->

## Task Lifecycle

```
open/ → (pick up) → in progress → /task-done TASK-NNN → closed/
```

Create a task with `/task-new "Title"`. Pick it up by branching off `main`
(`feature/TASK-NNN-short-title`). When done, run `/task-done TASK-NNN` to move
the file to `closed/` and update this overview.

## Column Reference

| Column | Meaning |
|--------|---------|
| **ID** | Unique identifier — link to the task file |
| **Title** | One-line description of the work |
| **Effort** | Expected size: Small (<2h), Medium (2-8h), Large (8-24h), Extra Large (24-40h) |
| **Complexity** | Skill level needed: Junior, Medium, Senior |
| **Human-in-loop** | How much human involvement is required (see values below) |
| **Order** | Execution sequence within a group (grouped tasks only) |

## Writing a Task File

Each task file has a YAML frontmatter block followed by three sections.
The `id`, `title`, `effort`, `complexity`, and optional `group`/`order` fields are read
directly from frontmatter to generate the overview table — set them correctly when creating
a task.

```markdown
---
id: TASK-NNN
title: Short imperative title
status: open
opened: YYYY-MM-DD
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: GroupName   # optional — omit if task does not belong to a group
order: 1           # optional — integer execution order within the group; omit if no group
---

## Description

What needs to be done and why.

## Acceptance Criteria

- [ ] Specific, verifiable outcome

## Notes

Progress, decisions, links.
```

**`opened`** / **`closed`** — ISO date (`YYYY-MM-DD`). Set `opened` when creating the task.
Add `closed` when moving to `closed/` via `/task-done`.

**`effort`** values: `Small (<2h)`, `Medium (2-8h)`, `Large (8-24h)`, `Extra Large (24-40h)`

**`complexity`** values: `Junior`, `Medium`, `Senior`

**`human-in-loop`** values:

- `No` — fully automatable, no human input needed
- `Clarification` — needs a one-time decision before starting
- `Support` — human provides context or review during the work
- `Main` — human does the bulk of the work (e.g. video recording, on-device testing)

**`group`** / **`order`** — optional. Assign tasks to a named group (e.g. `Finalization`) and
set an integer `order` to define the recommended execution sequence within that group. Grouped
tasks appear in their own section in the overview table, sorted by `order`. Tasks with
`human-in-loop: Main` are shown with a ★ marker and should generally be placed last in the
group.
