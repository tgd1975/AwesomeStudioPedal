# Release Checklist

This document outlines the steps required to prepare and publish a new release of AwesomeStudioPedal.

## Pre-Release Preparation

- [ ] Ensure all planned features and bug fixes are completed.
- [ ] Verify that all tests pass successfully.
- [ ] Update the `CHANGELOG.md` file with the changes for the new release.
- [ ] Ensure the `README.md` file is up-to-date with the latest information.

## Code and Documentation

- [ ] Verify that all code is properly formatted and linted.
- [ ] Ensure all documentation is accurate and complete.
- [ ] Update version numbers in relevant files (e.g., `CMakeLists.txt`, `platformio.ini`).

## Testing

- **Hardware Testing**:
  - [ ] Test on ESP32 (NodeMCU-32S) hardware.
  - [ ] Test on nRF52840 hardware (if available).
- **Button Configurations**:
  - [ ] Test all button mappings (e.g., keypress, media commands, typed strings).
  - [ ] Verify profile switching functionality.
  - [ ] Test time-delayed actions.
- **Bluetooth Connectivity**:
  - [ ] Verify Bluetooth pairing and connectivity.
  - [ ] Test keyboard emulation (keypresses, media keys).
  - [ ] Ensure no latency or connectivity issues.
- **Edge Cases**:
  - [ ] Test rapid button presses.
  - [ ] Verify behavior with multiple profiles.
  - [ ] Test power cycling and reconnection.

## Build and Packaging

- [ ] Build firmware binaries for all supported platforms.
- [ ] Create release packages with firmware binaries and documentation.
- [ ] Ensure all necessary files are included in the release package.

## Release Process

- [ ] Create a new release tag in GitHub.
- [ ] Upload the firmware binaries and release notes to the GitHub release.
- [ ] Publish the release on GitHub.

## Post-Release

- [ ] Announce the release on relevant platforms (e.g., GitHub Discussions, social media).
- [ ] Monitor for any issues reported by users and address them promptly.
- [ ] Update the project roadmap or future ideas based on feedback.
