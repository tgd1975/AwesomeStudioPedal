# Contributing a Profile Set

Thank you for sharing your AwesomeStudioPedal setup with the community!
This folder contains user-contributed profile sets — each file covers one real-world
use-case (e.g. "page-turner for sheet music" or "DAW transport control").

---

## What Makes a Good Profile Set

- **Focused use-case** — one app or workflow per file, not a catch-all.
- **Descriptive button labels** — use the action name _and_ the key (e.g.
  `"Play/Pause — SPACE"`, not just `"Button A"`).
- **Filled `description` field** — explain the target app, the hotkeys it
  expects, and where to configure them (Settings > Keyboard Shortcuts, etc.).
- **Tested** — set `_meta.tested` to the platform(s) you verified on
  (e.g. `"macOS 14, Ableton Live 11"`).
- **Minimal button count** — place the file in the lowest subfolder that still
  covers all required buttons (`1-button/`, `2-button/`, `3-button/`, or
  `4-button/`). The subfolder name drives the `minButtons` value shown in the
  gallery; do not use keys beyond what the subfolder allows.

---

## File Format

Each file is a valid `profiles.json` (validated against
`data/profiles.schema.json`) with one extra top-level `_meta` block:

```json
{
  "_meta": {
    "id":          "page-turner-basic",
    "author":      "your-github-username",
    "tags":        ["sheet-music", "musicians"],
    "tested":      "macOS 14, forScore 13",
    "minFirmware": "1.2.0"
  },
  "profiles": [
    {
      "name":        "01 Page Turner",
      "description": "...",
      "buttons": { ... }
    }
  ]
}
```

| Field | Required | Description |
|---|---|---|
| `_meta.id` | Yes | Unique slug — same as the filename without `.json` |
| `_meta.author` | Yes | Your GitHub username |
| `_meta.tags` | Yes | 1–5 lowercase strings describing the use-case |
| `_meta.tested` | Yes | OS / app version string you tested on |
| `_meta.minFirmware` | No | Earliest firmware version required; omit if using only basic action types |

See [docs/builders/KEY_REFERENCE.md](../docs/builders/KEY_REFERENCE.md) for all
available action types and key names.

---

## Terminology

| Term | Meaning |
|---|---|
| **Profile set** | A single `.json` file containing one or more `profiles` entries for one use-case |
| **Profile** | One named mode within a set (e.g. "Recording", "Playback") |
| **Button slot** | The physical button label: `A`, `B`, `C`, `D` |
| `minButtons` | The minimum number of physical buttons required, derived from the subfolder name |

---

## Step-by-Step: How to Contribute

1. **Fork** the repository and create a branch:
   `git checkout -b profiles/your-profile-name`

2. **Choose the right subfolder** based on how many buttons your profile uses:
   - `profiles/1-button/` — only button A
   - `profiles/2-button/` — buttons A and B
   - `profiles/3-button/` — buttons A, B, and C
   - `profiles/4-button/` — all four buttons A–D

3. **Create your file** — e.g. `profiles/2-button/page-turner-basic.json`.
   Use the template above. The filename (without `.json`) must match `_meta.id`.

4. **Validate locally**:

   ```bash
   npm run validate-profiles
   ```

   All files must pass before submitting.

5. **Regenerate the index**:

   ```bash
   npm run generate-profiles-index
   ```

   Commit the updated `profiles/index.json` together with your new file.

6. **Open a Pull Request** — describe the use-case, the app, and how you tested
   it. CI will automatically re-validate and check button counts.

---

## Profile Set Naming Conventions

- Use kebab-case for filenames: `daw-transport-minimal.json`
- Start profile names with a two-digit ordinal when a set has multiple modes:
  `"01 Recording"`, `"02 Playback"`
- Keep names short enough to fit on a card in the gallery (~30 chars)

---

Questions? Open an issue or start a Discussion on GitHub.
