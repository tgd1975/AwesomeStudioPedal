# Awesome Guitar Pedal - ESP32 BLE Foot Controller

A versatile foot pedal controller for musicians with Bluetooth LE connectivity, designed for controlling DAWs, plugins, and other music software.

## Features

- **Triple Bank System**: Three configurable banks for different button mappings
- **BLE Keyboard Emulation**: Works with any device supporting Bluetooth keyboards
- **Hardware Abstraction**: Portable architecture for different microcontrollers
- **Memory Safe**: Modern C++ with smart pointers
- **Extensible Design**: Easy to add new features and button actions

## Hardware Requirements

- ESP32 development board (tested with NodeMCU-32S)
- 5 buttons (4 action buttons + 1 bank select button)
- 4 LEDs (1 power, 1 Bluetooth status, 2 bank indicators)
- Bluetooth LE capable host device

## Software Requirements

- PlatformIO with ESP32 support
- Arduino framework
- ESP32 BLE Keyboard library

## Installation

```bash
# Clone the repository
git clone https://github.com/your-repo/awesome-guitar-pedal.git
cd awesome-guitar-pedal

# Install dependencies
pio lib install

# Build and upload
make upload
```

## Usage

1. Power on the pedal - power LED will light up
2. Connect via Bluetooth to "Strix-Pedal"
3. Use buttons A-D for configured actions
4. Press SELECT button to cycle through banks (3 banks total)

## Configuration

Edit `include/config.h` to modify pin assignments for your hardware.

## Project Structure

```
include/
├── bank_manager.h       # Bank switching logic
├── config.h            # Hardware configuration
├── event_dispatcher.h  # Event handling system
├── hardware/           # Hardware abstraction layers
├── send.h              # Action classes
└── button.h            # Button handling

src/
├── bank_manager.cpp
├── config.cpp
├── event_dispatcher.cpp
├── hardware/
├── send.cpp
├── button.cpp
└── main.cpp            # Main application
```

## Building

```bash
make build        # Build project
make upload      # Upload to device
make clean       # Clean build artifacts
make monitor     # Open serial monitor
```

## License

MIT License - See LICENSE file for details.
