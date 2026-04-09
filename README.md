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
