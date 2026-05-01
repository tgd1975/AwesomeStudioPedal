// Local Name prefix advertised by the pedal firmware
// (HookableBleKeyboard("AwesomeStudioPedal", …) in
// lib/hardware/esp32/src/ble_keyboard_adapter.cpp). Used as a prefix
// match for client-side scan filtering — the firmware advertises only
// the HID service UUID (0x1812), not our 128-bit config UUID, so a
// withServices: filter on the config UUID would reject the pedal. See
// docs/developers/KNOWN_ISSUES.md (BlueZ HID auto-load) and
// task-258 (in-app scan empty while OS sees the device).
//
// We intentionally use a 13-char prefix instead of the full 18-char
// "AwesomeStudioPedal" because the BLE 4.x advertising PDU is 31 B and
// the GAP Local Name field is hard-capped to ~14 chars once Flags /
// TX Power / Service UUID AD entries are included. The platform stack
// (Android in particular) truncates anything longer in the pre-connect
// scan list — Pixel 9 reports "AwesomeStudioPe" for the full 18-char
// name. The full name is reachable post-connect via the GAP Device
// Name characteristic, but a 13-char prefix is what reliably survives
// the pre-connect scan filter on every platform we support.
const String kPedalNamePrefix = 'AwesomeStudio';

const String kServiceUuid = '516515c0-4b50-447b-8ca3-cbfce3f4d9f8';
const String kConfigWriteUuid = '516515c1-4b50-447b-8ca3-cbfce3f4d9f8';
const String kConfigWriteHwUuid = '516515c2-4b50-447b-8ca3-cbfce3f4d9f8';
const String kConfigStatusUuid = '516515c3-4b50-447b-8ca3-cbfce3f4d9f8';
// Read-only: returns the board identity string ("esp32" / "nrf52840").
const String kHwIdentityUuid = '516515c4-4b50-447b-8ca3-cbfce3f4d9f8';
// Read-only: returns the canonical firmware version string from
// include/version.h (e.g. "v0.4.1"). ESP32 only — nRF52840 has no custom
// GATT service today (deferred to TASK-358).
const String kFirmwareVersionUuid = '516515c5-4b50-447b-8ca3-cbfce3f4d9f8';

// Wire ceiling for an uploaded config. Mirrors MAX_CONFIG_BYTES in
// lib/PedalLogic/include/ble_config_reassembler.h — keep in sync. Pre-flight
// here so the user gets a clear "file too large" error instead of a
// multi-second BLE upload that ends in ERROR:parse_failed (TASK-240).
const int kMaxConfigBytes = 16 * 1024; // 16384
