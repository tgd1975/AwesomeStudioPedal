---
name: release
description: Bump version, archive tasks, update CHANGELOG, tag, push, build firmware, and publish GitHub Release with artifacts
---

# release

Guide a release following the process in `docs/developers/CI_PIPELINE.md`.

The user invokes this as `/release vX.Y.Z` (e.g. `/release v1.3.0`).
If no version is given, read the current version from `include/version.h` and ask what the
new version should be.

Steps:

1. **Verify clean state**: run `git status`. If there are uncommitted changes, warn the
   user and stop — a release must be made from a clean working tree on `main`.

2. **Verify branch**: run `git branch --show-current`. If not on `main`, warn and stop.

3. **Read current version**: find the `#define FIRMWARE_VERSION` line in `include/version.h`
   and show it.

4. **Confirm the new version** with the user before making any changes.

5. **Bump version**: update `include/version.h`:

   ```c
   #define FIRMWARE_VERSION "vX.Y.Z"
   ```

6. **Archive closed tasks**: move every flat `.md` file in `docs/developers/tasks/closed/`
   into `docs/developers/tasks/archive/vX.Y.Z/` using `git mv`. Tasks in `open/`,
   `active/`, and `paused/` are **not** archived — they carry forward into the next
   release. Paused inherits open/active behavior here: a paused task does not block
   the release, does not appear shipped, and stays in `paused/` until it is resumed
   or closed in a future release.

   ```bash
   mkdir -p docs/developers/tasks/archive/vX.Y.Z
   for f in docs/developers/tasks/closed/*.md; do
     git mv "$f" docs/developers/tasks/archive/vX.Y.Z/
   done
   ```

7. **Regenerate task overview** (regenerates OVERVIEW.md, EPICS.md, KANBAN.md, and `archive/vX.Y.Z/OVERVIEW.md`):

   ```bash
   python scripts/housekeep.py --apply
   git add docs/developers/tasks/
   ```

7a. **Snapshot OVERVIEW / EPICS / KANBAN into archive/vX.Y.Z/.** Freezes the
    post-archive state of the three top-level overviews into the per-release
    folder, renamed with the version as a suffix, with auto-generation
    markers and notices stripped so they read as static historical
    artifacts (housekeep will not touch them on later runs):

    ```bash
    python scripts/release_snapshot.py vX.Y.Z
    git add docs/developers/tasks/archive/vX.Y.Z/
    ```

    Result: `archive/vX.Y.Z/{OVERVIEW,EPICS,KANBAN}_vX.Y.Z.md` next to
    the existing `archive/vX.Y.Z/OVERVIEW.md` (the per-release closed-tasks
    listing — unchanged).

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

1. **Update README firmware section**: replace the block between
   `<!-- RELEASE_SECTION_START -->` and `<!-- RELEASE_SECTION_END -->` in `README.md`
   with the real download links. Read the existing block first to detect whether
   previous releases are already listed, then write the updated block:

   ```
   <!-- RELEASE_SECTION_START -->
   **Current stable: vX.Y.Z**

   - ESP32: [awesome-pedal-esp32-vX.Y.Z.bin](../../releases/download/vX.Y.Z/awesome-pedal-esp32-vX.Y.Z.bin)
   - nRF52840: [awesome-pedal-nrf52840-vX.Y.Z.bin](../../releases/download/vX.Y.Z/awesome-pedal-nrf52840-vX.Y.Z.bin)

   **Previous releases** (current + 2 kept):

   <keep up to 2 previous release entries here, remove older ones>
   <!-- RELEASE_SECTION_END -->
   ```

   Stage the file:

   ```bash
   git add README.md
   ```

2. **Commit the bump, archive, changelog, and README**.

   This is a multi-file release operation that does not fit the
   `/commit` pathspec model (the archive directory contents are not
   knowable as a flat file list). Use the
   `ASP_COMMIT_BYPASS=<reason>` mechanism documented in
   [docs/developers/COMMIT_POLICY.md](../../../docs/developers/COMMIT_POLICY.md);
   the bypass is logged to `.git/asp-commit-bypass.log` for review:

   ```bash
   git add include/version.h
   ASP_COMMIT_BYPASS="release: version bump + archive + CHANGELOG" \
       git commit --no-verify -m "chore: bump version to vX.Y.Z, archive closed tasks, update CHANGELOG"
   ```

   `--no-verify` is kept here because `/release` runs the full release
   build separately; re-running the pre-commit chain (tests + clang-tidy
   + mermaid) would duplicate that work.

1. **Create annotated tag**:

    ```bash
    git tag -a vX.Y.Z -m "Release vX.Y.Z"
    ```

2. **Show a final summary** of what will be pushed (commit + tag), then ask the user to
    confirm before pushing.

3. **Push commit and tag**:

    ```bash
    git push origin main
    git push origin vX.Y.Z
    ```

4. **Build firmware for all targets**:

    ```bash
    pio run -e nodemcu-32s
    pio run -e feather-nrf52840
    ```

    Copy and compress artifacts with versioned names. The ESP32 `.elf` is ~23 MB
    raw but compresses to ~8 MB, so zip it:

    ```bash
    cp .pio/build/nodemcu-32s/firmware.bin      firmware-nodemcu-32s-vX.Y.Z.bin
    zip firmware-nodemcu-32s-vX.Y.Z-debug.zip   .pio/build/nodemcu-32s/firmware.elf
    cp .pio/build/feather-nrf52840/firmware.hex  firmware-feather-nrf52840-vX.Y.Z.hex
    cp .pio/build/feather-nrf52840/firmware.zip  firmware-feather-nrf52840-vX.Y.Z.zip
    ```

5. **Create GitHub Release** with `gh`, using the `## [vX.Y.Z]` section from CHANGELOG
    as release notes, and attach all four firmware artifacts:

    ```bash
    gh release create vX.Y.Z \
      firmware-nodemcu-32s-vX.Y.Z.bin \
      firmware-nodemcu-32s-vX.Y.Z-debug.zip \
      firmware-feather-nrf52840-vX.Y.Z.hex \
      firmware-feather-nrf52840-vX.Y.Z.zip \
      --title "vX.Y.Z" \
      --notes "<changelog content for vX.Y.Z>"
    ```

    The release notes body must include a **Firmware Artifacts** table:

    | File | Target |
    |------|--------|
    | `firmware-nodemcu-32s-vX.Y.Z.bin` | NodeMCU-32S (ESP32) — flash binary |
    | `firmware-nodemcu-32s-vX.Y.Z-debug.zip` | NodeMCU-32S (ESP32) — debug symbols (zipped ~8 MB) |
    | `firmware-feather-nrf52840-vX.Y.Z.hex` | Adafruit Feather nRF52840 — flash hex |
    | `firmware-feather-nrf52840-vX.Y.Z.zip` | Adafruit Feather nRF52840 — OTA zip |

6. **Clean up** the temporary artifact copies:

    ```bash
    rm firmware-nodemcu-32s-vX.Y.Z.bin firmware-nodemcu-32s-vX.Y.Z-debug.zip \
       firmware-feather-nrf52840-vX.Y.Z.hex firmware-feather-nrf52840-vX.Y.Z.zip
    ```

Do not push without explicit user confirmation in step 11.
