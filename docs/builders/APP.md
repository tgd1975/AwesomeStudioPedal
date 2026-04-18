# Mobile App — Builder's Guide

The AwesomeStudioPedal companion app lets you edit profiles and upload them to the pedal
over Bluetooth, without a computer or a USB cable.

---

## 1. Installing

### Android

- **Play Store:** _(link coming — not yet published)_
- **Sideload APK:** Download the latest `app-release.apk` from the
  [GitHub Releases](https://github.com/your-org/AwesomeStudioPedal/releases) page.
  On your Android device: Settings → Security → Install unknown apps → allow your browser
  or Files app → open the downloaded APK.

### iOS

iOS distribution is not yet set up through the App Store. To install on a personal device
you need Xcode and an Apple developer account:

1. Clone the repository on a Mac.
2. Run `flutter build ios --no-codesign` from the `app/` directory.
3. Open `app/ios/Runner.xcworkspace` in Xcode and deploy to your device.

See [app/README.md](../../app/README.md) for full iOS build instructions.

---

## 2. Connecting to the pedal

The pedal advertises two separate Bluetooth services:
- **HID keyboard profile** — paired from iOS/Android Settings. This is what makes the
  pedal act as a USB keyboard. You don't need to touch this for the app.
- **Config GATT service** — this is what the app connects to for profile uploads.
  It's a separate, independent BLE connection.

**Steps:**
1. Open the app → tap **Connect to pedal**.
2. The scan screen appears and lists nearby pedals by name.
3. Each result shows a signal-strength indicator (bars).
4. Tap **Connect** next to your pedal.
5. The app returns to the home screen showing "Connected".

> If the pedal doesn't appear: confirm the BLE LED on the pedal is blinking (advertising
> mode). Power-cycle the pedal if needed.

---

## 3. Editing profiles

### Profile list

The **Profiles** screen shows all profiles in order. You can:
- **Add** a new profile with the + button (enter a name and optional description).
- **Reorder** profiles by dragging the handle on the right.
- **Delete** a profile with the trash icon.

A colour-coded banner at the top shows whether the current profile set is valid:
- Green **Valid ✓** — ready to upload.
- Red **N error(s)** — tap the banner to see what's wrong.

### Profile editor

Tap a profile to open the editor. It shows one row per button slot (A–D by default;
more if your hardware config defines additional buttons). Tap a slot to edit its action.

### Action editor

The action editor has:
1. **Action Type** dropdown — select the action for this button press.
2. **Value / Key field** — appears for key-based actions; autocompletes from the key list.
3. **Display name** — optional label shown in the profile list.

**Simple action types:**
| Type | What it does |
|------|-------------|
| Key (named) | Sends a named HID key (e.g. `KEY_F5`) |
| Key (raw HID) | Sends a raw HID key code |
| Type String | Types a string character by character |
| Media Key | Sends a media control key (play, next, volume…) |
| Serial Output | Sends a string over the UART serial port |
| Pin High / Low / Toggle | Sets or toggles a GPIO output pin |
| Pin High/Low While Pressed | Holds a pin state for the duration of the button press |

**Advanced action types:**
| Type | What it does |
|------|-------------|
| Delayed Action | Waits N milliseconds, then executes a sub-action |
| Macro | Executes a sequence of steps; each step is a list of simultaneous actions |

Each button slot also has optional **Long press** and **Double press** sections — expand
them with the toggle to assign separate actions to those gestures.

---

## 4. Uploading

From the home screen, tap **Upload**. The upload screen shows:
- A summary of the current profiles (count + last-modified time).
- A validation check — the **Upload Profiles** button is disabled if there are errors.
- A linear progress bar that advances as chunks are sent to the pedal.
- A **Upload Hardware Config** button for updating pin assignments.

**Upload status:**
- **Success snackbar** — "Upload successful!" appears at the bottom.
- **Error dialog** — if the pedal responds with an error code, a dialog explains the reason
  (e.g. "parse_failed", "too_large"). Fix the issue and retry.

---

## 5. Importing / exporting JSON

### Import

Tap the **↑ Import** icon in the Profiles screen toolbar.
The system file picker opens — choose a `profiles.json` file.
The app validates the file before loading it; if validation fails, an error dialog lists
the problems.

### Export

Tap the **Share** icon in the Profiles screen toolbar.
The standard iOS/Android share sheet appears — send the file to any app (Files,
Dropbox, email, etc.).

You can also tap **JSON Preview** to view the raw JSON and copy it to the clipboard.

### Auto-save

The app automatically saves the current profile state to local storage whenever you
make a change. If you close and reopen the app, your last unsaved edits are restored.

---

## 6. Hardware config tab

The hardware config (`config.json`) controls pin assignments and the number of buttons and
profiles. Edit it in a text editor and upload it via the **Upload Hardware Config** button
on the upload screen, or use the CLI: `python3 scripts/pedal_config.py upload-config config.json`.

See [HARDWARE_CONFIG.md](HARDWARE_CONFIG.md) for field reference.

---

## 7. iOS notes

- When you first use the app on iOS, a **Bluetooth permission prompt** appears. Tap **Allow**.
- The HID keyboard pair (from iOS Settings) and the app's GATT connection are independent.
  You can have both active simultaneously.
- iOS builds require macOS + Xcode. See [app/README.md](../../app/README.md) for details.
- `flutter build ios --no-codesign` must be run on a Mac — it is not available in the
  Linux dev container or CI.
