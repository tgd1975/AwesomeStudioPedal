---
id: TASK-072
title: Update musicians documentation - add prototype pictures
status: closed
closed: 2026-04-16
opened: 2026-04-11
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

Enhance `docs/musicians/USER_GUIDE.md` with prototype pictures to help users better understand the physical device. Pictures are in `docs/media/` — relative path from `docs/musicians/` is `../media/`.

## Acceptance Criteria

- [ ] Add `../media/prototype_pedal_open.jpg` in the setup or overview section of USER_GUIDE.md
- [ ] Add `../media/prototype_pedal_pressed.jpg` in the usage section to show the pedal being pressed
- [ ] Add descriptive captions for each image
- [ ] Optionally add `../media/prototype.jpg` to PROFILES.md as a device overview

## Notes

- Prefer `pedal_open` and `pedal_pressed` — they show the device from a musician's perspective
- ESP32 detail shots (`esp32_front`, `esp32_side`, `esp32_top`) are more relevant for builders (TASK-071)
- Image paths are relative: `../media/prototype_pedal_open.jpg` etc.
