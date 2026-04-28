---
id: TASK-125
title: Dart Data Models and Schema Validation Service
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: No
epic: MobileApp
order: 5
prerequisites: [TASK-123]
---

## Description

Implement Dart model classes for profiles, actions, hardware config, and macro steps, plus a `SchemaService` that validates JSON against the embedded schema assets. Models mirror the recursive logic of `config_loader.cpp` using the same field names.

## Acceptance Criteria

- [ ] `app/lib/models/profile.dart` — `Profile`: name, description, `Map<String, ActionConfig> buttons`
- [ ] `app/lib/models/action_config.dart` — `ActionConfig`: type, value, pin, steps, delayMs, name, optional `longPress`, `doublePress`
- [ ] `app/lib/models/hardware_config.dart` — `HardwareConfig`: numButtons, numProfiles, numSelectLeds, pins, LED assignments
- [ ] `app/lib/models/macro_step.dart` — `MacroStep`: `List<ActionConfig>`
- [ ] All models implement `toJson()` / `fromJson()`; round-trip is lossless
- [ ] `ActionConfig.fromJson` handles all action types recursively (mirrors `config_loader.cpp` logic)
- [ ] `app/lib/services/schema_service.dart` created with `validateProfiles(Map json)` and `validateConfig(Map json)` returning `ValidationResult`
- [ ] `SchemaService` loads schema assets from `pubspec.yaml` assets on first use (lazy load)
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/models/profile.dart` (new)
- `app/lib/models/action_config.dart` (new)
- `app/lib/models/hardware_config.dart` (new)
- `app/lib/models/macro_step.dart` (new)
- `app/lib/services/schema_service.dart` (new)

## Test Plan

**Unit tests**: covered by TASK-130 (round-trip tests for all action types; `SchemaService` acceptance/rejection tests).

## Prerequisites

- **TASK-123** — Flutter project and schema assets must exist
