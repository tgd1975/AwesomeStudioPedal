---
id: TASK-067
title: Add update-scripts-readme skill
status: closed
opened: 2026-04-11
closed: 2026-04-11
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

Create a new skill that automatically updates the scripts/README.md documentation whenever scripts are added, changed, or removed. This skill should use the new `update_scripts_readme.py` script.

## Acceptance Criteria

- [ ] Create `.claude/skills/update-scripts-readme/SKILL.md` with instructions for the agent
- [ ] The skill should invoke `python3 scripts/update_scripts_readme.py`
- [ ] The skill should report what scripts were added/changed/removed
- [ ] Add the new skill to the enabled skills in `.vibe/config.toml`
- [ ] Test the skill by running `/update-scripts-readme`

## Notes

The skill should follow the same pattern as other existing skills like `format` and `test`.
