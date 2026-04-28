---
id: TASK-161
title: Publish app to Apple App Store
status: paused
opened: 2026-04-19
effort: Large (8-24h)
complexity: High
human-in-loop: Main
epic: iphone-app
order: 2
---

## Description

Prepare and submit the Flutter app to the Apple App Store via App Store Connect.
This covers Apple Developer Program enrollment, provisioning, signing, store listing,
and the initial submission including Apple's review process.

## Pre-conditions

- Apple Developer Program membership ($99/year)
- macOS machine with Xcode 15+ (required for building and submitting iOS apps)
- App passes all iOS feature tests (TASK-158)
- App icon, screenshots, and description ready (ideally after TASK-162 design work)
- Privacy policy URL (required by Apple for apps that request Bluetooth permission)

## Steps

### 1 — Apple Developer Program & App Store Connect

- [ ] Enroll in Apple Developer Program at developer.apple.com
- [ ] In App Store Connect: create a new app entry
  - Bundle ID: `com.awesomestudiopedal.app` (register in the Certificates, IDs & Profiles portal first)
  - SKU: any unique string (e.g. `awesomestudiopedal-1`)
  - Primary language: English

### 2 — Certificates & provisioning profiles

- [ ] In Xcode: Signing & Capabilities → enable "Automatically manage signing" with your team
- [ ] Or manually: create a Distribution certificate and an App Store provisioning profile in the portal
- [ ] Verify `flutter build ios --release` succeeds without signing errors

### 3 — Info.plist — required usage descriptions

Verify or add these keys in `app/ios/Runner/Info.plist`:

| Key | Value |
|-----|-------|
| `NSBluetoothAlwaysUsageDescription` | "Used to connect to your AwesomeStudioPedal hardware over Bluetooth" |
| `NSBluetoothPeripheralUsageDescription` | "Used to connect to your AwesomeStudioPedal hardware over Bluetooth" |

Apple will reject the app without these.

### 4 — Store listing in App Store Connect

- [ ] App name: "AwesomeStudioPedal"
- [ ] Subtitle (30 chars): "BLE foot pedal for musicians"
- [ ] Description: adapt from docs/musicians/ — explain the hardware + app pairing concept
- [ ] Keywords (100 chars total): `pedal,BLE,musician,keyboard,foot switch,MIDI,studio`
- [ ] Support URL: GitHub repo URL or GitHub Pages site
- [ ] Privacy policy URL (required)
- [ ] Upload screenshots:
  - iPhone 6.7" (iPhone 15 Pro Max): at minimum 3 screens
  - iPhone 6.5" (iPhone 14 Plus / 13 Pro Max): required for older device compatibility
  - iPad 12.9" (optional but recommended)
- [ ] Upload app icon (1024×1024 px PNG, no transparency, no rounded corners — App Store adds them)
- [ ] Age rating: 4+ (no objectionable content)

### 5 — Build & upload

- [ ] `flutter build ipa` — produces an `.ipa` suitable for distribution
- [ ] Upload via Xcode Organizer → Distribute App → App Store Connect
  - Or use `xcrun altool` / Transporter app
- [ ] In App Store Connect → TestFlight: invite yourself and run a smoke test
- [ ] Promote to App Store submission once TestFlight smoke test passes

### 6 — App Review submission

- [ ] Fill in "App Review Information": demo account or instructions not needed (no login)
- [ ] Add a note to reviewers: "Requires AwesomeStudioPedal Bluetooth hardware to use BLE
  features; all UI screens are accessible without hardware"
- [ ] Submit for review (typical review time: 1–3 days)

## Acceptance Criteria

- [ ] App available on TestFlight for internal testing
- [ ] Store listing complete: description, screenshots, icon, privacy policy, keywords
- [ ] App submitted for App Store review
- [ ] All required `Info.plist` usage description keys present
- [ ] Signing certificates and provisioning profiles documented in a private note or password manager

## Notes

- Apple rejects apps that crash on launch, reference placeholder content in the description,
  or are missing required usage descriptions — verify TASK-158 tests pass first
- If the reviewer cannot test BLE features (no hardware), the review note in step 6 is important
- Bundle ID is permanent after submission; choose carefully
- `flutter build ipa` requires macOS; cannot be done from a Linux dev container

## Paused

- 2026-04-27: Paused — no iPhone is currently on hand. App Store submission
  depends on TASK-158 (iOS feature tests on physical hardware) passing first,
  which is itself blocked on iPhone availability. Resume with
  `/ts-task-active TASK-161` once an iPhone is available and TASK-158 has
  been completed.
