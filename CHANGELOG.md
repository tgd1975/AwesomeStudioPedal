# Changelog

All notable changes to this project will be documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added

- nRF52840 hardware package (`lib/hardware/nrf52840`) with `Button`, `LEDController`, `ButtonController`, and `BleKeyboardAdapter` (wrapping `BLEHidAdafruit`)
- `feather-nrf52840` and `feather-nrf52840-test` PlatformIO environments
- `build-nrf52840` Makefile target
- Target-specific Makefile commands: `build-esp32`, `upload-esp32`, `monitor-esp32`, `upload-nrf52840`, `monitor-nrf52840`
- Enhanced `make build` now builds ALL targets (ESP32 + nRF52840)
- Helpful `make` command shows usage information
- `scripts/serial_monitor.py` - Python-based alternative serial monitor
- Comprehensive Linux development setup guide in `CONTRIBUTION_GUIDELINE.md`
- Serial port permissions and troubleshooting documentation
- `include/platform.h` — no-op `IRAM_ATTR` shim so non-ESP32 targets compile cleanly
- `IButton` interface in `lib/PedalLogic` — `Button` now inherits from it, enabling platform-agnostic button handling
- `createBleKeyboardAdapter()` factory function per hardware package — `main.cpp` is now hardware-agnostic
- Host unit test infrastructure (GoogleTest + CMake) — all tests run without hardware via `make test-host`
- `ILEDController`, `IButtonController`, `IBleKeyboard` interfaces enabling mock-based unit tests
- `BankManager`, `EventDispatcher`, `Send`, and `Button` unit tests (22 tests total)
- Pre-commit hook running markdownlint and the full host test suite
- `lib/PedalLogic` — hardware-independent logic extracted into its own library
- `lib/hardware/esp32` — ESP32-specific drivers extracted into a dedicated package
- `pedal_config.cpp` — bank/button mappings moved out of `main.cpp` into `PedalLogic`
- `TESTING_IMPLEMENTATION.md` — full test infrastructure documentation
- Release process and branching concept documented in `CONTRIBUTION_GUIDELINE.md`

### Changed

- `HardwareConfig` fields changed from `gpio_num_t` to `uint8_t` — header now compiles on any Arduino target without ESP-IDF
- `config.cpp` moved from `src/` to `lib/hardware/esp32/src/` — each hardware package owns its own pin assignments
- `LEDController` and `ButtonController` constructors accept `uint8_t` pin (cast to `gpio_num_t` internally where required)
- `platformio.ini` uses `lib_ignore` per environment to exclude the other platform's package
- `main.cpp` uses `Button::setup()` / `reset()` instead of inline GPIO calls
- `BankManager::updateLEDs()` made private; called from constructor for correct initial LED state
- `BankManager` magic numbers replaced with `NUM_BANKS` / `NUM_BUTTONS` named constants
- `BankManager::switchBank()` returns `uint8_t` (current bank index)
- Key constants in `i_ble_keyboard.h` guarded with `#ifndef ESP32_BLE_KEYBOARD_H` to prevent redefinition when both headers are included
- `KEY_MEDIA_STOP` changed from `inline constexpr` to `static constexpr` for C++14 compatibility
- Branching strategy and contribution workflow documented in `CONTRIBUTION_GUIDELINE.md`
- `make build` behavior changed to build ALL targets instead of just ESP32
- Makefile help system enhanced with comprehensive usage information
- Documentation updated to reflect new Makefile command structure
- Updated GitHub Actions workflow to force Node.js 24 for all actions to resolve deprecation warnings
- Updated `markdownlint-cli2-action` to v22 for native Node.js 24 support

### Fixed

- Firmware link failure caused by PlatformIO treating `include/` and `src/` as separate flat libraries when `lib_extra_dirs` pointed inside a package directory
- Trailing comma in `lib/hardware/esp32/library.json` that caused PlatformIO to ignore the entire package
- ISR safety: removed `Serial.printf` from ISR context; `millis()` captured once per debounce check
- `volatile bool pressed` added to `button.h` to prevent optimisation-related ISR bugs
- Dead code removed from `main.cpp` (`SHIFT` macro, duplicate `BleKeyboard` include, unused `ButtonController` instances)
- `Send::bleKeyboard` moved to `protected` for correct encapsulation
- `HOST_TEST_BUILD` guards removed from `PedalLogic` — Arduino dependency was leaking into hardware-independent code
- ODR violation on `KEY_MEDIA_STOP` resolved
- Removed non-functional `make test` and `make test-coverage` targets from Makefile
- Fixed Makefile to properly handle multi-target builds

---

## [0.2.0] — 2025-11-01

### Added

- Hardware abstraction layer: `LEDController`, `ButtonController`
- `BankManager` — encapsulated three-bank switching with LED feedback
- `EventDispatcher` — centralised, decoupled event handling for button presses
- `Send` class hierarchy: `SendChar`, `SendString`, `SendKey`, `SendMediaKey`
- Configuration system (`config.h` / `config.cpp`) for centralised pin assignments
- Smart pointer usage throughout — no raw `new`/`delete`
- `Makefile` with `build`, `upload`, `monitor`, `test`, and `clean` targets

### Fixed

- `detachInterrupts` bug in tri-bank switching
- Memory leaks replaced with `std::unique_ptr`

---

## [0.1.0] — 2025-09-01

### Added

- Initial ESP32 BLE keyboard pedal implementation
- Tri-bank button mapping (buttons A–D × 3 banks)
- BLE keyboard output via `ESP32 BLE Keyboard` library
- Interrupt-driven button handling with software debounce
- Basic LED indicators for bank selection and Bluetooth status
