#!/usr/bin/env bash
# ble_reset.sh — canonical BLE pairing-recovery sequence for the pedal.
# See SKILL.md for usage. Invoked by the `/ble-reset` skill.

set -uo pipefail

MAC="${ASP_PEDAL_MAC:-}"

if [ -z "$MAC" ]; then
  echo "ble-reset: \$ASP_PEDAL_MAC is unset. Set it in .envrc (see .envrc.example)." >&2
  exit 2
fi

# Each step runs `bluetoothctl <cmd> <MAC>` and inspects exit + stderr. Steps
# 1 and 2 are tolerated (the pedal may already be disconnected / unknown);
# steps 3–6 are hard fails.

fail() {
  local step="$1" name="$2" detail="$3"
  echo "ble-reset FAILED at step $step ($name): $detail" >&2
  exit 1
}

run_bt() {
  # bluetoothctl exits 0 even on most errors — the actual signal is in
  # stdout/stderr text. Capture both, return both.
  bluetoothctl "$@" 2>&1
}

# Step 1 — disconnect (tolerated)
out=$(run_bt disconnect "$MAC")
echo "step 1 disconnect: $out"

# Step 2 — remove bond (tolerated)
out=$(run_bt remove "$MAC")
echo "step 2 remove: $out"

# Step 3 — scan; pedal must reappear in cache
echo "step 3 scan: scanning for 8 s …"
scan_out=$(timeout 10 bluetoothctl --timeout 8 scan on 2>&1)
echo "$scan_out"
if ! bluetoothctl devices | grep -qi "$MAC"; then
  fail 3 "scan" "pedal $MAC did not reappear after 8 s scan"
fi

# Step 4 — pair
out=$(run_bt pair "$MAC")
echo "step 4 pair: $out"
if echo "$out" | grep -Ei "AuthenticationFailed|AuthenticationCanceled|Failed to pair" >/dev/null; then
  fail 4 "pair" "$out"
fi
if ! echo "$out" | grep -Ei "Pairing successful|already paired" >/dev/null; then
  fail 4 "pair" "no 'Pairing successful' or 'already paired' marker in: $out"
fi

# Step 5 — connect
out=$(run_bt connect "$MAC")
echo "step 5 connect: $out"
if echo "$out" | grep -Ei "Failed to connect|not available" >/dev/null; then
  fail 5 "connect" "$out"
fi
if ! echo "$out" | grep -Ei "Connection successful" >/dev/null; then
  fail 5 "connect" "no 'Connection successful' marker in: $out"
fi

# Step 6 — verify with scripts/pedal_config.py
PEDAL_CONFIG="scripts/pedal_config.py"
if [ ! -f "$PEDAL_CONFIG" ]; then
  fail 6 "verify" "scripts/pedal_config.py not found (run from repo root)"
fi
PY="python3"
[ -x ".venv/bin/python" ] && PY=".venv/bin/python"
verify_out=$($PY "$PEDAL_CONFIG" scan 2>&1)
echo "step 6 verify:"
echo "$verify_out"
if ! echo "$verify_out" | grep -qi "AwesomeStudio"; then
  fail 6 "verify" "pedal_config.py scan did not report an AwesomeStudio device"
fi

echo "ble-reset OK"
