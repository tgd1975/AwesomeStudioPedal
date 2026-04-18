---
id: TASK-128
title: File Import / Export and Auto-Save
status: open
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: MobileApp
order: 11
prerequisites: [TASK-126, TASK-144]
---

## Description

Add file import (open `profiles.json` from device storage), export (share via Android/iOS share sheet), and auto-save (persist unsaved state across app restarts) to `ProfileListScreen` and `JsonPreviewScreen`.

## Acceptance Criteria

- [ ] `app/lib/services/file_service.dart` created
- [ ] **Import:** `file_picker` opens device file manager, `.json` only; on selection: parse + validate via `SchemaService`; if valid replace in-app profile state; if invalid show error dialog with schema violation details
- [ ] **Export:** `share_plus` triggers share sheet / iOS share extension with `profiles.json` as text attachment; also allows saving to local `Documents` via `path_provider`
- [ ] **Auto-save:** app persists current profile state to local app storage (`path_provider`) on every edit; state survives app restart
- [ ] Import and export buttons integrated into `ProfileListScreen` and `JsonPreviewScreen`
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/services/file_service.dart` (new)
- `app/lib/screens/profile_list_screen.dart` — add import/export buttons
- `app/lib/screens/json_preview_screen.dart` — add export button

## Test Plan

Manual: import a known-good `profiles.json` → verify profiles appear. Export → verify file is shareable. Close and reopen app → verify unsaved changes are restored.

## Prerequisites

- **TASK-126** — `ProfileListScreen` must exist before import/export buttons can be added
- **TASK-144** — `JsonPreviewScreen` must exist before export button can be added
