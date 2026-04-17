---
id: IDEA-014
title: Automated Hardware Testing Rig
description: A second device with servo motors that presses buttons and a camera that observes LEDs/display — enables fully automated on-device testing without a human in the loop
---

## Motivation

Host unit tests cover logic, but they cannot verify that the physical firmware behaves
correctly on real hardware. Today, on-device testing requires a human to press buttons
and visually confirm LED states. This idea eliminates that manual step entirely.

## Concept 1 — Servo Button Presser

A small companion device that mounts above the pedal and drives two servo motors, each
positioned over a button. A test runner sends commands over USB/UART to trigger
specific button presses and release sequences.

### Key aspects

- **Hardware**: two micro-servos (e.g. SG90) on a 3D-printed bracket that aligns with
  the pedal's button layout
- **Controller**: a second MCU (e.g. ATtiny or small Arduino) that receives press
  commands and actuates the servos
- **Protocol**: simple UART command set — `PRESS_A`, `PRESS_B`, `HOLD_A 500ms`, `RELEASE_ALL`
- **Mounting**: bracket designed to clip onto or sit beside the pedal enclosure without
  permanent modification

### Benefit

Automated test scripts can now drive the physical hardware the same way a human would,
making regression tests possible after every firmware flash.

## Concept 2 — Camera LED/Display Observer (feedback loop)

A camera pointed at the pedal's LEDs and optional OLED display closes the feedback
loop: the test runner can assert that the correct LEDs lit up after a button press.

### Key aspects

- **Hardware**: a small USB camera or ESP32-CAM module positioned to see all LEDs and
  the display
- **Vision**: image recognition or simple colour thresholding to detect LED on/off
  state; OCR or template matching for display content
- **Integration**: the test runner queries the observer after each action and compares
  the observed state against the expected state
- **Tolerance**: handle ambient light variation through calibration or difference
  imaging (before/after a press)

### Benefit

Full round-trip verification — input (servo press) and output (LED/display state) —
without any human involvement.

## Combined Test Flow

```
Test script
  │
  ├─► UART → Servo controller → physical button press
  │
  └─► Camera observer → LED/display state
        │
        └─► Assert expected state → pass / fail
```

## Risks and Considerations

- **Mechanical alignment**: bracket must be reprinted if the pedal enclosure changes
- **Lighting**: ambient light can affect LED detection — a hood or controlled lighting
  may be needed
- **Latency**: firmware debounce and LED update timing must be accounted for in the
  test script (small delay between press and observation)
- **Scope**: this is a developer tool, not a product feature — keep it simple and
  low-cost
- **ESP32-CAM option**: combining camera and servo control on a single ESP32-CAM board
  could reduce the hardware footprint to one device
