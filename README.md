# AwesomeStudioPedal

A programmable, multi-profile foot controller for DAWs, score readers, and studio automation.

AwesomeStudioPedal is an ESP32-based device that connects to any host over Bluetooth and appears as
a keyboard. Press a button and it sends a keypress, media command, or typed string — no driver, no
app, no cable required. Seven profiles are stored on the device; a SELECT button cycles through them
with an LED indicator array. A time-delayed action lets solo performers trigger a command (such as a
camera shutter) and step into position before it fires.

| I am a... | Start here |
|-----------|------------|
| Musician — I have the pedal in front of me | [User Guide](docs/musicians/USER_GUIDE.md) |
| Builder — I want to build one | [Build Guide](docs/builders/BUILD_GUIDE.md) |
| Developer — I want to contribute | [Architecture](docs/developers/ARCHITECTURE.md) |

ESP32 (NodeMCU-32S) is the only deployed and tested hardware target. nRF52840 is implemented but
untested — use at your own risk.

## License

MIT License — see LICENSE file for details.

## Status

[![CI](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/test.yml/badge.svg)](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/test.yml)
[![CodeQL](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/codeql-analysis.yml)
[![Static Analysis](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/static-analysis.yml)
[![Test Coverage](https://img.shields.io/badge/Coverage-80%25-brightgreen)](https://github.com/tgd1975/AwesomeGuitarPedal/actions/workflows/test.yml)

## Future Ideas

The following ideas are potential future enhancements for AwesomeStudioPedal. These are not committed features but represent directions the project could explore:

- **Mobile App Configuration**: Develop Android and/or iOS apps to configure profiles and settings via Bluetooth.
- **CLI Tools**: Create command-line tools for advanced configuration and automation.
- **Additional Hardware Support**: Extend compatibility to platforms like Arduino Nano.
- **nRF Hardware Testing**: Thoroughly test and validate the nRF52840 implementation.
- **Large Button Pedal Prototype**: Design and build a prototype with larger, more accessible buttons.
- **Macros**: Implement support for macros, allowing sequences of actions to be triggered with a single button press.
- **Display Integration**:
  - **Version A**: Add a small display to show the currently selected profile.
  - **Version B**: Incorporate a larger display to show more detailed configuration information.
- **Hybrid Tool with DSP**: Explore a version with more powerful hardware (DSP) and two audio jacks to add guitar pedal functionalities like distortion, creating a hybrid tool.
- **Long Press Event**: Implement support for long press events to trigger different actions based on button hold duration.
- **Double Press Event**: Add support for double press events to enable quick successive button presses for additional functionality.

These ideas are open for community contributions and discussions. If you're interested in working on any of these, please open an issue or start a discussion!

## Firmware

No pre-built firmware release is available yet. Build from source using the
[Build Guide](docs/builders/BUILD_GUIDE.md).
