# CLI Tool — `pedal_config.py`

`scripts/pedal_config.py` lets you upload profiles and hardware config to the pedal over
BLE from any machine with Python and a Bluetooth adapter. No app or cable required.

---

## 1. Prerequisites

- **Python 3.9 or later** — check with `python3 --version`
- **A Bluetooth adapter** — built-in on most laptops; USB dongles work too
- **Dependencies** — install once from the repo root:

```bash
pip install -r scripts/requirements.txt
```

This installs:
- `bleak` — cross-platform async BLE library
- `jsonschema` — JSON schema validation

**Linux note:** on some distributions, your user account must be in the `bluetooth` group
or you must run with `sudo`. If `scan` finds nothing, try `sudo python3 scripts/pedal_config.py scan`.

---

## 2. Scanning — find nearby pedals

```bash
python3 scripts/pedal_config.py scan
```

Expected output when the pedal is on and advertising:

```
Scanning for AwesomeStudioPedal (10 s)…
  AwesomePedal  AA:BB:CC:DD:EE:FF  RSSI -58 dBm
Found 1 device(s).
```

If nothing appears:
- Make sure the pedal is powered on.
- Confirm the BLE LED on the pedal is blinking (advertising mode).
- Check that Bluetooth is enabled on your computer.

---

## 3. Uploading profiles

```bash
python3 scripts/pedal_config.py upload data/profiles.json
```

Step-by-step what happens:

1. The script scans for a nearby pedal and connects automatically.
2. `profiles.json` is validated against `data/profiles.schema.json`.
3. The JSON is chunked into 512-byte BLE packets and written to the pedal.
4. A final end-of-transfer packet is sent.
5. The pedal responds on the status characteristic — the script waits up to 15 s.

Expected terminal output:

```
Connecting to AwesomePedal (AA:BB:CC:DD:EE:FF)…
Uploading profiles.json (3 profile(s), 847 bytes, 2 chunk(s))…
  chunk 1/2 … OK
  chunk 2/2 … OK
  EOF … waiting for status
Status: OK
Upload complete. LED confirmation: 3× blink.
```

**LED confirmation on the pedal:**
- **3 short blinks** — upload accepted and applied.
- **1 long blink** — upload failed (see error message in terminal for reason).

---

## 4. Validating without upload

```bash
python3 scripts/pedal_config.py validate data/profiles.json
```

This checks `profiles.json` against the JSON schema without connecting to the pedal.
Useful for catching typos before a BLE session.

Expected output for a valid file:

```
Validating data/profiles.json against data/profiles.schema.json…
Valid ✓
```

If the file has errors:

```
Validating data/profiles.json against data/profiles.schema.json…
ERROR: 'type' is a required property (path: profiles[0].buttons.A)
ERROR: 'value' must be a string (path: profiles[0].buttons.B)
2 error(s) found. Fix before uploading.
```

**Interpreting schema errors:**
- `'type' is a required property` — a button slot is missing the `"type"` field.
- `'value' must be a string` — the `value` field has the wrong data type.
- `path: profiles[N].buttons.X` — the error is in profile number N (zero-indexed), button slot X.

See [KEY_REFERENCE.md](KEY_REFERENCE.md) for valid key and action type names.

---

## 5. Uploading hardware config

```bash
python3 scripts/pedal_config.py upload-config data/config.json
```

This uploads `config.json` (GPIO pin assignments, LED counts, button count) to the
`CONFIG_WRITE_HW` characteristic. The pedal applies the hardware config immediately.

Expected output:

```
Connecting to AwesomePedal (AA:BB:CC:DD:EE:FF)…
Uploading config.json (187 bytes, 1 chunk)…
Status: OK
Hardware config applied.
```

You can edit `data/config.json` to remap pins or change the number of buttons without
recompiling firmware. See [HARDWARE_CONFIG.md](HARDWARE_CONFIG.md) for field reference.

---

## 6. Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `No devices found` | Pedal not advertising | Power-cycle the pedal; confirm BLE LED is blinking |
| `Permission denied` (Linux) | User not in `bluetooth` group | `sudo usermod -aG bluetooth $USER` then log out/in, or run with `sudo` |
| `Upload timed out` | BLE connection dropped mid-transfer | Move closer to the pedal; retry |
| `ERROR:too_large` | `profiles.json` exceeds 32 768 bytes | Reduce the number of profiles or shorten key names |
| `ERROR:parse_failed` | Pedal rejected JSON | Run `validate` first to find schema errors |
| `ERROR:bad_sequence` | Packet reordering (rare BLE issue) | Retry — the pedal resets its buffer automatically |
| `ERROR:invalid_config` | Hardware config values out of range | Check pin numbers are 0–39 and counts are within schema limits |

If the problem persists, open an issue and include the full terminal output.
