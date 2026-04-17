---
id: TASK-139
title: Profile Configurator — Long-Press and Double-Press Fields
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: LongDoublePress
order: 8
prerequisites: [TASK-108]
---

## Description

Extend the web profile configurator (`docs/tools/config-builder/builder.js` and `index.html`) so builders can configure `longPress` and `doublePress` sub-actions on any button slot. The sub-actions use the same action editor already in use for primary actions and for `DelayedAction`'s inner action.

## Acceptance Criteria

- [ ] Each button slot shows two optional collapsible sections: **Long Press** and **Double Press**
- [ ] Each section contains a full action editor (type dropdown + matching fields), reusing `renderActionFields`
- [ ] Sections are collapsed by default when no sub-action is configured; a toggle button expands/collapses them
- [ ] When a sub-action type is set to `(none)`, the key is omitted from the exported JSON entirely
- [ ] `buildJson` / `actionToJson` emits `longPress` and `doublePress` keys only when configured
- [ ] `populateForm` / `actionFromJson` reads `longPress` and `doublePress` from loaded JSON and pre-fills the fields
- [ ] The JSON preview updates live as sub-actions are edited
- [ ] The schema validator (`schema.json`) is updated to allow `longPress` and `doublePress` as optional action objects on button entries
- [ ] Existing profiles without `longPress`/`doublePress` load and export identically to before

## Files to Touch

- `docs/tools/config-builder/builder.js` — `emptyAction`, `renderProfileForm`, `renderActionFields`, `actionToJson`, `actionFromJson`, `buildJson`
- `docs/tools/config-builder/index.html` — any CSS needed for the collapsible sub-action sections
- `docs/tools/config-builder/schema.json` — add `longPress` and `doublePress` optional properties

## Implementation Notes

Extend `emptyAction()` to include `longPress: null` and `doublePress: null`. In `renderProfileForm`, after the primary action editor for each slot, render two `<details>` elements (native HTML collapsible) labelled **"+ Long Press"** and **"+ Double Press"**. Each `<details>` contains a `renderActionFields` call with `nested = true` (prevents nesting another `DelayedAction`).

`actionToJson` extension:
```js
if (a.longPress && a.longPress.type) r.longPress = actionToJson(a.longPress);
if (a.doublePress && a.doublePress.type) r.doublePress = actionToJson(a.doublePress);
```

`actionFromJson` extension:
```js
r.longPress = a.longPress ? actionFromJson(a.longPress) : null;
r.doublePress = a.doublePress ? actionFromJson(a.doublePress) : null;
```

`schema.json` additions — add to the button action object definition:
```json
"longPress":   { "$ref": "#/definitions/action" },
"doublePress": { "$ref": "#/definitions/action" }
```
Both optional (not in `required`).

## Test Plan

Manual test in browser:

1. Open the configurator. Expand **Long Press** on button A, select `SendMediaKeyAction`, choose `MEDIA_STOP`. Verify the JSON preview gains `"longPress": { "type": "SendMediaKeyAction", ... }`.
2. Collapse the **Long Press** section. Verify the key is still present in the JSON.
3. Set **Long Press** type back to `(none)`. Verify `longPress` is removed from the JSON.
4. Load `data/profiles.json` (which has a `longPress` on button A of profile 1). Verify the field is pre-filled correctly.
5. Click **Validate** — must pass schema validation with sub-actions present.
6. Load a profile without any `longPress`/`doublePress` keys — must behave identically to before this change.

## Notes

Sub-action editors must use `nested = true` in `renderActionFields` to prevent `DelayedAction` from appearing as a sub-action type (matching the same constraint already applied to `DelayedAction`'s inner action).
