# Makefile for AwesomeGuitarPedal ESP32 project
# Uses PlatformIO as the build system

.PHONY: all build upload clean monitor test

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

# Run tests
test:
	pio test

# Show project information
info:
	pio run --list-targets

# Shortcut for common operations
flash: upload

# Environment variables can be set here if needed
# export PLATFORMIO_BUILD_FLAGS = "-DDEBUG"

# Note: This Makefile assumes PlatformIO is installed and available in PATH
# Install with: pip install platformio

# Project-specific PlatformIO configuration is in platformio.ini
# Test targets
test:
	pio test

test-coverage:
	pio test --coverage

# Clean test artifacts
clean-test:
	rm -rf .pio/build/test
