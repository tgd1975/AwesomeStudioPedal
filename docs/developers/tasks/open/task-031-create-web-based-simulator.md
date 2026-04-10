---
id: TASK-031
title: Create web-based simulator
status: open
opened: 2026-04-10
effort: Extra Large (24-40h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Create a browser-based pedal simulator hosted on GitHub Pages, allowing users to test button
mappings and preview BLE keyboard output without physical hardware.

## Acceptance Criteria

- [ ] Simulator hosted at GitHub Pages URL (already configured)
- [ ] Visual representation of pedal buttons
- [ ] Load a `pedal_config.json` file to populate button mappings
- [ ] Clicking a button shows the simulated BLE keyboard output
- [ ] Simulated serial output display
- [ ] Configuration can be saved/exported
- [ ] Linked from `README.md`

## Notes

Suggested file structure under `docs/simulator/`:

```
index.html          # Main simulator page
simulator.js        # Button logic and config parsing
styles.css          # Styling
config-example.json # Example configuration
```

Key JavaScript functions to implement:

```javascript
function loadConfig(jsonData)       // parse pedal_config.json, update button mappings
function simulateButtonPress(id)    // show BLE key + serial output for that button
function validateConfig()           // check for common errors before loading
```

Related: TASK-032 (config builder tool) — the simulator's "Export Config" could integrate with it.
