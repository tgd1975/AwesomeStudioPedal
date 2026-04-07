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
