---
id: TASK-175
title: Flutter — CommunityProfilesService
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 10
---

## Description

Create `app/lib/services/community_profiles_service.dart`. The service fetches
`profiles/index.json` from GitHub Pages, fetches individual profile set files on demand,
and caches both locally using the `http` package and `path_provider`. Must be
unit-testable with a mock HTTP client.

Public API:

- `Future<List<ProfileIndexEntry>> fetchIndex()` — fetches (or returns cached) index
- `Future<ProfilesConfig> fetchProfileSet(String id)` — fetches (or returns cached) profile set file
- `Future<void> clearCache()` — removes all cached files

## Acceptance Criteria

- [ ] `app/lib/services/community_profiles_service.dart` created
- [ ] `fetchIndex()` fetches `index.json` from GitHub Pages, caches locally, returns cached data on subsequent calls
- [ ] `fetchProfileSet(id)` looks up the file path from the index, fetches and caches the JSON, returns a parsed `ProfilesConfig`
- [ ] `clearCache()` removes cached index and all cached profile set files
- [ ] HTTP client injected via constructor (or a factory) so tests can supply a mock
- [ ] Unit tests cover: fresh fetch, cache hit, network error (throws), malformed JSON (throws)
- [ ] Tests pass via `flutter test` in `app/`

## Notes

`http` and `path_provider` are already in `pubspec.yaml` (check before adding). The
`ProfilesConfig` model already exists from TASK-125 (`app/lib/models/`).

Depends on: TASK-168 (index.json format is the contract this service consumes).
