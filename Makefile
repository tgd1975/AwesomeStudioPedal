# Makefile for AwesomeGuitarPedal ESP32 project
# Uses PlatformIO as the build system

.PHONY: all build upload clean monitor test-host test-esp32-button test-esp32-serial test-esp32-profilemanager test-nrf52840-button test-nrf52840-serial test-nrf52840-profilemanager build-nrf52840 build-esp32 upload-esp32 monitor-esp32 upload-nrf52840 monitor-nrf52840 docs

# Target-specific variables
ESP32_ENV ?= nodemcu-32s
NRF52840_ENV ?= feather-nrf52840

# Default target - show help
all:
	@echo "Awesome Guitar Pedal - Makefile Usage"
	@echo ""
	@echo "General Commands:"
	@echo "  make build        - Build ALL targets (ESP32 + nRF52840)"
	@echo "  make clean       - Clean build artifacts"
	@echo "  make test-host   - Run host unit tests (GoogleTest)"
	@echo "  make format      - Format all C++ files using clang-format"
	@echo "  make lint-markdown - Fix markdown linting issues"
	@echo "  make info        - Show project information"
	@echo "  make docs        - Generate API documentation with Doxygen"
	@echo ""
	@echo "ESP32-Specific Commands:"
	@echo "  make build-esp32     - Build for ESP32 only"
	@echo "  make upload-esp32   - Upload to ESP32"
	@echo "  make monitor-esp32  - Monitor ESP32 serial"
	@echo "  make run-esp32      - Build, upload, and monitor ESP32"
	@echo "  make test-esp32-button   - Run on-device button tests (Unity, requires ESP32)"
	@echo "  make test-esp32-serial          - Run on-device serial output tests (Unity, requires ESP32)"
	@echo "  make test-esp32-profilemanager  - Run on-device profile manager tests (Unity, requires ESP32)"
	@echo ""
	@echo "nRF52840-Specific Commands:"
	@echo "  make build-nrf52840    - Build for nRF52840 only"
	@echo "  make upload-nrf52840   - Upload to nRF52840"
	@echo "  make monitor-nrf52840  - Monitor nRF52840 serial"
	@echo "  make run-nrf52840     - Build, upload, and monitor nRF52840"
	@echo "  make test-nrf52840-button          - Run on-device button tests (Unity, requires nRF52840)"
	@echo "  make test-nrf52840-serial          - Run on-device serial output tests (Unity, requires nRF52840)"
	@echo "  make test-nrf52840-profilemanager  - Run on-device profile manager tests (Unity, requires nRF52840)"
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

monitor-esp32:
	pio device monitor -e $(ESP32_ENV)

run-esp32: build-esp32 upload-esp32
	@echo "Build and upload complete. Starting monitor..."
	pio device monitor -e $(ESP32_ENV) || echo "Monitor failed, but build/upload succeeded"

# nRF52840 specific targets
build-nrf52840:
	pio run -e $(NRF52840_ENV)

upload-nrf52840:
	pio run -e $(NRF52840_ENV) --target upload

monitor-nrf52840:
	pio device monitor -e $(NRF52840_ENV)

run-nrf52840: build-nrf52840 upload-nrf52840
	@echo "Build and upload complete. Starting monitor..."
	pio device monitor -e $(NRF52840_ENV) || echo "Monitor failed, but build/upload succeeded"

# Run on-device button tests (Unity via PlatformIO) — requires ESP32 connected
test-esp32-button:
	pio test -e nodemcu-32s-test -v

# Run on-device serial output tests (Unity via PlatformIO) — requires ESP32 connected
test-esp32-serial:
	pio test -e nodemcu-32s-serial-test -v

# Run on-device profile manager tests (Unity via PlatformIO) — requires ESP32 connected
test-esp32-profilemanager:
	pio test -e nodemcu-32s-profilemanager-test -v

# Run on-device button tests (Unity via PlatformIO) — requires nRF52840 connected
test-nrf52840-button:
	pio test -e feather-nrf52840-test -v

# Run on-device serial output tests (Unity via PlatformIO) — requires nRF52840 connected
test-nrf52840-serial:
	pio test -e feather-nrf52840-serial-test -v

# Run on-device profile manager tests (Unity via PlatformIO) — requires nRF52840 connected
test-nrf52840-profilemanager:
	pio test -e feather-nrf52840-profilemanager-test -v

# Run host unit tests (GoogleTest via CMake)
test-host:
	cmake --build .vscode/build --target pedal_tests && .vscode/build/test/pedal_tests

# Format all C++ files using clang-format
format:
	find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | grep -v \.pio | grep -v build | xargs clang-format -i
	@echo "All C++ files formatted successfully"

# Fix markdown linting issues (requires markdownlint-cli2)
lint-markdown:
	npx markdownlint-cli2 "**/*.md" "#node_modules" "#.pio" "#build" "#.vscode"
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

# Generate API documentation with Doxygen
docs:
	doxygen Doxyfile
	@echo "Documentation generated in docs/api/"

install-hooks:
	cp scripts/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
