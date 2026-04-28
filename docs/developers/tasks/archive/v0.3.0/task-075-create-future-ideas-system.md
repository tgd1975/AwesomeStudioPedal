---
id: TASK-075
title: Create Future Ideas System
date: 2024-07-22
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
status: closed
closed: 2026-04-14
---

## Description

Create a structured system for tracking future ideas that are not yet ready to become formal tasks. This system should provide a way to capture and organize potential future enhancements, features, or projects without the overhead of the formal task lifecycle.

The system should include:

1. A dedicated directory structure for future ideas
2. A simple naming convention (IDEA-NNN format)
3. Automatic updating of the README.md Future Ideas section
4. Similar automation pattern to the existing task overview system

## Acceptance Criteria

- [ ] Create directory structure: `docs/devs/tasks/future/`
- [ ] Define and document the IDEA-NNN naming convention
- [ ] Create a Python script (`scripts/update_future_ideas.py`) to automate README.md updates
- [ ] Implement script that scans `docs/devs/tasks/future/` and generates markdown list
- [ ] Add Future Ideas section to README.md that gets automatically updated
- [ ] Test the automation and verify it works correctly
- [ ] Create first example idea file to demonstrate the system

## Notes

- The future ideas system should be simpler than the task system - no formal lifecycle needed
- Follow similar patterns to the existing task overview automation for consistency
- Ensure the README.md section is clearly marked and easy to find
- Consider adding a brief explanation of what "future ideas" are vs "tasks"
- The system should be maintainable and easy for contributors to use
