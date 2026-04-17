# Scripts Documentation

This folder contains utility scripts for development, CI/CD, and maintenance tasks.

## Available Scripts

| Script | Purpose | Usage |
|--------|---------|-------|
| [`check_code_smells.py`](check_code_smells.py) | Code smell detector for AwesomeStudioPedal C++ sources. | `python3 check_code_smells.py` |
| [`cleanup-releases.sh`](cleanup-releases.sh) | cleanup-releases.sh — keep only the current + 2 previous GitHub releases. | `./cleanup-releases.sh` |
| [`format-code.sh`](format-code.sh) | Format all C++ files with clang-format | `./format-code.sh` |
| [`migrate_tasks.py`](migrate_tasks.py) | Migrate tasks from REPOSITORY_IMPROVEMENT_CONCEPT.md into | `python3 migrate_tasks.py` |
| [`pre-commit`](pre-commit) | Comprehensive pre-commit hook for code quality checks | `python3 pre-commit` |
| [`serial_monitor.py`](serial_monitor.py) | Simple serial monitor for ESP32/Arduino devices | `python3 serial_monitor.py` |
| [`sync_skills_config.py`](sync_skills_config.py) | Sync enabled_skills in .vibe/config.toml with .claude/skills/ directory. | `python3 sync_skills_config.py` |
| [`update_future_ideas.py`](update_future_ideas.py) | Regenerate the Future Ideas section in README.md from idea files | `python3 update_future_ideas.py` |
| [`update_scripts_readme.py`](update_scripts_readme.py) | Automatically update scripts/README.md based on current scripts in the folder. | `python3 update_scripts_readme.py` |
| [`update_task_overview.py`](update_task_overview.py) | Regenerate docs/developers/tasks/OVERVIEW.md from the task files | `python3 update_task_overview.py` |
| [`validate_mermaid.py`](validate_mermaid.py) | Mermaid Diagram Validation Script | `python3 validate_mermaid.py` |

## Script Details

### check_code_smells.py

**Purpose**: Code smell detector for AwesomeStudioPedal C++ sources.

**Usage**: `python3 check_code_smells.py`

### cleanup-releases.sh

**Purpose**: cleanup-releases.sh — keep only the current + 2 previous GitHub releases.

**Usage**: `./cleanup-releases.sh`

### format-code.sh

**Purpose**: Format all C++ files with clang-format

**Usage**: `./format-code.sh`

### migrate_tasks.py

**Purpose**: Migrate tasks from REPOSITORY_IMPROVEMENT_CONCEPT.md into

**Usage**: `python3 migrate_tasks.py`

### pre-commit

**Purpose**: Comprehensive pre-commit hook for code quality checks

**Usage**: `python3 pre-commit`

### serial_monitor.py

**Purpose**: Simple serial monitor for ESP32/Arduino devices

**Usage**: `python3 serial_monitor.py`

### sync_skills_config.py

**Purpose**: Sync enabled_skills in .vibe/config.toml with .claude/skills/ directory.

**Usage**: `python3 sync_skills_config.py`

### update_future_ideas.py

**Purpose**: Regenerate the Future Ideas section in README.md from idea files

**Usage**: `python3 update_future_ideas.py`

### update_scripts_readme.py

**Purpose**: Automatically update scripts/README.md based on current scripts in the folder.

**Usage**: `python3 update_scripts_readme.py`

### update_task_overview.py

**Purpose**: Regenerate docs/developers/tasks/OVERVIEW.md from the task files

**Usage**: `python3 update_task_overview.py`

### validate_mermaid.py

**Purpose**: Mermaid Diagram Validation Script

**Usage**: `python3 validate_mermaid.py`
