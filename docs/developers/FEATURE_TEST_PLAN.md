# Feature Test Plan — CLI Tool & Mobile App

This plan covers two newly shipped features:

- **Python CLI tool** (`scripts/pedal_config.py`) — scan, validate, upload
- **Flutter mobile app** (`app/`) — profile management, BLE connection, upload

Each section has two tracks:

- **Functional** — does it work correctly?
- **Usability** — can a musician do it without confusion?

Hardware required: ESP32 pedal flashed with current firmware, Bluetooth-capable host.

## How to record results

In the **Pass?** column of each table, replace the empty cell with one of:

| Mark | Meaning |
|------|---------|
| `✓` | Passed as expected |
| `✗` | Failed — file a defect task using the template at the bottom |
| `~` | Partial / acceptable with noted caveat |
| `N/A` | Not applicable in the current test environment |

Linked tasks: TASK-149 (validate), TASK-150 (scan), TASK-151 (upload), TASK-152 (upload-config),
TASK-153 (home/BLE), TASK-154 (profiles), TASK-155 (action editor),
TASK-156 (upload/preview), TASK-157 (E2E/integration/edge cases).

---

## Test Environment Setup

Before running any test:

1. Flash current firmware to the pedal (`make run-esp32`)
2. Power on the pedal — power LED solid, BT LED off until connected
3. For CLI tests: `pip install -r requirements.txt` in a fresh venv
4. For app tests: `flutter run` on a connected Android or iOS device (or emulator for UI-only tests)
5. Keep `data/profiles.json` and `data/config.json` as the reference baseline — restore from git after destructive tests

---

## Part 1 — Python CLI Tool

### 1.1 `validate` — Profile JSON

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| V-01 | `python scripts/pedal_config.py validate data/profiles.json` | Exit 0, no errors printed | |
| V-02 | `python scripts/pedal_config.py validate data/config.json --hw` | Exit 0, no errors printed | |
| V-03 | Create `bad.json` with `{"profiles": [{"name": "X"}]}` (missing `buttons`). Run validate. | Exit 1, error message names the missing field and path | |
| V-04 | Create `bad.json` with a button action `{"type": "SendCharAction"}` (missing `value`). Run validate. | Exit 1, error mentions `value` | |
| V-05 | Run validate on a file that does not exist | Non-zero exit, clear error message (not a Python traceback) | |
| V-06 | Run validate on a file that is not valid JSON (e.g. truncated) | Non-zero exit, clear error message | |

**Usability check V-U1:** Read the error output for V-03 aloud. Could a non-developer understand which field is wrong and where? If not, note it.

---

### 1.2 `scan` — BLE Discovery

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| S-01 | Pedal on, BLE enabled on host. `python scripts/pedal_config.py scan` | Pedal appears in output within 5 s with name and RSSI | |
| S-02 | Pedal off. Run scan. | Output indicates no devices found; does not crash | |
| S-03 | BLE adapter disabled on host. Run scan. | Clear error message (not a Python traceback) | |
| S-04 | Run scan with two pedals powered on simultaneously | Both appear in output with distinct MAC addresses | |

---

### 1.3 `upload` — Profile Upload

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| U-01 | `python scripts/pedal_config.py upload data/profiles.json` | Progress shown; "OK" from pedal; LED blinks 3× | |
| U-02 | After U-01, press buttons A–D on the pedal | Each button sends the action defined in Profile 01 | |
| U-03 | Edit `profiles.json` — change Profile 01 Button A value. Upload. | New keypress fires when A pressed (verify in a text editor) | |
| U-04 | Upload a profiles.json that fails schema validation | CLI reports error before connecting to pedal; no upload attempt | |
| U-05 | Start upload, physically disconnect USB power from pedal mid-transfer | CLI reports connection loss clearly | |
| U-06 | Upload an empty `{"profiles": []}` | Firmware responds with error; CLI displays pedal's error message | |
| U-07 | Upload a profiles.json that is schema-valid but semantically odd (e.g. all actions `(none)`) | Upload succeeds; pedal accepts it; no LEDs blink error | |

---

### 1.4 `upload-config` — Hardware Config Upload

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| C-01 | `python scripts/pedal_config.py upload-config data/config.json` | "OK" from pedal; LED blinks 3× | |
| C-02 | Reboot pedal after C-01; press buttons | Hardware configuration takes effect correctly | |
| C-03 | Upload `config.json` with `numButtons: 5` but only 4 `buttonPins` entries | CLI validates against schema; reports mismatch before upload | |

