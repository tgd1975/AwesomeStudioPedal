# Makefile for AwesomeStudioPedal ESP32 project
# Uses PlatformIO as the build system

.PHONY: all build upload clean monitor test-host test-flutter test-esp32-button test-esp32-serial test-esp32-profiles test-esp32-ble-config test-nrf52840-button test-nrf52840-serial test-nrf52840-profiles build-nrf52840 build-esp32 upload-esp32 uploadfs-esp32 monitor-esp32 upload-nrf52840 uploadfs-nrf52840 monitor-nrf52840 docs docs-coverage coverage coverage-clean flutter-get flutter-analyze flutter-test flutter-build

# Flutter is intentionally not installed in the dev container.
# If 'flutter: command not found', either:
#   a) You are on the host machine and Flutter is not installed yet — see app/README.md.
#   b) You are inside the dev container — run these targets from a host terminal instead.
FLUTTER ?= flutter

flutter-get:
	@command -v $(FLUTTER) >/dev/null 2>&1 || { echo "ERROR: flutter not found on PATH."; echo "  If you are inside the dev container: run this from a host terminal (Flutter is not installed in the container by design)."; echo "  If you are on the host machine: install Flutter — see app/README.md for instructions."; exit 1; }
	cd app && $(FLUTTER) pub get

flutter-analyze: flutter-get
	cd app && $(FLUTTER) analyze

flutter-test: flutter-get
	cd app && $(FLUTTER) test

flutter-build: flutter-get
	cd app && $(FLUTTER) build apk --release

# Alias used by the test suite and pre-commit hook narrative
test-flutter: flutter-test

# Target-specific variables
ESP32_ENV ?= nodemcu-32s
NRF52840_ENV ?= feather-nrf52840

# Use separate build dirs for host and dev container to avoid CMake cache
# conflicts caused by differing absolute paths (/home/... vs /workspaces/...).
ifeq ($(shell test -f /.dockerenv && echo yes),yes)
  HOST_BUILD_DIR = .vscode/build
else
  HOST_BUILD_DIR = .vscode/build-host
endif

COVERAGE_BUILD_DIR = .vscode/build-coverage
COVERAGE_INFO      = $(COVERAGE_BUILD_DIR)/coverage.info
COVERAGE_REPORT    = docs/coverage

# Default target - show help
all:
	@echo "Awesome Guitar Pedal - Makefile Usage"
	@echo ""
	@echo "General Commands:"
	@echo "  make build          - Build ALL targets (ESP32 + nRF52840)"
	@echo "  make clean          - Clean build artifacts"
	@echo "  make test-host      - Run host unit tests (GoogleTest)"
	@echo "  make format         - Format all C++ files using clang-format"
	@echo "  make lint-markdown  - Fix markdown linting issues"
	@echo "  make info           - Show project information"
	@echo "  make docs           - Generate API documentation with Doxygen"
	@echo "  make docs-coverage  - Show undocumented symbols in project source"
	@echo "  make coverage       - Run host tests and generate HTML coverage report (requires lcov)"
	@echo "  make coverage-clean - Remove coverage build artifacts"
	@echo ""
	@echo "ESP32-Specific Commands:"
	@echo "  make build-esp32         - Build for ESP32 only"
	@echo "  make upload-esp32        - Upload firmware to ESP32"
	@echo "  make uploadfs-esp32      - Upload filesystem image (data/) to ESP32 — run once after adding/changing config files"
	@echo "  make monitor-esp32       - Monitor ESP32 serial"
	@echo "  make run-esp32           - Build, upload firmware + filesystem, and monitor ESP32"
	@echo "  make test-esp32-button   - Upload filesystem + run on-device button tests (Unity, requires ESP32)"
	@echo "  make test-esp32-serial   - Upload filesystem + run on-device serial output tests (Unity, requires ESP32)"
	@echo "  make test-esp32-profiles - Upload filesystem + run on-device profile manager tests (Unity, requires ESP32)"
	@echo ""
	@echo "nRF52840-Specific Commands:"
	@echo "  make build-nrf52840         - Build for nRF52840 only"
	@echo "  make upload-nrf52840        - Upload to nRF52840"
	@echo "  make uploadfs-nrf52840      - Upload filesystem image (data/) to nRF52840 — run once after adding/changing config files"
	@echo "  make monitor-nrf52840       - Monitor nRF52840 serial"
	@echo "  make run-nrf52840           - Build, upload firmware + filesystem, and monitor nRF52840"
	@echo "  make test-nrf52840-button   - Upload filesystem + run on-device button tests (Unity, requires nRF52840)"
	@echo "  make test-nrf52840-serial   - Upload filesystem + run on-device serial output tests (Unity, requires nRF52840)"
	@echo "  make test-nrf52840-profiles - Upload filesystem + run on-device profile manager tests (Unity, requires nRF52840)"
	@echo ""
	@echo "Flutter App Commands (requires flutter on PATH — see app/README.md):"
	@echo "  make flutter-get     - Install Flutter dependencies (pub get)"
	@echo "  make flutter-analyze - Run flutter analyze"
	@echo "  make flutter-test    - Run flutter test (alias: make test-flutter)"
	@echo "  make flutter-build   - Build release APK"
	@echo "  make test-flutter    - Alias for flutter-test (mirrors CI job name)"
	@echo ""
	@echo "See README.md for more details"

# Build the project using PlatformIO
build: build-esp32 build-nrf52840

# Upload firmware to device
upload: upload-esp32

# Clean build artifacts
clean:
	pio run --target clean

# Open serial monitor
monitor:
	pio device monitor

