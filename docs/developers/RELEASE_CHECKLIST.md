# Release Checklist

This document outlines the steps required to prepare and publish a new release of AwesomeStudioPedal.

## Pre-Release Preparation

- [ ] Ensure all planned features and bug fixes are completed.
- [ ] Verify that all tests pass successfully (`make test-host`).
- [ ] Update `CHANGELOG.md` — move items from `[Unreleased]` to the new version section.
- [ ] Ensure `README.md` is up-to-date.
- [ ] Verify that all code is properly formatted and linted (enforced by pre-commit hook).
- [ ] Ensure all documentation is accurate and complete.

## Hardware Testing

- [ ] Test on ESP32 (NodeMCU-32S) hardware.
- [ ] Test on nRF52840 hardware (if available).
- **Button Configurations**:
  - [ ] Test all button mappings (keypress, media commands, typed strings).
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

## Branch & Merge

- [ ] Squash all commits on the feature branch into a single logical commit per feature/fix/refactor.
- [ ] Open a pull request against `main`.
- [ ] Ensure CI passes on the PR (tests, static analysis, CodeQL, markdownlint).
- [ ] Merge the pull request into `main`.

## Tagging the Release

- [ ] On `main`, create an annotated tag: `git tag -a vX.Y.Z -m "Release vX.Y.Z"`
- [ ] Push the tag: `git push origin vX.Y.Z`
- [ ] Create a GitHub release from the tag with the CHANGELOG entry as release notes.

## Build and Packaging

- [ ] Build firmware binaries for all supported platforms (`make build`).
- [ ] Attach firmware binaries to the GitHub release (once first public release is ready — see TASK-060).

## Post-Release

- [ ] Announce the release on relevant platforms (e.g., GitHub Discussions).
- [ ] Monitor for issues reported by users and address them promptly.
- [ ] Update the project roadmap or future ideas based on feedback.
