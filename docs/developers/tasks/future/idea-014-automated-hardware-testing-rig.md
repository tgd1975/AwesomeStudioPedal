---
id: IDEA-014
title: Automated Hardware Testing Rig
description: A relay-based test interface with optocoupler output detection that enables fully automated on-device testing without human intervention
---

## Motivation

Host unit tests cover logic, but they cannot verify that the physical firmware behaves
correctly on real hardware. Today, on-device testing requires a human to press buttons
and visually confirm LED states. This idea eliminates that manual step entirely.

## Concept 1 — Relay-Based Input Simulation

A companion device that uses relays to simulate button presses, providing galvanic isolation
between the test controller and the device under test (DUT). This approach is more robust
and repeatable than servo-based mechanical pressing.

### Key aspects

- **Hardware**: 5 relays (for A, B, Select buttons + 2 spare) controlled by a relay driver IC
- **Controller**: Test MCU (Arduino/ESP32) that sends commands and reads responses
- **Isolation**: Relays provide complete galvanic isolation ("Potentialtrennung")
- **Protocol**: UART-based command set for relay control and output state reading
- **Design**: Modular PCB that connects to DUT via standard connectors

### Benefit

- More reliable than mechanical servos (no alignment issues)
- Built-in debouncing via relay mechanics
- Full electrical isolation protects both test equipment and DUT
- Scalable to 5 inputs while maintaining compact design

## Concept 2 — Optocoupler-Based Output Detection

Optocouplers connected in parallel with each LED provide electrical isolation while
allowing the test controller to directly monitor output states without visual observation.

### Key aspects

- **Hardware**: 5 optocouplers (PC817 or similar) - one for each LED circuit
- **Connection**: Optocoupler LEDs connected in parallel with main LEDs
- **Isolation**: 5000V optical isolation between DUT and test controller
- **Detection**: Optocoupler transistors connect to test MCU digital inputs
- **Design**: Compact circuit that shares LED current limiting resistors

### Benefit

- Direct electrical detection (no camera/lighting issues)
- Full galvanic isolation maintains safety
- Fast response time for accurate timing tests
- Works with both active-high and active-low LED circuits
- Simple to implement and calibrate

## Combined Test Flow

```
Test script → UART → Test MCU
  │                 │
  ├─► Relay control → DUT inputs
  │                 │
  └─◄ Optocoupler read ← DUT outputs (LEDs)
        │
        └─► Assert expected state → pass / fail
```

## Simplified Architecture

```
[Test PC] ←USB→ [Test MCU] ←→ [Relay/Optocoupler Board] ←→ [DUT: ESP32 Pedal]
                     │                              │
                     │                              │
                     └─► Relay control (5 channels)  │
                     │                              │
                     └─◄ Optocoupler read (5 channels)
```

## Implementation Plan

### Hardware Implementation

**Relay Board:**
- 5x 5V relays (e.g., Songle SRD-05VDC-SL-C) with ULN2803 driver
- 2.54mm pitch screw terminals for DUT connections
- 5V power input (separate from test MCU power)
- Status LEDs for relay activation

**Optocoupler Circuit:**
- 5x PC817 optocouplers
- 1kΩ pull-up resistors on transistor side
- Direct connection to LED circuits (parallel)
- 5V power from test MCU

**Test Controller:**
- Arduino Nano or ESP32 (for sufficient I/O)
- USB-UART interface to test PC
- 5x digital outputs for relay control
- 5x digital inputs for optocoupler readings

### Test Suite Integration

