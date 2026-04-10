---
name: tasks
description: Show all open tasks from docs/developers/tasks/open/
---

# tasks

Read every `.md` file in `docs/developers/tasks/open/`, parse the frontmatter,
and print a compact table:

```
ID        Effort              Complexity  Title
TASK-011  Small (<2h)         Junior      Add FUNDING.yml
TASK-047  Small (<2h)         Junior      Fix profile LED blinking behavior
...
```

Sort by ID (ascending). After the table, print the total count of open tasks.
Do not suggest actions or next steps — just display the list.