---

### 1.5 CLI Usability Assessment

Test with a musician who has never used a command line tool before (or simulate their perspective):

| # | Scenario | Usability criteria |
|---|----------|--------------------|
| CU-01 | Ask tester to upload profiles without instructions | They should be able to figure out the command from `--help` output alone |
| CU-02 | Introduce a typo in the JSON. Ask tester to fix using validate output | Error message should point to the right line/field |
| CU-03 | Scan with no pedal nearby | Output must not look like a crash; should clearly say "no devices found" |

---

## Part 2 — Mobile App

### 2.1 First Launch & Home Screen

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| H-01 | Launch app fresh (no saved state) | Home screen loads; three cards visible; "Not connected" shown in grey | |
| H-02 | Observe card states | "Upload" card is greyed out (disabled); other two are active | |
| H-03 | Rotate to landscape | No overflow, no clipped text, layout usable | |

**Usability check H-U1:** Show the home screen to someone unfamiliar with the app. Ask: "What would you do first?" The expected answer is "Connect to pedal" or "Edit profiles." If they are confused, note it.

---

### 2.2 BLE Scanner & Connection

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| SC-01 | Tap "Connect to pedal" with pedal powered on | Scanner opens; pedal appears in list within 10 s | |
| SC-02 | Check signal bar on scan result | Signal bar reflects RSSI (move pedal closer/farther; bar changes on rescan) | |
| SC-03 | Tap "Connect" on the pedal | Loading indicator shown; home screen returns; status shows "Connected" in green | |
| SC-04 | Confirm "Upload" card is now enabled on home screen | Card becomes active after successful connection | |
| SC-05 | Power off pedal. Wait 10 s. | App detects disconnection; "Connected" reverts to "Not connected"; "Upload" disabled | |
| SC-06 | Open scanner with Bluetooth disabled on phone | Clear message: "Bluetooth is off" with option to enable | |
| SC-07 | Deny Bluetooth permission (first run on Android 12+) | Clear message about permissions; no crash; link to settings | |
| SC-08 | Scan with pedal powered off | "No devices found" message (or empty list) after timeout; no crash | |
| SC-09 | Connect, then go to Profile List, then return to Home | Connection persists; still shows "Connected" | |

**Usability check SC-U1:** Time how long it takes a new user to connect the pedal from app launch. Target: under 30 seconds. Note any confusion points.

---

### 2.3 Profile List Screen

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| PL-01 | Tap "Edit profiles" with no profiles loaded | Empty list shown with add (+) button visible; no crash | |
| PL-02 | Tap + to add a profile | Dialog appears with name (required) and description (optional) fields | |
| PL-03 | Leave name blank, tap Save | Name field shows validation error; dialog stays open | |
| PL-04 | Enter name "My Test Profile", tap Save | Profile appears in list | |
| PL-05 | Add 3 profiles. Drag to reorder | Reorder persists after release | |
| PL-06 | Tap trash icon on a profile | Profile removed from list | |
| PL-07 | Import a valid `profiles.json` via Import button | All profiles load; validation banner shows green | |
| PL-08 | Import an invalid JSON file | Error shown; existing profiles not lost | |
| PL-09 | Import a file that passes JSON parsing but fails schema | Error shown with schema violation details; existing profiles not lost | |
| PL-10 | Export profiles to Downloads | File appears in Downloads; is valid JSON matching current state | |
| PL-11 | Tap "JSON Preview" | JSON preview screen opens showing current profiles | |
| PL-12 | With an invalid profile in the list (e.g. action missing `value`) | Validation banner shows red "N error(s)" | |
| PL-13 | Tap the red error banner | Error details dialog opens listing specific problems | |

**Usability check PL-U1:** Ask tester: "You want to make a copy of your profile setup to share with a friend. How would you do it?" Expected flow: Export → share the file. If they can find this independently, usability is good.

**Usability check PL-U2:** Is the difference between "Import" (replace all) and "Add profile" (add one) clear to a non-technical user? Note.

---

