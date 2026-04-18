# Android Emulator Setup Guide

This guide explains how to create and run an Android Virtual Device (AVD) for developing
and testing the AwesomeStudioPedal Flutter app — without a physical Android device.

> **BLE limitation:** The Android emulator does not support Bluetooth. See
> [Testing without hardware](#testing-without-hardware-ble-mock) at the bottom of this
> guide for the recommended workaround.

---

## Prerequisites

- Flutter installed and on your `PATH` (version from `app/.flutter-version`)
- **Java 17 JRE** — required by Android SDK tools (`sdkmanager`, `avdmanager`):
  - Linux: `sudo apt install openjdk-17-jre-headless`
  - Windows: bundled with Android Studio; no separate install needed
- **Android Studio** installed — it provides the AVD Manager and the required Android SDK
  components. Download from [developer.android.com/studio](https://developer.android.com/studio).

Run `flutter doctor` after installing to verify Android tooling is detected:

```
[✓] Android toolchain - develop for Android devices
    • Android SDK at /path/to/sdk
    • Android SDK version 34.x.x
```

---

## Linux setup

### 1. Install Android Studio (or use an existing SDK)

**Option A — Android Studio (recommended):**

Download the `.tar.gz` from [developer.android.com/studio](https://developer.android.com/studio),
extract it, and launch `android-studio/bin/studio.sh`. Follow the setup wizard — it will
install the Android SDK and all required components automatically.

**Option B — SDK already installed manually:**

If you already have the Android SDK (e.g. at `~/Android/Sdk`), you only need to add the
`cmdline-tools` component, which Flutter requires but standalone SDK downloads omit:

```bash
# Tell Flutter where your SDK lives
flutter config --android-sdk ~/Android/Sdk

# Download cmdline-tools manually
cd ~/Android/Sdk
mkdir -p cmdline-tools/latest
# Download from https://developer.android.com/studio#downloads (scroll to "Command line tools only")
# Extract the zip and move its contents into cmdline-tools/latest:
# unzip commandlinetools-linux-*.zip
# mv cmdline-tools/* cmdline-tools/latest/

# Accept SDK licenses
flutter doctor --android-licenses

# Verify
flutter doctor
```

Add the SDK tools to your `PATH` (add to `~/.bashrc` or `~/.zshrc`):

```bash
export ANDROID_HOME="$HOME/Android/Sdk"   # default Android Studio path on Linux
export PATH="$PATH:$ANDROID_HOME/emulator:$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin"
```

### 2. Enable hardware acceleration (KVM)

The emulator requires KVM for acceptable performance on Linux.

```bash
# Install KVM
sudo apt install qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils

# Add your user to the kvm group
sudo usermod -aG kvm $USER

# Verify (log out and back in first)
kvm-ok
# Expected: "KVM acceleration can be used"
```

If `kvm-ok` is not found: `sudo apt install cpu-checker`.

> **Virtualisation must be enabled in BIOS/UEFI.** If `kvm-ok` reports it cannot be used,
> reboot, enter your firmware settings, and enable Intel VT-x or AMD-V.

### 3. Create an AVD

**Option A — Android Studio GUI:**

Open Android Studio → **More Actions → Virtual Device Manager → Create device**.

Recommended settings:

| Setting | Value |
|---------|-------|
| Hardware profile | Pixel 6 |
| System image | API 34 (Android 14), x86_64, Google APIs |
| RAM | 2048 MB (default) |
| Graphics | Hardware — GLES 2.0 |

Click **Finish**. The emulator image will download (~1 GB).

**Option B — command line:**

```bash
# Download the system image first (required if not using Android Studio)
sdkmanager "system-images;android-34;google_apis;x86_64"

# Create the AVD
avdmanager create avd -n Pixel_6_API_34 \
  -k "system-images;android-34;google_apis;x86_64" \
  --device "pixel_6"
```

> **"SDK XML version 4" warning**: harmless — appears when the cmdline-tools version
> is older than the installed SDK. The commands still work correctly.

### 4. Launch and run the app

```bash
# Start the emulator (replace avd_name with your AVD name)
emulator -avd Pixel_6_API_34 &

# Wait for boot, then from the app/ directory:
flutter run
```

Flutter automatically detects the running emulator and deploys to it.

---

## Windows 11 setup

### 1. Install Android Studio

Download and run the `.exe` installer from
[developer.android.com/studio](https://developer.android.com/studio). The setup wizard
installs the Android SDK to `%LOCALAPPDATA%\Android\Sdk` by default.

Add SDK tools to your `PATH` (System → Advanced system settings → Environment Variables):

```
%LOCALAPPDATA%\Android\Sdk\emulator
%LOCALAPPDATA%\Android\Sdk\platform-tools
```

Set `ANDROID_HOME` to `%LOCALAPPDATA%\Android\Sdk`.

### 2. Enable hardware acceleration

**Option A — Hyper-V (recommended):**

Hyper-V is built into Windows 11 Pro/Enterprise. Enable it in
**Settings → Apps → Optional features → More Windows features → Hyper-V** (check all
sub-items), then reboot.

> **WSL2 users:** WSL2 already requires Hyper-V, so it is likely already enabled.
> The emulator uses WHPX (Windows Hypervisor Platform) automatically when Hyper-V is on.

**Option B — HAXM (alternative):**

Use HAXM if Hyper-V conflicts with another hypervisor (e.g. VMware with older versions).
In Android Studio: **SDK Manager → SDK Tools → Intel x86 Emulator Accelerator (HAXM) →
Apply**. Only one accelerator can be active at a time — disable Hyper-V if using HAXM.

### 3. Create an AVD

Open Android Studio → **More Actions → Virtual Device Manager → Create device**.

Recommended settings (same as Linux):

| Setting | Value |
|---------|-------|
| Hardware profile | Pixel 6 |
| System image | API 34 (Android 14), x86_64, Google APIs |
| RAM | 2048 MB |
| Graphics | Hardware — GLES 2.0 |

### 4. Launch and run the app

Open a terminal (PowerShell or Git Bash) in the `app/` directory:

```bash
flutter emulators --launch Pixel_6_API_34
flutter run
```

Or start the emulator from Android Studio's Device Manager and then run `flutter run`.

---

## Testing without hardware — BLE mock

The Android emulator has no Bluetooth hardware, so `BleService.scan()` will always return
an empty list and `connect()` will fail. For UI development, inject a mock `BleService`
instead.

### Using `--dart-define`

Pass a flag at launch time:

```bash
flutter run --dart-define=USE_MOCK_BLE=true
```

Then in `main.dart`, conditionally swap the provider:

```dart
const useMockBle = bool.fromEnvironment('USE_MOCK_BLE');

MultiProvider(
  providers: [
    ChangeNotifierProvider<BleService>(
      create: (_) => useMockBle ? MockBleService() : BleService(),
    ),
    // ...
  ],
)
```

### MockBleService implementation

Create `app/lib/services/mock_ble_service.dart`:

```dart
import 'dart:async';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'ble_service.dart';
import '../models/upload_result.dart';

class MockBleService extends BleService {
  @override
  Future<List<ScanResult>> scan({Duration timeout = const Duration(seconds: 10)}) async {
    await Future.delayed(const Duration(seconds: 1));
    // Returns an empty list — the ScannerScreen will show "No pedal found".
    // To simulate a found device, return a fake ScanResult here.
    return [];
  }

  @override
  Future<void> connect(BluetoothDevice device) async {
    await Future.delayed(const Duration(milliseconds: 500));
    // ignore: invalid_use_of_protected_member
    notifyListeners();
  }

  @override
  bool get isConnected => true;

  @override
  Future<UploadResult> uploadProfiles(String json) async {
    await Future.delayed(const Duration(seconds: 2));
    return const UploadResult.success();
  }

  @override
  Future<UploadResult> uploadConfig(String json) async {
    await Future.delayed(const Duration(seconds: 1));
    return const UploadResult.success();
  }
}
```

This mock is already used in widget and integration tests via `MockBleService` generated
by mockito — see `test/integration/app_flow_test.dart` for reference.

---

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `HAXM is not installed` (Windows) | HAXM not installed or Hyper-V conflict | Install HAXM via SDK Manager, or switch to Hyper-V/WHPX |
| Emulator very slow | KVM/HAXM not active | Verify acceleration: `emulator -accel-check` |
| `flutter run` doesn't detect emulator | Emulator not fully booted | Wait for the home screen to appear, then re-run |
| `No connected devices` | `ANDROID_HOME` not set | Add `ANDROID_HOME` and SDK tools to `PATH`, restart terminal |
| `Package path is not valid` in `avdmanager` | System image not downloaded | Run `sdkmanager "system-images;android-34;google_apis;x86_64"` first |
| `JAVA_HOME is not set` / `java` not found | Java not installed | `sudo apt install openjdk-17-jre-headless` (Linux); Android Studio bundles Java on Windows |
| `cmdline-tools component is missing` | SDK installed without cmdline-tools | See "Option B" in Linux setup above; download cmdline-tools zip and place in `$ANDROID_HOME/cmdline-tools/latest/` |
| `Android license status unknown` | Licenses not accepted | Run `flutter doctor --android-licenses` and accept all |
| `flutter doctor` shows Android SDK missing | SDK path wrong | Run `flutter config --android-sdk ~/Android/Sdk` |
| API 34 image not listed | Not downloaded | Android Studio → SDK Manager → SDK Platforms → Android 14 → Apply; or `sdkmanager "platforms;android-34"` |
