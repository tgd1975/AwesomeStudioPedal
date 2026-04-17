---
name: release-branch
description: Squash a feature/refactoring branch, merge it into main, update CHANGELOG, and cut a release
---

# release-branch

Squash-merge the current branch into `main` and cut a release in one guided flow.

Invoke as `/release-branch vX.Y.Z`. If no version is given, read the current version from
`platformio.ini` and ask what the new version should be.

## Steps

### Phase 1 — Prepare the branch

1. **Verify state**: run `git status`. Stop if uncommitted changes exist. Run
   `git branch --show-current`. Stop if already on `main`.

2. **Show branch summary**: print the branch name and every commit ahead of `main`:

   ```bash
   git log main..HEAD --oneline
   ```

3. **Confirm version**: read current version from `platformio.ini`. Show it and ask the
   user to confirm the target release version if not supplied as an argument.

### Phase 2 — Update CHANGELOG

4. **Populate `[Unreleased]`**: read `CHANGELOG.md`. Find the `## [Unreleased]` section.

   - If the section is **empty**, generate candidate entries from the commit log:

     ```bash
     git log main..HEAD --pretty=format:"- %s"
     ```

     Group entries under appropriate sub-headings (`### Added`, `### Changed`,
     `### Fixed`) based on conventional-commit prefixes (`feat`/`fix`/`refactor`/`chore`).
     Show the draft to the user and ask for confirmation or edits before writing.

   - If the section already has content, show it and ask the user to confirm it is
     complete before proceeding.

5. **Write CHANGELOG**: replace the `## [Unreleased]` heading with:

   ```
   ## [Unreleased]

   ---

   ## [vX.Y.Z] — YYYY-MM-DD
   ```

   Keep the existing `[Unreleased]` content under the new versioned heading.
   Use today's date (`currentDate` from context, or `date +%Y-%m-%d`).

### Phase 3 — Squash and push

6. **Squash commits**: propose a squash commit message based on the branch name and
   version (e.g. `"release: squash feature/improvements for vX.Y.Z"`). Confirm with the
   user, then:

   ```bash
   BASE=$(git merge-base HEAD main)
   git reset --soft $BASE
   git add CHANGELOG.md
   git commit -m "<confirmed message>"
   ```

7. **Push branch**:

   ```bash
   git push --force-with-lease origin <branch>
   ```

8. **Open PR**: create a pull request targeting `main`. Use the CHANGELOG versioned
   section as the PR body:

   ```bash
   gh pr create --title "Release vX.Y.Z" --body "$(cat <<'EOF'
   ## Summary
   <CHANGELOG [vX.Y.Z] content here>

   🤖 Generated with [Claude Code](https://claude.com/claude-code)
   EOF
   )"
   ```

   Print the PR URL.

9. **Prompt user to merge**: tell the user to merge the PR in the GitHub UI (review,
   approve, squash-merge or merge commit — their choice), then confirm back here.
   Wait for explicit confirmation before continuing.

### Phase 4 — Release on main

10. **Switch to main and pull**:

    ```bash
    git checkout main
    git pull
    ```

11. **Bump version**: update **both** files atomically:
    - `version =` line in `platformio.ini`
    - `#define FIRMWARE_VERSION` line in `include/version.h`

    Both must show the same `vX.Y.Z` string.

12. **Archive closed tasks**: move every flat `.md` file in `docs/developers/tasks/closed/`
    into `docs/developers/tasks/closed/vX.Y.Z/` using `git mv`. Skip existing version
    subdirectories.

    ```bash
    mkdir -p docs/developers/tasks/closed/vX.Y.Z
    for f in docs/developers/tasks/closed/*.md; do
      git mv "$f" docs/developers/tasks/closed/vX.Y.Z/
    done
    ```

13. **Regenerate task overview**:

    ```bash
    python scripts/update_task_overview.py
    git add docs/developers/tasks/
    ```

14. **Commit the bump and archive**:

    ```bash
    git add platformio.ini include/version.h
    git commit -m "chore: bump version to vX.Y.Z and archive closed tasks"
    ```

15. **Create annotated tag**:

    ```bash
    git tag -a vX.Y.Z -m "Release vX.Y.Z"
    ```

16. **Show a final summary** of what will be pushed (commit + tag), then ask the user to
    confirm before pushing.

17. **Push commit and tag**:

    ```bash
    git push origin main
    git push origin vX.Y.Z
    ```

18. **Remind the user** to create a GitHub Release from the tag in the GitHub UI and
    attach the `.elf`/`.bin` build artefacts for each target environment.

Do not push at any step without explicit user confirmation.
