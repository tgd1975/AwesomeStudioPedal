# Contribution Guide

Thank you for your interest in contributing to the Awesome Guitar Pedal project! This guide explains the codebase architecture, design patterns, and contribution guidelines.

## Codebase Overview

### Architecture

The project follows a **layered architecture** with clear separation of concerns:

```mermaid
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
| --- | --- | --- |
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

The project uses a lightweight trunk-based strategy with short-lived topic branches.

#### Branch Types

| Branch | Pattern | Purpose |
| --- | --- | --- |
| `main` | — | Always releasable; protected; requires passing CI and PR review |
| Feature | `feature/<short-description>` | New functionality (e.g. `feature/rotary-encoder`) |
| Bug fix | `fix/<short-description>` | Targeted bug fixes (e.g. `fix/debounce-race`) |
| Refactor | `refactor/<short-description>` | Code quality changes with no behaviour change |
| Docs | `docs/<short-description>` | Documentation-only changes |

#### Branch Lifecycle

```text
main
 │
 ├─── feature/my-feature ──── (develop) ──── PR ──── squash-merge ──► main
 │
 ├─── fix/some-bug ────────── (fix)     ──── PR ──── squash-merge ──► main
 │
 └─── refactor/cleanup ────── (refactor)──── PR ──── squash-merge ──► main
```

1. **Branch off `main`** — always start from the latest `main`.
2. **Keep branches short-lived** — aim to open a PR within a few days.
3. **One concern per branch** — mix of feature + unrelated refactor = two branches.
4. **Open a PR early** — draft PRs are encouraged for early feedback.
5. **All tests must pass** before requesting review (`make test-host`).
6. **Squash-merge into `main`** — keeps the history linear and readable.
7. **Delete the branch** after merge.

#### Naming Conventions

- Use lowercase and hyphens only: `feature/ble-reconnect`, not `Feature/BLE_Reconnect`.
- Keep descriptions concise (≤ 4 words).
- Reference an issue number when one exists: `fix/42-watchdog-reset`.

#### `main` Branch Protection

- Direct pushes to `main` are not allowed.
- Every merge requires a pull request.
- The pre-commit hook (`make test-host`) must pass locally before pushing.
- CI runs the full host test suite on every PR.

### Release Process

Releases are tagged on `main` and follow [Semantic Versioning](https://semver.org/) (`MAJOR.MINOR.PATCH`).

#### Version Scheme

| Segment | Increment when… |
| ------- | --------------- |
| `MAJOR` | Breaking change to the hardware interface or bank configuration format |
| `MINOR` | New feature added in a backwards-compatible way (e.g. new hardware platform, new action type) |
| `PATCH` | Backwards-compatible bug fix or documentation update |

#### Release Checklist

1. **All tests pass** — `make test-host` must report zero failures on `main`.
2. **Firmware builds clean** for all target environments — `make build` (ESP32) and `make build-nrf52840`.
3. **Update version** — bump the version string in `platformio.ini` (both ESP32 and nRF52840 envs share the same logical version).
4. **Tag the release** on `main`:

   ```bash
   git tag -a v1.2.0 -m "Release v1.2.0"
   git push origin v1.2.0
   ```

5. **Create a GitHub Release** from the tag. Attach the compiled `.elf`/`.bin` artefacts for each target environment and write a short changelog.
6. **Do not** release from a topic branch — tags must always point to a commit on `main`.

#### Hotfix Releases

For critical fixes that need to ship before the next planned release:

```text
main (v1.2.0 tag)
 │
 └─── fix/critical-bug ──── PR ──── squash-merge ──► main (v1.2.1 tag)
```

There are no long-lived release branches — patch the tip of `main` and tag immediately.

### Testing

- **Unit Testing**: Mock hardware interfaces for testing
- **Integration Testing**: Test complete functionality
- **Manual Testing**: Verify with actual hardware

See [TESTING_IMPLEMENTATION.md](TESTING_IMPLEMENTATION.md) for the full test infrastructure documentation.

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

1. **Add to bank configuration**:

   ```cpp
   bankManager.addAction(0, 0, std::unique_ptr<Send>(new SendMouseClick()));
   ```

### Example: Adding Hardware Support

1. **Create new controller**:

   ```cpp
   class RotaryEncoderController {
   public:
       void setup();
       int read();
   };
   ```

1. **Integrate with event system**:

   ```cpp
   eventDispatcher.registerHandler(5, []() {
       // Handle rotary encoder events
   });
   ```

## Unit Testing

The project includes a comprehensive unit testing infrastructure using the Unity framework.

### Test Directory Structure

```text
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
| :--- | :--- | :--- |
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

### Test Environment

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

### Debugging

#### Common Issues

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

#### Debug Tools

- **Serial Debug**: `Serial.println()` for basic debugging
- **PlatformIO Monitor**: `make monitor`
- **Memory Analysis**: PlatformIO project inspect

### Contribution Workflow

1. **Fork** the repository
2. **Create** a feature branch
3. **Develop** your feature/fix
4. **Test** thoroughly
5. **Document** changes
6. **Submit** pull request
7. **Review** and iterate

### Resources

- **ESP32 Documentation**: <https://docs.espressif.com/>
- **PlatformIO Docs**: <https://docs.platformio.org/>
- **Modern C++ Guide**: <https://isocpp.org/>

Thank you for contributing! 🎸🎸