### 2.4 Profile Editor Screen

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| PE-01 | Tap a profile in the list | Profile editor opens; shows buttons A, B, C, D as rows | |
| PE-02 | Tap button A row | Action editor opens for slot A | |
| PE-03 | Return without saving | Original action unchanged | |
| PE-04 | A profile with a named action (e.g. "Prev Page") | Display name shown in the row, not the raw type | |
| PE-05 | A profile where a button has no action | Row shows "(none)" clearly | |

---

### 2.5 Action Editor Screen

Test each action type group:

#### Named Key Actions

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| AE-01 | Select action type "Key (named)" | Value field appears with autocomplete | |
| AE-02 | Type "KEY_F" in value field | Autocomplete suggestions appear (KEY_F1, KEY_F2 … KEY_F12, etc.) | |
| AE-03 | Select KEY_PAGE_UP, add display name "Next Page", tap Save | Profile row shows "Next Page"; JSON preview shows correct structure | |
| AE-04 | Select action type "Media Key" | Value field shows autocomplete; type "PLAY" to filter | |
| AE-05 | Select MEDIA_PLAY_PAUSE, save | JSON shows `"type": "SendMediaKeyAction", "value": "MEDIA_PLAY_PAUSE"` | |

#### String & Raw Key Actions

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| AE-06 | Select "Type String", enter "ctrl+z" | JSON shows `"type": "SendStringAction", "value": "ctrl+z"` | |
| AE-07 | Select "Key (raw HID)", enter "0x28" | JSON shows `"type": "SendKeyAction", "value": "0x28"` | |

#### Pin Actions

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| AE-08 | Select "Pin High" | Pin number field appears | |
| AE-09 | Enter pin 27, save | JSON shows `"type": "PinHighAction", "pin": 27` | |
| AE-10 | Enter a non-numeric pin value | Pin field shows error; Save blocked | |
| AE-11 | Select "Pin High While Pressed", enter pin 14 | JSON shows `"type": "PinHighWhilePressedAction", "pin": 14` | |

#### Serial Output

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| AE-12 | Select "Serial Output", enter "test message" | JSON shows `"type": "SerialOutputAction", "value": "test message"` | |

#### Save / Cancel

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| AE-13 | Tap Save with no action type selected (type = empty) | Save button disabled or shows validation error | |
| AE-14 | Tap Save with action type set | Returns to Profile Editor; row updates | |
| AE-15 | Navigate back without tapping Save | No change to the action | |

**Usability check AE-U1:** Show a musician the action editor. Ask them to set button A to "Play/Pause media." They should find `Media Key` → `MEDIA_PLAY_PAUSE` without guidance. Time how long it takes. Target: under 60 seconds.

**Usability check AE-U2:** Is it clear what "Key (named)" vs "Key (raw HID)" vs "Media Key" means to a non-developer? Note confusion points.

---

### 2.6 Upload Screen

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| UP-01 | Open upload screen while not connected | Upload button disabled; clear message that connection is needed | |
| UP-02 | Connect to pedal, open upload screen | Upload button enabled; profile count and validation status visible | |
| UP-03 | With valid profiles, tap "Upload Profiles" | Progress bar animates; success message (SnackBar or dialog) | |
| UP-04 | After UP-03, press button A on the pedal in a text editor | Correct keypress fires | |
| UP-05 | Make profiles invalid (delete required field via JSON edit + import). Open upload. | Upload button disabled; red validation card visible | |
| UP-06 | Tap the error card | Error details shown | |
| UP-07 | Disconnect pedal mid-upload | Error dialog shown; app does not crash | |
| UP-08 | Tap "Upload Hardware Config" | Progress and success/failure shown; pedal reboots after reboot | |
| UP-09 | Tap "View JSON Preview" from upload screen | Preview opens with current profiles JSON | |

**Usability check UP-U1:** After editing profiles on the profile list, ask tester: "How would you now send this to the pedal?" They should navigate to Upload without instructions. If they try to upload from the profile list, note it as a UX gap.

---

### 2.7 JSON Preview Screen

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| JP-01 | Open JSON preview | Monospace, readable JSON displayed | |
| JP-02 | Tap "Copy to clipboard" | Clipboard contains the JSON (verify by pasting) | |
| JP-03 | Tap "Share" | System share sheet opens with JSON content | |

---

### 2.8 End-to-End Musician Workflow Tests

