# Project: AwesomeStudioPedal

## OS context

This project is developed on both **Windows 11** and **Ubuntu**. At the start of every session, check the platform from the system environment info and apply the correct shell syntax:

| Situation | Rule |
|---|---|
| Windows 11 (Git Bash / MSYS2) | Use Unix shell syntax (bash). Absolute paths use forward slashes: `C:/Users/...`. No `cmd.exe` commands (`dir`, `type`, `copy`, `del`). |
| Ubuntu / Linux | Standard bash. `/dev/null`, `apt`, etc. all work as expected. |

## Skill registration

When adding a new skill (creating `.claude/skills/<name>/SKILL.md`), always also add `<name>` to `enabled_skills` in [.vibe/config.toml](.vibe/config.toml). Failing to do so means the skill exists on disk but is not registered and may not be loaded.
