---
id: TASK-144
title: Profile Configurator UI — JsonPreviewScreen and Validation Banner
status: closed
closed: 2026-04-18
opened: 2026-04-18
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: MobileApp
order: 8
prerequisites: [TASK-126, TASK-125]
---

## Description

Implement `JsonPreviewScreen` (live JSON preview with copy/share) and the persistent
validation banner on `ProfileListScreen`. Both depend on `SchemaService` from TASK-125.

## Acceptance Criteria

- [ ] `app/lib/screens/json_preview_screen.dart` created
- [ ] `JsonPreviewScreen` shows live syntax-highlighted JSON of the current profile state
- [ ] Copy button copies JSON to clipboard
- [ ] Share button triggers Android/iOS share sheet via `share_plus`
- [ ] Persistent validation banner added to `ProfileListScreen`:
  - Green "Valid ✓" when `SchemaService.validateProfiles()` passes
  - Red "N error(s)" with error list when validation fails
  - Tapping an error navigates to the offending screen/field
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/screens/json_preview_screen.dart` (new)
- `app/lib/screens/profile_list_screen.dart` — add validation banner
- `app/lib/widgets/validation_banner.dart` (new)

## Test Plan

Widget test covered by TASK-130: validation banner shows red error count when invalid
profile is loaded. Manual: open JSON preview → verify output matches expected structure for
a known profile.

## Prerequisites

- **TASK-126** — `ProfileListScreen` must exist to add the banner
- **TASK-125** — `SchemaService` must exist for validation
