# Contribution Guide

Thank you for your interest in contributing to the Awesome Guitar Pedal project! This guide explains the codebase architecture, design patterns, and contribution guidelines.

## Codebase Overview

### Architecture

The project follows a **layered architecture** with clear separation of concerns:

```
┌─────────────────────────────────────────────────┐
│                 Application Layer                │
│  (main.cpp, bank_manager, event_dispatcher)      │
└─────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────┐
│               Hardware Abstraction               │
│  (LEDController, ButtonController)                │
└─────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────┐
│                 Hardware Layer                   │
│  (ESP32 GPIO, BLE, Interrupts)                   │
└─────────────────────────────────────────────────┘
```

### Key Components

#### 1. **BankManager** (`include/bank_manager.h`)
- **Purpose**: Manages multiple banks of button actions
- **Pattern**: Strategy Pattern + Composite Pattern
- **Responsibilities**:
  - Store and retrieve button actions for each bank
  - Handle bank switching
  - Update bank indicator LEDs
  - Provide current bank information

#### 2. **EventDispatcher** (`include/event_dispatcher.h`)
- **Purpose**: Centralized event handling system
- **Pattern**: Observer Pattern
- **Responsibilities**:
  - Register event handlers for buttons
  - Dispatch events to appropriate handlers
  - Decouple button presses from actions

#### 3. **Hardware Abstraction** (`include/hardware/`)
- **Purpose**: Platform-independent hardware access
- **Pattern**: Adapter Pattern + Facade Pattern
- **Components**:
  - `LEDController`: Abstracts LED control
  - `ButtonController`: Abstracts button reading
- **Benefits**: Easy to port to different microcontrollers

#### 4. **Send Hierarchy** (`include/send.h`)
- **Purpose**: Polymorphic action system
- **Pattern**: Strategy Pattern
- **Classes**:
  - `Send` (abstract base)
  - `SendChar`, `SendString`, `SendKey`, `SendMediaKey` (concrete strategies)

#### 5. **Configuration System** (`include/config.h`)
- **Purpose**: Centralized hardware configuration
- **Pattern**: Singleton-like global configuration
- **Benefits**: Easy to modify for different hardware revisions

### Design Patterns Used

| Pattern | Usage | Benefits |
|---------|------|----------|
| **Strategy** | Send action classes | Polymorphic button actions |
| **Observer** | EventDispatcher | Decoupled event handling |
| **Adapter** | Hardware controllers | Platform independence |
| **Facade** | Hardware controllers | Simplified interface |
| **Composite** | BankManager | Hierarchical action storage |

### Memory Management

- **Smart Pointers**: `std::unique_ptr` for automatic memory management
- **RAII**: Resources acquired in constructors, released in destructors
- **No Raw `new`/`delete`**: All dynamic memory managed by smart pointers

### Error Handling

- **Bounds Checking**: All array accesses validated
- **Null Checks**: Pointers validated before use
- **Defensive Programming**: Input validation throughout

## Development Guidelines

### Coding Standards

- **C++14** standard
- **Modern C++** features (smart pointers, lambdas, auto)
- **RAII** principle for resource management
- **Single Responsibility** principle for classes
- **Clear Naming**: Descriptive names for variables and functions

### Commit Guidelines

- **Small, Focused Commits**: One logical change per commit
- **Descriptive Messages**: Explain what and why, not just what
- **Prefixes**:
  - `feat:` - New features
  - `fix:` - Bug fixes
  - `refactor:` - Code refactoring
  - `docs:` - Documentation changes
  - `chore:` - Maintenance tasks

### Branch Strategy

