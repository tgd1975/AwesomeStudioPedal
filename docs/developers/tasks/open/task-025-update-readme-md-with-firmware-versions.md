---
id: TASK-025
title: Update README.md with firmware versions
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add a "Firmware Versions" section to `README.md` listing the current stable release and the
two previous versions, with download links for both ESP32 and nRF52840 builds.

## Acceptance Criteria

- [ ] `README.md` has a "Firmware Versions" section
- [ ] Current stable version is listed with download links for both platforms
- [ ] Up to two previous versions listed with links
- [ ] Links follow the naming convention: `awesome-pedal-[platform]-[version].bin`
- [ ] Section is kept in sync with actual GitHub Releases

## Notes

Version retention policy: keep current stable + 2 previous versions.
Delete older releases via the cleanup script (TASK-029) to keep the section clean.

Example section structure:

```markdown
## Firmware Versions

**Current Stable**: v1.2.0

- ESP32 Firmware: [awesome-pedal-esp32-v1.2.0.bin](...)
- nRF52840 Firmware: [awesome-pedal-nrf52840-v1.2.0.bin](...)

**Previous Versions**:

- v1.1.0: [ESP32](...) | [nRF52840](...)
- v1.0.0: [ESP32](...) | [nRF52840](...)
```

Related: TASK-020 (release workflow), TASK-027 (upload instructions), TASK-029 (cleanup script).