**Python Test API:**
```python
class HardwareTestRig:
    def __init__(self, port='/dev/ttyUSB0'):
        self.serial = Serial(port, 115200)
    
    def press_button(self, button_name, duration_ms=50):
        """Press and hold button for specified duration"""
        self._send_command(f'PRESS {button_name} {duration_ms}')
        time.sleep((duration_ms + 20) / 1000)  # Account for relay + debounce
    
    def release_all(self):
        """Release all buttons"""
        self._send_command('RELEASE_ALL')
        time.sleep(0.015)  # Relay release time
    
    def get_led_states(self):
        """Return dictionary of LED states: {'led1': True, 'led2': False, ...}"""
        self._send_command('READ_LEDS')
        response = self._read_response()
        return self._parse_led_response(response)
    
    def assert_led_states(self, expected_states, timeout_ms=100):
        """Wait for expected LED states or timeout"""
        start = time.time()
        while time.time() - start < timeout_ms/1000:
            current = self.get_led_states()
            if current == expected_states:
                return True
            time.sleep(0.01)
        raise AssertionError(f"LED states mismatch. Expected: {expected_states}, Got: {current}")
```

**Test Protocol (UART):**
- 115200 baud, 8N1
- Command format: `[cmd][space][args]\n`
- Commands:
  - `PRESS A 100` - Press button A for 100ms
  - `RELEASE_ALL` - Release all buttons
  - `READ_LEDS` - Read all LED states
  - `READ_LED 2` - Read specific LED
  - `DELAY 50` - Delay 50ms

**Response Format:**
- `LEDS:10101\n` - Binary string for 5 LEDs (1=on, 0=off)
- `LED:2:1\n` - Specific LED state
- `OK\n` - Command acknowledged
- `ERROR:message\n` - Error response

### Example Test Case

```python
def test_button_a_toggles_led_1():
    rig = HardwareTestRig()
    
    # Initial state: all LEDs off
    rig.assert_led_states({'led1': False, 'led2': False, 'led3': False, 
                          'led4': False, 'led5': False})
    
    # Press button A
    rig.press_button('A', duration_ms=50)
    
    # LED 1 should turn on
    rig.assert_led_states({'led1': True, 'led2': False, 'led3': False, 
                          'led4': False, 'led5': False}, timeout_ms=200)
    
    # Press button A again
    rig.press_button('A', duration_ms=50)
    
    # LED 1 should turn off
    rig.assert_led_states({'led1': False, 'led2': False, 'led3': False, 
                          'led4': False, 'led5': False}, timeout_ms=200)
```

### Test Runner Integration

**PyTest Fixture:**
```python
@pytest.fixture(scope="module")
def hardware_rig():
    rig = HardwareTestRig()
    yield rig
    rig.release_all()
    rig.serial.close()
```

**CI/CD Integration:**
```yaml
# .github/workflows/hardware-tests.yml
jobs:
  hardware-test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.10'
      - name: Install dependencies
        run: pip install pytest pyserial
      - name: Run hardware tests
        run: pytest tests/hardware/ --tb=short
        env:
          HARDWARE_RIG_PORT: /dev/ttyUSB0
```

### Timing Considerations

**Minimum Delays:**
- Relay activation: 10ms
- Relay deactivation: 15ms
- Optocoupler response: <1ms
- Firmware debounce: 20ms (configurable)
- LED update: <5ms

**Recommended Test Delays:**
- Button press: duration + 30ms
- State assertion: 50-200ms timeout
- Sequence steps: 10ms between actions

## Project Structure Recommendations

### Repository Structure