These simulate real use cases from `docs/musicians/PROFILES.md`. Each requires a connected pedal and a host device to receive keystrokes.

#### E2E-01: Score Navigator Setup

Goal: configure button A to go to the previous page in a PDF reader.

1. Open app → Edit profiles
2. Add a new profile named "Score Navigator"
3. Edit button A → Key (named) → KEY_PAGE_UP → Display name "Prev Page" → Save
4. Edit button B → Key (named) → KEY_PAGE_DOWN → Display name "Next Page" → Save
5. Upload profiles
6. Open a PDF on the connected computer
7. Press A on the pedal → page goes back
8. Press B → page goes forward

**Pass criteria:** All 8 steps completed without help; keypresses work correctly.

#### E2E-02: Media Playback Control

Goal: configure a button to play/pause media.

1. Add profile "Media Controls"
2. Set button A → Media Key → MEDIA_PLAY_PAUSE
3. Upload
4. Open a media player on the connected host
5. Press A → playback toggles

**Pass criteria:** Correct media key fires; no false keypresses.

#### E2E-03: Import Shared Profile

Goal: receive a profiles.json from a colleague and load it.

1. Receive a `profiles.json` file (use the project's `data/profiles.json`)
2. Open app → Edit profiles → Import
3. Browse to and select the file
4. Confirm all 7 pre-configured profiles load with correct names

**Pass criteria:** Import completes; validation banner shows green; profile names match source file.

#### E2E-04: Quick Message via String Typing

Goal: configure a "Starting now" button for a live stream greeting.

1. Add or edit a profile
2. Set button A → Type String → "Hello chat, going live now!"
3. Upload
4. Focus a chat input on the connected host
5. Press A → the full string is typed

**Pass criteria:** String appears exactly as typed; no missing characters.

---

### 2.9 App Usability Assessment — Overall

Rate each criterion 1–5 after completing the tests above (1 = major problem, 5 = excellent):

| Criterion | Rating (1–5) | Notes |
|-----------|:------------:|-------|
| Finding how to connect to the pedal | | |
| Understanding the profile / button hierarchy | | |
| Picking the right action type from the dropdown | | |
| Knowing when to upload vs. just save | | |
| Understanding the validation banner | | |
| Recovering from a BLE connection error | | |
| Import / Export discoverability | | |
| Overall confidence after 15 minutes of use | | |

Any criterion rated ≤ 3 should be filed as a UX improvement task.

---

## Part 3 — CLI + App Integration

These tests verify that the CLI and app are interchangeable: a file exported from one can be uploaded by the other.

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| I-01 | Export profiles.json from the app; upload it with the CLI | CLI upload succeeds; pedal reflects the app-authored profiles | |
| I-02 | Edit profiles.json in a text editor; import into the app | App shows correct profiles; no validation errors for valid JSON | |
| I-03 | Use app to upload profiles; use CLI to validate the same file | CLI validate exits 0 | |
| I-04 | Use CLI to upload; use app to show JSON preview of same file after import | Preview matches the uploaded file | |

---

## Part 4 — Edge Cases & Regression

| # | Step | Expected result | Pass? |
|---|------|----------------|-------|
| R-01 | Upload 7 profiles (maximum). Press SELECT to cycle through all 7 | LED indicator shows correct binary pattern for each (see USER_GUIDE.md LED table) | |
| R-02 | Upload 1 profile. Cycle SELECT | Single profile; LED shows pattern 1 (LED1 only) | |
| R-03 | Rapidly press a button 10 times | Pedal sends exactly 10 keypresses; no drops or doubles | |
| R-04 | Hold a button for 3 seconds (no long-press action configured) | Single keypress fires on press; no spurious repeat | |
| R-05 | Upload profiles while pedal is actively sending keystrokes | Upload queues correctly; no lost BLE packets | |
| R-06 | Close app during upload | App close handled gracefully; pedal either completes or times out cleanly | |
| R-07 | Import a profiles.json with 7 profiles → add an 8th in the app | Schema validation error shown (max 7 for 3-bit LED) | |

---

## Defect Logging Template

For each failed test, record:

```
Test ID: 
Title:
Steps to reproduce:
Expected:
Actual:
Severity: Critical / High / Medium / Low
Type: Functional / Usability / Crash
Screenshot / log:
```

File as a task in `docs/developers/tasks/open/` using `/task-new`.
