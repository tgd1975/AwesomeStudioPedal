---
id: TASK-160
title: Publish app to Google Play Store
status: open
opened: 2026-04-19
effort: Large (8-24h)
complexity: Medium
human-in-loop: Main
epic: distribution
order: 2
---

## Description

Prepare and publish the Flutter app to the Google Play Store as an open-source / hobby
release. This covers account setup, store listing, signing, release build, and the initial
submission.

## Pre-conditions

- Google Play Developer account ($25 one-time fee)
- App passes all Android feature tests (TASK-153–TASK-157)
- App icon, screenshots, and short/long description ready (ideally after TASK-162 design work)
- Privacy policy URL (required by Google Play for apps that request Bluetooth permission)

## Steps

### 1 — Account & app registration

- [ ] Create Google Play Developer account at play.google.com/console
- [ ] Create a new app entry; set package name `com.awesomestudiopedal.app`
  (check `app/android/app/build.gradle.kts` for current `applicationId` and align)
- [ ] Complete the app content rating questionnaire
- [ ] Add a privacy policy URL (can be a simple GitHub Pages page or README section)

### 2 — Signing

- [ ] Generate a release keystore:

  ```bash
  keytool -genkey -v -keystore upload-keystore.jks \
    -keyalg RSA -keysize 2048 -validity 10000 \
    -alias upload
  ```

- [ ] Store `upload-keystore.jks` **outside the repo** (never commit)
- [ ] Create `app/android/key.properties` (gitignored):

  ```
  storePassword=<password>
  keyPassword=<password>
  keyAlias=upload
  storeFile=<absolute path to upload-keystore.jks>
  ```

- [ ] Wire signing config into `app/android/app/build.gradle.kts`
- [ ] Verify `make flutter-build` (or `flutter build apk --release`) uses the release key

### 3 — Store listing

- [ ] App title: "AwesomeStudioPedal"
- [ ] Short description (80 chars max): "BLE foot pedal controller for musicians — configure button actions from your phone"
- [ ] Full description (4000 chars max): see docs/musicians/ for inspiration
- [ ] Category: Music
- [ ] Upload at least 2 phone screenshots (from TASK-157 feature tests or dedicated screen captures)
- [ ] Upload feature graphic (1024×500 px)
- [ ] Upload app icon (512×512 px, PNG, no rounded corners — Play Store adds them)

### 4 — Release build & upload

- [ ] `flutter build appbundle --release` (`.aab` is required for Play Store; `.apk` is sideload only)
- [ ] Upload `.aab` to the Internal Testing track first
- [ ] Install via Play Store internal testing link; run smoke test
- [ ] Promote to Open Testing or Production once smoke test passes

### 5 — Permissions declaration

Google Play requires a declaration for `BLUETOOTH_SCAN` / `BLUETOOTH_CONNECT` (Android 12+):

- [ ] In Play Console → App content → Permissions: declare BLE is used to communicate with
  the hardware pedal device; no location data is derived

## Acceptance Criteria

- [ ] App available on Internal Testing track in Google Play Console
- [ ] App installable on a fresh device via Play Store link
- [ ] Store listing complete with description, screenshots, and icon
- [ ] Signing key stored securely outside the repo; documented in a private note or password manager
- [ ] Privacy policy linked from the Play Store listing

## Notes

- Use App Bundle (`.aab`) not APK for Play Store submission
- The `applicationId` in `build.gradle.kts` must be unique and final — changing it after
  publishing creates a new app
- Keep the keystore and `key.properties` out of version control — add to `.gitignore`