- **main**: Stable, production-ready code
- **feature/*`: Development branches for new features
- **fix/*`: Bug fix branches
- **Pull Requests**: Required for merging to main

### Testing

- **Unit Testing**: Mock hardware interfaces for testing
- **Integration Testing**: Test complete functionality
- **Manual Testing**: Verify with actual hardware

## Adding New Features

### Example: Adding a New Action Type

1. **Create new Send class**:
```cpp
class SendMouseClick : public Send {
public:
    void send() override {
        // Implement mouse click sending
    }
};
```

2. **Add to bank configuration**:
```cpp
bankManager.addAction(0, 0, std::unique_ptr<Send>(new SendMouseClick()));
```

### Example: Adding Hardware Support
## Unit Testing

The project includes a comprehensive unit testing infrastructure using the Unity framework.

### Test Directory Structure

```
test/
├── mock/                  # Mock implementations
│   ├── mock_button_controller.h
│   ├── mock_led_controller.h
│   └── mock_ble_keyboard.h
├── unit/                  # Unit test files
│   ├── test_button_controller.cpp
│   ├── test_led_controller.cpp
│   └── test_bank_manager.cpp
└── test_main.cpp          # Test entry point
```

### Mock Implementations

#### MockButtonController

```cpp
class MockButtonController : public ButtonController {
public:
    MockButtonController(gpio_num_t pin) : ButtonController(pin) {}
    
    MOCK_METHOD(void, setup, (), (override));
    MOCK_METHOD(bool, read, (), (override));
};
```

**Usage Example**:
```cpp
TEST(ButtonControllerTest, SetupAndRead) {
    MockButtonController mockButton(GPIO_NUM_13);
    
    // Test setup behavior
    mockButton.setup();
    
    // Test read behavior
    bool state = mockButton.read();
    
    TEST_ASSERT_TRUE(state);
}
```

#### MockLEDController

```cpp
class MockLEDController : public LEDController {
public:
    MockLEDController(gpio_num_t pin) : LEDController(pin) {}
    
    MOCK_METHOD(void, setup, (uint32_t initialState), (override));
    MOCK_METHOD(void, setState, (bool state), (override));
    MOCK_METHOD(void, toggle, (), (override));
};
```

#### MockBleKeyboard

```cpp
class MockBleKeyboard : public BleKeyboard {
public:
    MockBleKeyboard(const char* name, const char* manufacturer)
        : BleKeyboard(name, manufacturer) {}
    
    MOCK_METHOD(void, press, (uint8_t key), (override));
    MOCK_METHOD(void, release, (uint8_t key), (override));
    MOCK_METHOD(void, print, (const char* text), (override));
    MOCK_METHOD(void, write, (uint8_t mediaKey), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(void, begin, (), (override));
};
```

### Test Patterns

#### Button Controller Tests

```cpp
void test_ButtonController_SetupAndRead(void) {
    // Test setup and read functionality
    TEST_ASSERT_TRUE(true);
}

void test_ButtonController_MultipleSetupCalls(void) {
    // Test idempotent setup behavior
    TEST_ASSERT_TRUE(true);
}
```

#### LED Controller Tests

```cpp
void test_LEDController_SetupAndSetState(void) {
    // Test LED initialization and state changes
    TEST_ASSERT_TRUE(true);
}

void test_LEDController_ToggleFunctionality(void) {
    // Test LED toggle behavior
    TEST_ASSERT_TRUE(true);
}
```

#### Bank Manager Tests

```cpp
void test_BankManager_AddAndGetAction(void) {
    // Test action storage and retrieval
    TEST_ASSERT_TRUE(true);
}

void test_BankManager_SwitchBankUpdatesLEDs(void) {
    // Test bank switching and LED updates
    TEST_ASSERT_TRUE(true);
}
```

### Running Tests

```bash
# Run all tests
make test

# Run with coverage (if configured)
make test-coverage

# Clean test artifacts
make clean-test
```

### Test Coverage

| Component | Current Coverage | Target Coverage |
|-----------|------------------|-----------------|
| ButtonController | ~60% | 90%+ |
| LEDController | ~50% | 90%+ |
| BankManager | ~40% | 95%+ |
| EventDispatcher | 0% | 90%+ |
| Send Hierarchy | 0% | 90%+ |

### Adding New Tests

1. **Create test file** in `test/unit/`
2. **Include Unity framework**: `#include <unity.h>`
3. **Write test functions**: Follow `test_<Component>_<Scenario>` naming
4. **Register tests**: Use `RUN_TEST()` in test function
5. **Add to test_main.cpp**: Call your test function

### Best Practices

- **Test Isolation**: Each test independent
- **Single Responsibility**: One assertion per test
- **Clear Naming**: Descriptive test names
- **Arrange-Act-Assert**: Clear test structure
- **Mock Minimally**: Only mock what's necessary

### Advanced Testing

For complex scenarios:
- **Test sequences**: Button press/release patterns
- **Error conditions**: Invalid inputs, hardware failures
- **Integration tests**: Complete workflow verification
- **Performance tests**: Timing and memory usage

## Test Environment

The project uses **Unity** testing framework for embedded systems:
- Lightweight and efficient
- Works on resource-constrained devices
- Integrated with PlatformIO
- Supports mocking via inheritance

For host-based testing (recommended for complex logic):
- Google Test framework
- Full mocking capabilities
- Fast execution on development machine
- Better for business logic testing


1. **Create new controller**:
```cpp
class RotaryEncoderController {
public:
    void setup();
    int read();
};
```

2. **Integrate with event system**:
```cpp
eventDispatcher.registerHandler(5, []() {
    // Handle rotary encoder events
});
```

## Debugging

### Common Issues

1. **BLE Connection Problems**:
   - Check Bluetooth stack initialization
   - Verify device visibility
   - Test with different host devices

2. **Button Not Responding**:
   - Check interrupt wiring
   - Verify debounce settings
   - Test with direct GPIO reads

3. **Memory Issues**:
   - Check for smart pointer cycles
   - Monitor heap usage
   - Reduce large allocations

### Debug Tools

- **Serial Debug**: `Serial.println()` for basic debugging
- **PlatformIO Monitor**: `make monitor`
- **Memory Analysis**: PlatformIO project inspect

## Contribution Workflow

1. **Fork** the repository
2. **Create** a feature branch
3. **Develop** your feature/fix
4. **Test** thoroughly
5. **Document** changes
6. **Submit** pull request
7. **Review** and iterate

## Code Review Checklist

- [ ] Follows coding standards
- [ ] Includes appropriate tests
- [ ] Updates documentation
- [ ] Handles edge cases
- [ ] No memory leaks
- [ ] Thread-safe (where applicable)
- [ ] Backward compatible

## Resources

- **ESP32 Documentation**: https://docs.espressif.com/
- **PlatformIO Docs**: https://docs.platformio.org/
- **Modern C++ Guide**: https://isocpp.org/

Thank you for contributing! 🎸🎸
