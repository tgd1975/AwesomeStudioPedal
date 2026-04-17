---
id: TASK-054
title: Check and resolve license inconsistency
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Audit the repository for license inconsistencies — mismatching SPDX headers, conflicting
`LICENSE` file vs. `package.json` / `library.json` declarations, or missing license notices.

## Acceptance Criteria

- [ ] All source files have consistent license headers (or none, as appropriate)
- [ ] `LICENSE` file matches the declared license in any package manifests
- [ ] No conflicting SPDX identifiers across files
- [x] Resolution documented in `## Notes` below

## Notes

The `LICENSE` file contained GPL-3 (GitHub default), while `README.md` declared MIT.
No source files had license headers. Dependencies audited:

- ArduinoJson — MIT
- ESP32 BLE Keyboard — no license file (no copyleft conflict)
- Arduino-ESP32 framework — LGPL-2.1 (linking permitted from any license)
- Adafruit nRF52 Arduino — MIT/BSD-3
- GoogleTest / Unity — BSD-3 / MIT (test-only)

All compatible with MIT. `LICENSE` replaced with standard MIT text, copyright `tgd1975`.
