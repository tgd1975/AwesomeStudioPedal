---
name: ble-reset
description: Recover from flaky BLE pairing with the pedal — disconnect, remove the bond, scan, pair, connect, verify. Reads the pedal MAC from $ASP_PEDAL_MAC. Use this any time you would hand-roll `bluetoothctl disconnect/remove/pair` retries on the pedal.
---

# ble-reset

Invoked as `/ble-reset` when BLE pairing with the pedal flakes — the
typical signal is the agent (or the user) running
`bluetoothctl disconnect <MAC>` two or more times in a row, or seeing
a stale "Connected: yes" while the host can no longer talk to the
pedal.

The skill replaces the freehand retry dance with one canonical
recovery sequence. Each step is bounded by a timeout; on first failure
the skill exits non-zero with a clear message naming the failed step,
so you have one tool to reach for instead of guessing.

## Pedal MAC — single source

The skill reads the pedal MAC from `$ASP_PEDAL_MAC` (set in `.envrc`,
see [.envrc.example](../../../.envrc.example) and the
"Project env vars (direnv)" section of
[DEVELOPMENT_SETUP.md](../../../docs/developers/DEVELOPMENT_SETUP.md#project-env-vars-direnv)).
If the variable is empty or unset, the skill exits with a clear
"set ASP_PEDAL_MAC" message — it does **not** scan for the pedal first
and pick a guess, because that hides the real configuration error.

The hex `24:62:AB:D4:E0:D2` from the project's reference pedal must
not appear inline anywhere in this skill — re-pairing a different
pedal must require only a `.envrc` edit.

## Implementation

The skill delegates to
[`.claude/skills/ble-reset/ble_reset.sh`](ble_reset.sh) so the whole
sequence is one allowlistable Bash invocation:

```bash
.claude/skills/ble-reset/ble_reset.sh
```

The script runs, in order:

1. **Disconnect** — `bluetoothctl disconnect $ASP_PEDAL_MAC`. Tolerated
   if the device wasn't connected (host already considers it gone).
2. **Remove bond** — `bluetoothctl remove $ASP_PEDAL_MAC`. Forces a
   fresh pair on the next attempt; clears stale link-key state.
   Tolerated if the device wasn't known.
3. **Scan** — `bluetoothctl --timeout 8 scan on`. The pedal must
   re-appear in the cache after scan. Hard-fail if not seen.
4. **Pair** — `bluetoothctl pair $ASP_PEDAL_MAC`. Hard-fail on
   non-zero / `AuthenticationFailed` / `AuthenticationCanceled`.
5. **Connect** — `bluetoothctl connect $ASP_PEDAL_MAC`. Hard-fail on
   non-zero.
6. **Verify** — `python scripts/pedal_config.py scan` and confirm the
   pedal name appears in output (the same end-to-end signal used in
   feature tests). Hard-fail if it doesn't.

On any hard-fail, the script prints:

```
ble-reset FAILED at step <N> (<step name>): <stderr from the step>
```

…and exits non-zero. On full success it prints `ble-reset OK` and the
output of step 6.

## When to use

- BLE pairing or connection has flaked and a manual retry didn't fix it.
- After flashing new pedal firmware that changes pairing behaviour
  (e.g. PIN-bonded vs Just Works) — the host's stale link key will
  reject the new advertisement; this clears it.
- Before running a feature-test scenario that depends on a clean
  pairing baseline.

## When NOT to use

- The pedal is unpowered or out of range. The scan step will fail
  legitimately; rerunning won't help. Power/move the pedal first.
- You want a one-off `bluetoothctl` command (e.g. just `info <MAC>`).
  Run that directly — this skill is the recovery sequence, not a
  general-purpose `bluetoothctl` wrapper.

## Skill registration

Registered in [.vibe/config.toml](../../../.vibe/config.toml)'s
`enabled_skills` list per the project's CLAUDE.md skill-registration
rule.
