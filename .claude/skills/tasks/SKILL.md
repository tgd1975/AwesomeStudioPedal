---
name: tasks
description: Show all open tasks from docs/developers/tasks/open/
---

# tasks

> **Source of truth: `docs/developers/tasks/OVERVIEW.md`**
> Do NOT scan individual task files to discover groups, IDs, or effort values.
> The `<!-- GENERATED -->` … `<!-- END GENERATED -->` block in `OVERVIEW.md` is
> auto-maintained by `scripts/update_task_overview.py` and is always up to date.
> Read it directly — it is faster and avoids re-deriving what the script already computed.

Read `docs/developers/tasks/OVERVIEW.md` (the `<!-- GENERATED -->` block) and
reformat its content as a structured list.

**Ungrouped tasks** (no `group` field) — one compact table, sorted by ID ascending:

```
ID        Effort              Complexity  Title
TASK-033  Large (8-24h)       Medium      Create Setup/Installation Demo Video
TASK-049  Small (<2h)         Junior      Setup video platform channel
...
```

**Grouped tasks** — rendered under `### Task Groups` → `#### <GroupName>` sub-sections,
sorted alphabetically by group name, tasks within each group sorted by `order` (ascending).
Show a richer table with Order, Human-in-loop, and mark `Main` with ★:

```
#### Finalization

Order  ID        Effort        Complexity  Human-in-loop  Title
1      TASK-085  Large (8-24h) Senior      Support        Project management review
2      TASK-079  Large (8-24h) Senior      Support        Senior code review
...
7      TASK-083  Medium (2-8h) Senior      Main ★         On-device integration tests
```

After all sections, print the total count of open tasks (ungrouped + all groups).
Do not suggest actions or next steps — just display the list.
