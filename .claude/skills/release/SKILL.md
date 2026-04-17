---
name: release
description: Bump version in platformio.ini, create a git tag, and push it
---

# release

Guide a release following the process in `docs/developers/CI_PIPELINE.md`.

The user invokes this as `/release vX.Y.Z` (e.g. `/release v1.3.0`).
If no version is given, read the current version from `platformio.ini` and ask what the
new version should be.

Steps:

1. **Verify clean state**: run `git status`. If there are uncommitted changes, warn the
   user and stop — a release must be made from a clean working tree on `main`.

2. **Verify branch**: run `git branch --show-current`. If not on `main`, warn and stop.

3. **Read current version**: find the `version = X.Y.Z` line in `platformio.ini` and
   show it.

4. **Confirm the new version** with the user before making any changes.

5. **Bump version**: update **both** files atomically:
   - `version =` line in `platformio.ini`
   - `#define FIRMWARE_VERSION` line in `include/version.h`

   Both must show the same `vX.Y.Z` string.

6. **Archive closed tasks**: move every file currently in `docs/developers/tasks/closed/`
   (not in a subdirectory) into a new `docs/developers/tasks/closed/vX.Y.Z/` folder using
   `git mv`. Skip any existing version subdirectories — only move flat `.md` files.

   ```bash
   mkdir -p docs/developers/tasks/closed/vX.Y.Z
   for f in docs/developers/tasks/closed/*.md; do
     git mv "$f" docs/developers/tasks/closed/vX.Y.Z/
   done
   ```

7. **Regenerate task overview** (generates OVERVIEW.md and `closed/vX.Y.Z/OVERVIEW.md`):

   ```bash
   python scripts/update_task_overview.py
   git add docs/developers/tasks/
   ```

8. **Update CHANGELOG**: read `CHANGELOG.md`. Find the `## [Unreleased]` section.

   - If the section is **empty**, note it to the user and still update the heading —
     the section will be empty in the versioned entry.
   - Replace `## [Unreleased]` with:

     ```
     ## [Unreleased]

     ---

     ## [vX.Y.Z] — YYYY-MM-DD
     ```

   Keep all existing `[Unreleased]` content under the new versioned heading.
   Use today's date (`date +%Y-%m-%d`). Stage the file:

   ```bash
   git add CHANGELOG.md
   ```

9. **Commit the bump, archive, and changelog**:

   ```bash
   git add platformio.ini include/version.h
   git commit -m "chore: bump version to vX.Y.Z, archive closed tasks, update CHANGELOG"
   ```

10. **Create annotated tag**:

    ```bash
    git tag -a vX.Y.Z -m "Release vX.Y.Z"
    ```

11. **Show a final summary** of what will be pushed (commit + tag), then ask the user to
    confirm before pushing.

12. **Push commit and tag**:

    ```bash
    git push origin main
    git push origin vX.Y.Z
    ```

13. **Remind the user** to create a GitHub Release from the tag in the GitHub UI and
    attach the `.elf`/`.bin` build artefacts for each target environment.

Do not push without explicit user confirmation in step 10.
