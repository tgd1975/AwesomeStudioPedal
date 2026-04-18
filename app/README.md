# AwesomeStudioPedal — Mobile App

Flutter application for configuring AwesomeStudioPedal profiles and hardware settings over BLE.

## Prerequisites

- Flutter (see `app/.flutter-version` for the pinned version) — install from [flutter.dev](https://flutter.dev/docs/get-started/install)
- For Android builds: Android SDK with API 34 and build-tools 34.0.0
- For iOS builds: macOS with Xcode 15+ (cannot build iOS from Linux/Windows)

### Why Flutter is not in the dev container

The dev container is optimised for C++/firmware/docs development. Installing Flutter
(~700 MB) at container build time makes rebuilds slow and fails in network-restricted
environments (GitHub Codespaces build phase, corporate proxies). Instead:

- **Local Flutter development**: install Flutter from flutter.dev and add it to `PATH`.
  The version in `app/.flutter-version` is the single source of truth — it is also read
  by FVM (`fvm use`) if you use the Flutter Version Manager.
- **CI**: Flutter jobs run natively on `ubuntu-latest` via `subosito/flutter-action`,
  which caches the SDK between runs. The version is read from `app/.flutter-version`
  automatically. See `.github/workflows/app.yml`.

## Getting started

```bash
cd app
flutter pub get
flutter analyze
flutter test
```

## Running on a device

```bash
# Android (USB debugging enabled)
flutter run

# iOS simulator (macOS only)
flutter run -d "iPhone 15"
```

## Building a release APK

```bash
flutter build apk --release
# Output: build/app/outputs/flutter-apk/app-release.apk
```

## iOS build (manual — macOS + Xcode required)

```bash
flutter build ios --no-codesign
```

> iOS build is not run in CI. The CI workflow (`app.yml`) builds Android only.
> To distribute for iOS, use Xcode to archive and export via the standard iOS distribution workflow.

## iOS BLE notes

On iOS, the pedal advertises two Bluetooth services simultaneously:
- **HID keyboard profile** — paired via iOS Settings → Bluetooth. The system handles this transparently; the app does not interact with it.
- **Custom GATT config service** (`516515c0-...`) — this is what the app connects to for profile and hardware config uploads. CoreBluetooth accesses it independently of the HID pairing.

The app and the HID keyboard can both be active at the same time without conflict.

## Architecture overview

```
app/
  lib/
    main.dart           — entry point
    app.dart            — MaterialApp + go_router setup
    constants/          — BLE UUIDs, action type lists
    models/             — Profile, ActionConfig, HardwareConfig, MacroStep
    services/           — BleService, SchemaService, FileService
    screens/            — HomeScreen, ProfileListScreen, ProfileEditorScreen, ...
    widgets/            — Reusable UI components
  test/
    unit/               — Model round-trip and service logic tests
    widget/             — Widget rendering tests
    integration/        — End-to-end app flow tests
```

## CI

The GitHub Actions workflow (`.github/workflows/app.yml`) runs on every push or PR that touches `app/`:
1. `flutter pub get`
2. `flutter analyze`
3. `flutter test`
4. `flutter build apk --release`