```
hardware-test-rig/
├── firmware/                  # Test MCU firmware
│   ├── test_controller/       # Main test controller code
│   │   ├── main.cpp           # Arduino/ESP32 main file
│   │   ├── protocol.cpp       # UART protocol handling
│   │   ├── relay_control.cpp  # Relay driver logic
│   │   ├── led_monitor.cpp    # Optocoupler reading
│   │   └── config.h           # Pin definitions, timing
│   └── test_controller.ino    # Arduino IDE project file
│
├── hardware/                  # Hardware design files
│   ├── schematic/             # Circuit diagrams
│   │   ├── relay-board.pdf     # Relay board schematic
│   │   ├── optocoupler-circuit.pdf
│   │   └── test-controller-connections.pdf
│   ├── pcb/                   # PCB design (KiCad)
│   │   ├── relay-board/        # Relay board PCB
│   │   └── breakout-board/     # DUT connection board
│   └── bom/                   # Bill of materials
│       ├── relay-board.csv    # Relay board components
│       └── full-system.csv     # Complete system BOM
│
├── software/                  # Test software
│   ├── python/                # Python test library
│   │   ├── hardware_test_rig/  # Main package
│   │   │   ├── __init__.py     # Package init
│   │   │   ├── rig.py         # HardwareTestRig class
│   │   │   ├── protocol.py     # Protocol implementation
│   │   │   └── utils.py        # Helper functions
│   │   ├── setup.py           # Package setup
│   │   ├── requirements.txt   # Dependencies
│   │   └── tests/             # Unit tests for test library
│   └── examples/              # Example test scripts
│       ├── basic_test.py      # Basic functionality test
│       ├── sequence_test.py   # Button sequence test
│       └── stress_test.py     # Long-running reliability test
│
├── tests/                     # Hardware test cases
│   ├── test_button_functions.py
│   ├── test_led_patterns.py
│   ├── test_timing.py
│   └── test_edge_cases.py
│
├── docs/                      # Documentation
│   ├── assembly-guide.md      # Hardware assembly instructions
│   ├── user-guide.md           # Software usage guide
│   ├── protocol-spec.md        # Detailed protocol specification
│   └── troubleshooting.md      # Common issues and solutions
│
├── scripts/                   # Utility scripts
│   ├── flash_firmware.sh      # Flash test controller
│   ├── run_tests.sh           # Run test suite
│   └── calibrate.py           # Calibration utility
│
├── .github/                   # GitHub configuration
│   └── workflows/             # CI/CD workflows
│       └── hardware-tests.yml # Hardware test workflow
│
├── README.md                  # Project overview
├── LICENSE                    # License file
└── requirements.txt            # Top-level dependencies
```

### Key Design Principles

**1. Separation of Concerns:**
- Hardware design separate from firmware
- Test library separate from test cases
- Protocol implementation separate from test logic

**2. Modularity:**
- Relay board as separate module (can be upgraded)
- Optocoupler circuit as separate module
- Test controller firmware independent of DUT firmware

**3. Testability:**
- Unit tests for Python library (no hardware needed)
- Integration tests for full system
- Mock interfaces for development without hardware

**4. Documentation:**
- Complete protocol specification
- Assembly and usage guides
- Troubleshooting guide with common issues

**5. CI/CD Integration:**
- Automated hardware tests in CI pipeline
- Separate workflow from regular unit tests
- Environment variables for configuration

### Development Workflow

**1. Hardware Development:**
```bash
# Design schematic and PCB
kiCad hardware/schematic/relay-board.kicad_sch
kiCad hardware/pcb/relay-board.kicad_pcb

# Generate manufacturing files
kiCad hardware/pcb/relay-board.kicad_pcb --plot
```

**2. Firmware Development:**
```bash
# Develop test controller firmware
cd firmware/test_controller
# Edit files, compile with Arduino IDE or PlatformIO

# Flash to test controller
./scripts/flash_firmware.sh /dev/ttyUSB0
```

**3. Software Development:**
```bash
# Develop Python library
cd software/python
python -m pytest tests/  # Run unit tests

# Install package
pip install -e .
```

**4. Test Development:**
```bash
# Write hardware test cases
cd tests
python -m pytest test_button_functions.py -v

# Run full test suite
./scripts/run_tests.sh
```

**5. CI/CD Pipeline:**
```yaml
# Hardware tests run automatically on push
name: Hardware Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.10'
      - name: Install dependencies
        run: pip install -r requirements.txt
      - name: Run hardware tests
        run: python -m pytest tests/ -v
        env:
          HARDWARE_RIG_PORT: ${{ secrets.HARDWARE_RIG_PORT }}
```

## Risks and Considerations

- **Relay bounce**: Mechanical relays have ~5-10ms bounce time (account for in test timing)
- **Current draw**: 5 relays may require separate power supply for test MCU
- **LED circuit compatibility**: Optocouplers must match LED circuit polarity and voltage
- **Latency**: Relay operation (~10ms) + LED update timing must be accounted for
- **Scope**: This is a developer tool — prioritize reliability over cost
- **Future expansion**: Design with spare channels for additional I/O testing