# ESP32 specific targets
build-esp32:
	pio run -e $(ESP32_ENV)

upload-esp32:
	pio run -e $(ESP32_ENV) --target upload

uploadfs-esp32:
	pio run -e $(ESP32_ENV) --target uploadfs

monitor-esp32:
	pio device monitor -e $(ESP32_ENV)

run-esp32: build-esp32 upload-esp32 uploadfs-esp32
	@echo "Build and upload complete. Starting monitor..."
	pio device monitor -e $(ESP32_ENV) || echo "Monitor failed, but build/upload succeeded"

# nRF52840 specific targets
build-nrf52840:
	pio run -e $(NRF52840_ENV)

upload-nrf52840:
	pio run -e $(NRF52840_ENV) --target upload

uploadfs-nrf52840:
	pio run -e $(NRF52840_ENV) --target uploadfs

monitor-nrf52840:
	pio device monitor -e $(NRF52840_ENV)

run-nrf52840: build-nrf52840 upload-nrf52840 uploadfs-nrf52840
	@echo "Build and upload complete. Starting monitor..."
	pio device monitor -e $(NRF52840_ENV) || echo "Monitor failed, but build/upload succeeded"

# Run on-device button tests (Unity via PlatformIO) — requires ESP32 connected
test-esp32-button: uploadfs-esp32
	pio test -e nodemcu-32s-test -v

# Run on-device serial output tests (Unity via PlatformIO) — requires ESP32 connected
test-esp32-serial: uploadfs-esp32
	pio test -e nodemcu-32s-serial-test -v

# Run on-device profile manager tests (Unity via PlatformIO) — requires ESP32 connected
test-esp32-profiles: uploadfs-esp32
	pio test -e nodemcu-32s-profilemanager-test -v

# Run on-device GPIO testrig for PinAction (Unity via PlatformIO) — requires ESP32 connected
test-esp32-pin-io: uploadfs-esp32
	pio test -e nodemcu-32s-pin-io-test -v

# Run BLE Config end-to-end integration tests — requires ESP32 + BLE-capable host
# Flashes the test harness firmware, then runs runner.py as BLE client.
# Usage: make test-esp32-ble-config [PORT=/dev/ttyUSB0]
PORT ?= /dev/ttyUSB0
test-esp32-ble-config: uploadfs-esp32
	pio run -e nodemcu-32s-ble-config-test --target upload
	python3 test/test_ble_config_esp32/runner.py --port $(PORT)

# Run on-device button tests (Unity via PlatformIO) — requires nRF52840 connected
test-nrf52840-button: uploadfs-nrf52840
	pio test -e feather-nrf52840-test -v

# Run on-device serial output tests (Unity via PlatformIO) — requires nRF52840 connected
test-nrf52840-serial: uploadfs-nrf52840
	pio test -e feather-nrf52840-serial-test -v

# Run on-device profile manager tests (Unity via PlatformIO) — requires nRF52840 connected
test-nrf52840-profiles: uploadfs-nrf52840
	pio test -e feather-nrf52840-profilemanager-test -v

# Run host unit tests (GoogleTest via CMake)
test-host:
	cmake --build $(HOST_BUILD_DIR) --target pedal_tests && $(HOST_BUILD_DIR)/test/pedal_tests

# Format all C++ files using clang-format
format:
	find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | grep -v \.pio | grep -v build | xargs clang-format -i
	@echo "All C++ files formatted successfully"

# Fix markdown linting issues (requires markdownlint-cli2)
lint-markdown:
	npx markdownlint-cli2 --fix "**/*.md" "#node_modules" "#.pio" "#build" "#.vscode"
	@echo "Markdown linting complete"

# Clean test artifacts
clean-test:
	rm -rf .pio/build/test

# Show project information
info:
	pio run --list-targets

# Shortcut for common operations
flash: upload

# Environment variables can be set here if needed
# export PLATFORMIO_BUILD_FLAGS = "-DDEBUG"

# Note: This Makefile assumes PlatformIO is installed and available in PATH
# Install with: pip install platformio

# Show undocumented symbols in project source files
docs-coverage:
	@echo "Undocumented symbols in project source:"
	@doxygen Doxyfile 2>&1 | \
		grep "warning:.*is not documented" | \
		grep -v "\.pio\|/usr/\|ArduinoJson\|Polyfill\|Variant\|_deps" | \
		sort || true
	@echo "Done."

# Generate API documentation with Doxygen
docs:
	doxygen Doxyfile
	@echo "Documentation generated in docs/api/"

# Code coverage report (requires lcov + genhtml)
coverage:
	cmake -B $(COVERAGE_BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_CXX_FLAGS="--coverage" \
		-DCMAKE_EXE_LINKER_FLAGS="--coverage"
	cmake --build $(COVERAGE_BUILD_DIR) --target pedal_tests
	$(COVERAGE_BUILD_DIR)/test/pedal_tests
	lcov --capture --directory $(COVERAGE_BUILD_DIR) \
		--output-file $(COVERAGE_INFO) \
		--ignore-errors inconsistent,inconsistent,mismatch
	lcov --extract $(COVERAGE_INFO) \
		'*/lib/PedalLogic/*' \
		'*/lib/hardware/*' \
		--output-file $(COVERAGE_INFO) \
		--ignore-errors unused,inconsistent,inconsistent
	genhtml $(COVERAGE_INFO) --output-directory $(COVERAGE_REPORT)
	@echo "Coverage report: $(COVERAGE_REPORT)/index.html"

coverage-clean:
	rm -rf $(COVERAGE_BUILD_DIR)

install-hooks:
	cp scripts/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
