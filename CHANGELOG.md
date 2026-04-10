# Changelog

All notable changes to this project will be documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

---

## [0.2.0] — 2026-04-10

### Added

**Firmware**

- JSON-based configuration system: `ConfigLoader` reads/writes `data/pedal_config.json` from LittleFS; button mappings are now external and editable without recompiling
- `Profile` class — dedicated abstraction for per-profile action storage and naming
- Action hierarchy: `SerialOutputAction`, `NonSendAction` base class, `DelayedAction` wrapper; each in its own source file
- LittleFS filesystem support with custom partition table giving 1.5 MB storage
- `data/pedal_config.json` — default on-device configuration deployed via `pio run --target uploadfs`
- ArduinoJson library dependency for JSON parsing and serialisation
- Profile description support — profiles carry a human-readable name field
- `ProfileManager::resetToFirstProfile()` — wired into config reload path
- 7-profile system with binary LED encoding across 3 select LEDs
- `DelayedAction` — lets solo performers trigger a command and step into position before it fires
- `EventDispatcher` — centralised, decoupled event handling for button presses
- `Send` class hierarchy: `SendChar`, `SendString`, `SendKey`, `SendMediaKey`
- `pedal_config.cpp` — button mappings moved out of `main.cpp` into `PedalLogic`
- Smart pointer usage throughout — no raw `new`/`delete`

**Hardware support**

- nRF52840 hardware package (`lib/hardware/nrf52840`) with `Button`, `LEDController`, `ButtonController`, and `BleKeyboardAdapter`
- `feather-nrf52840` and `feather-nrf52840-test` PlatformIO environments
- `IButton` interface — `Button` now inherits from it, enabling platform-agnostic handling
- `createBleKeyboardAdapter()` factory per hardware package — `main.cpp` is hardware-agnostic
- `ILEDController`, `IButtonController`, `IBleKeyboard` interfaces

**Testing**

- Host unit test infrastructure (GoogleTest + CMake) — all tests run without hardware via `make test-host`
- `ProfileManager`, `EventDispatcher`, `Send`, and `Button` unit tests (125 tests total)
- On-device Unity test suite for button logic with interactive prompts and countdown output
- Pre-commit hook running markdownlint and the full host test suite

**Build & tooling**

- `lib/PedalLogic` — hardware-independent logic in its own library
- `lib/hardware/esp32` — ESP32-specific drivers in a dedicated package
- `scripts/serial_monitor.py` — Python-based alternative serial monitor
- `scripts/check_code_smells.py` — detects long functions, deep nesting, and other C++ anti-patterns
- `make format` — runs `clang-format` across the codebase
- `make lint-markdown` — runs `markdownlint-cli2` on all Markdown files
- `make test-host` — builds and runs the full host test suite
- Target-specific Makefile commands: `build-esp32`, `upload-esp32`, `monitor-esp32`, `build-nrf52840`, `upload-nrf52840`, `monitor-nrf52840`, `run-*`
- `make docs` and `make docs-coverage` — Doxygen API docs and LCOV coverage report generation
- Custom ESP32 partition table (`config/esp32/partitions.csv`) giving 1.5 MB LittleFS storage

**CI / repository**

- GitHub Actions: CodeQL analysis, static analysis (clang-tidy), Mermaid diagram validation, Doxygen docs pipeline
- Dependabot configuration with auto-merge workflow
- Branch protection concept and CODEOWNERS
- Issue and PR templates
- `SECURITY.md` — vulnerability reporting process via GitHub private advisories
- `FUNDING.yml` — BuyMeACoffee and Liberapay links
- `.clang-tidy` configuration enforcing naming, modernisation, and readability checks
- Compiler warning flags treated as errors (`-Wall -Wextra -Werror`)
- Mermaid diagram style guide and validation script
- `docs/developers/tasks/` — structured task management system with OVERVIEW.md

**Documentation**

- `docs/musicians/USER_GUIDE.md` — end-user guide
- `docs/builders/BUILD_GUIDE.md` — hardware build guide
- `docs/developers/ARCHITECTURE.md` — system architecture
- `CONFIG_SYSTEM.md`, `CONFIGURATION.md`, `DATA_UPLOAD.md`, `DEPLOYMENT.md`, `PARTITIONS.md` — configuration and deployment guides
- `TESTING_IMPLEMENTATION.md` — full test infrastructure documentation
- `MERMAID_STYLE_GUIDE.md` — diagram style conventions
- `RELEASE_CHECKLIST.md` — pre-release checklist
- `BRANCH_PROTECTION_CONCEPT.md` — branching strategy

### Changed

- Project renamed from AwesomeGuitarPedal to **AwesomeStudioPedal**
- License changed from GPL-3 to **MIT**
- `configureProfiles()` now delegates to `ConfigLoader` instead of hardcoding actions inline
- `HardwareConfig` fields changed from `gpio_num_t` to `uint8_t` — compiles on any Arduino target
- `ProfileManager::updateLEDs()` made private
- `ProfileManager` magic numbers replaced with named constants
- `ProfileManager::switchProfile()` returns `uint8_t` (current profile index)
- `make build` now builds ALL targets (ESP32 + nRF52840)

---

## [0.1.0] — 2025-09-01

### Added

- Initial ESP32 BLE keyboard pedal implementation
- Tri-bank button mapping (buttons A–D × 3 banks)
- BLE keyboard output via `ESP32 BLE Keyboard` library
- Interrupt-driven button handling with software debounce
- Basic LED indicators for bank selection and Bluetooth status
