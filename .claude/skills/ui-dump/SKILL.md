---
name: ui-dump
description: Drive the Android UI on a connected device — dump the UI tree, optionally find a node by text and report its bounds, and optionally tap it. Replaces the hand-rolled `sleep 1 && adb shell uiautomator dump … pull … python3 ui_find.py` loop. Use this any time you would type `adb shell uiautomator dump` to drive the Pixel app, exploratory or otherwise.
---

# ui-dump

Invoked as:

- `/ui-dump` — print the current UI tree.
- `/ui-dump find "<needle>"` — print the bounds + text of the first node
  whose `text` or `content-desc` contains `<needle>`. Exit non-zero if
  not found.
- `/ui-dump tap "<needle>"` — find as above and tap the centre of the
  matched node. Exit non-zero if not found.

This is the canonical wrapper for the dump → pull → find / tap loop.
**Any time you would otherwise type `adb shell uiautomator dump`, invoke
this skill instead.** It does not matter whether the goal is exploratory
("tap through it once and see what happens") or scripted — both go
through `/ui-dump`. The structured scenario verifier `/verify-on-device`
is for end-to-end scenarios with pass/fail assertions; for everything
else, this is the entry point.

## Device serial — single source

The skill reads the device serial **once**, in this order, and exits with
a clear error if it can't pin a single device:

1. `ANDROID_SERIAL` env var — the standard adb env var. If set, used
   verbatim and not re-validated against `adb devices`.
2. Otherwise `adb devices` — if exactly one device is in state `device`,
   use its serial. If 0 or >1, stop and ask the user to set
   `ANDROID_SERIAL=<serial>` or unplug the extras.

The serial is **never** hard-coded in this skill. The hex
`4C200DLAQ0056N` from the project's Pixel 9 has no place here — survive
phone swaps by routing through `ANDROID_SERIAL`.

## Implementation

The skill delegates to a helper script
[`.claude/skills/ui-dump/ui_dump.sh`](ui_dump.sh) so the whole
operation (dump on device, pull to host, optional Python parse for
find/tap) is one allowlistable Bash invocation:

```bash
.claude/skills/ui-dump/ui_dump.sh                  # dump
.claude/skills/ui-dump/ui_dump.sh find "<needle>"  # dump + find
.claude/skills/ui-dump/ui_dump.sh tap  "<needle>"  # dump + find + tap
```

The script:

1. Resolves the serial (env var → adb devices → error).
2. `adb -s $SER shell uiautomator dump /sdcard/ui.xml`.
3. `adb -s $SER pull /sdcard/ui.xml /tmp/ui.xml`.
4. For `dump` — `cat /tmp/ui.xml` and exit.
5. For `find` / `tap` — Python parses `/tmp/ui.xml` with
   `ElementTree`, finds the first `<node>` whose `text` or
   `content-desc` contains the needle, prints `bounds=<x1,y1,x2,y2>
   text=<…>`, and (for `tap`) computes the centre and runs
   `adb -s $SER shell input tap $cx $cy`.

Errors surface verbatim — no swallowing.

## When to use

- Exploratory drives: "tap through the Action Editor and see what
  happens when I pick Media Key" — one or many `/ui-dump tap`
  invocations.
- Quick state checks: "is the Connect button visible right now?" —
  `/ui-dump find "Connect"`.
- One-shot regressions where wiring up a full `verify-on-device`
  scenario is overkill.

## When NOT to use

- Full feature-test or defect verification with named scenarios. Use
  `/verify-on-device <TASK-ID> <SCENARIO-ID>` — it has the
  scenario catalog, the pass/fail assertions, and the result-handling
  protocol that this skill deliberately omits.
- Multi-step scripted recipes. Build them up as a `verify-on-device`
  scenario instead so future runs are deterministic.

## Skill registration

Registered in [.vibe/config.toml](../../../.vibe/config.toml)'s
`enabled_skills` list per the project's CLAUDE.md skill-registration
rule.
