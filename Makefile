# Makefile for AwesomeGuitarPedal ESP32 project
# Uses PlatformIO as the build system

.PHONY: all build upload clean monitor test test-host test-coverage

# Default target
all: build

# Build the project using PlatformIO
build:
	pio run

# Upload firmware to device
upload:
	pio run --target upload

# Clean build artifacts
clean:
	pio run --target clean

# Open serial monitor
monitor:
	pio device monitor

# Run PlatformIO on-device tests
test:
	pio test -e nodemcu-32s-test

# Run host unit tests (GoogleTest via CMake)
test-host:
	cmake --build .vscode/build --target pedal_tests && .vscode/build/pedal_tests

# Run on-device tests with coverage
test-coverage:
	pio test -e nodemcu-32s-test --coverage

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

install-hooks:
	cp scripts/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
