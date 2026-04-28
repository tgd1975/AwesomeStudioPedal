#!/usr/bin/env python3
"""
BLE Config Integration Test Runner
===================================
Connects to the ESP32 BLE Config test harness (test_main.cpp) as a BLE client
and runs 4 end-to-end test scenarios:

  1. Valid upload     — uploads profiles.json via BLE; expects CONFIG_STATUS "OK"
                        and serial PROFILE line matching the first profile name.
  2. LED confirmation — success path implies 3× LED blink (visible on hardware;
                        here we verify the transfer completes without error).
  3. Error recovery   — sends malformed JSON (expects ERROR:parse_failed); then
                        valid JSON (expects OK) — verifies clean reset.
  4. Persistence      — after OK upload, triggers ESP32 soft-reset via serial;
                        reconnects after 5 s; verifies active profile unchanged.

Prerequisites
-------------
- ESP32 (NodeMCU-32S) connected via USB with test_main.cpp firmware flashed.
- BLE-capable host (Linux with bleak, or macOS).
- `bleak` and `pyserial` installed: pip install bleak pyserial

Usage
-----
    python3 test/test_ble_config_esp32/runner.py [--port /dev/ttyUSB0]

Return codes: 0 = all tests passed, 1 = one or more tests failed.
"""

import argparse
import asyncio
import json
import pathlib
import re
import serial
import sys
import time

# Reuse chunking logic from the CLI tool — single source of truth.
SCRIPTS_DIR = pathlib.Path(__file__).resolve().parent.parent.parent / "scripts"
sys.path.insert(0, str(SCRIPTS_DIR))
import pedal_config

try:
    from bleak import BleakClient, BleakScanner
except ImportError:
    print("ERROR: bleak not installed. Run: pip install bleak", file=sys.stderr)
    sys.exit(1)

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

SERIAL_BAUD    = 115200
SERIAL_TIMEOUT = 30   # seconds to wait for "[BLE_TEST] READY"
BLE_SCAN_TIMEOUT   = 15   # seconds to scan for device
BLE_STATUS_TIMEOUT = 15   # seconds to wait for CONFIG_STATUS notification
RECONNECT_DELAY    = 5    # seconds to wait before reconnecting after reset

DATA_DIR = pathlib.Path(__file__).resolve().parent.parent.parent / "data"
VALID_PROFILES_PATH = DATA_DIR / "profiles.json"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

PASS = "\033[32mPASS\033[0m"
FAIL = "\033[31mFAIL\033[0m"

results: list[tuple[str, bool]] = []


def report(name: str, ok: bool, detail: str = "") -> bool:
    tag = PASS if ok else FAIL
    print(f"  [{tag}] {name}" + (f" — {detail}" if detail else ""))
    results.append((name, ok))
    return ok


def wait_for_ready(port: str) -> serial.Serial:
    """Open serial port, trigger ESP32 reset via DTR, then wait for READY."""
    print(f"Opening serial port {port} at {SERIAL_BAUD} baud…")
    ser = serial.Serial(port, SERIAL_BAUD, timeout=1)
    # Pulse DTR low→high to reset the ESP32 so we don't miss the READY line.
    ser.setDTR(False)
    time.sleep(0.1)
    ser.setDTR(True)
    deadline = time.time() + SERIAL_TIMEOUT
    while time.time() < deadline:
        line = ser.readline().decode("utf-8", errors="replace").strip()
        if line:
            print(f"  serial: {line}")
        if "[BLE_TEST] ERROR:" in line:
            ser.close()
            raise RuntimeError(f"Firmware halted with infrastructure error: {line}")
        if "[BLE_TEST] READY" in line:
            print("Firmware ready.")
            return ser
    ser.close()
    raise RuntimeError(f"Timed out waiting for [BLE_TEST] READY on {port}")


def read_serial_line(ser: serial.Serial, timeout: float = 5.0,
                     keyword: str = "[BLE_TEST]") -> str | None:
    """Read lines until one matches *keyword* or timeout."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        line = ser.readline().decode("utf-8", errors="replace").strip()
        if line:
            print(f"  serial: {line}")
        if keyword in line:
            return line
    return None


NAME_PREFIX = "AwesomeStudioPe"


async def find_pedal(timeout: float = BLE_SCAN_TIMEOUT) -> object:
    """Scan for the pedal by Config service UUID or device name prefix.

    Production firmware (BleKeyboard) does not advertise the Config service
    UUID — it advertises HID UUID + device name. Falling back to name-prefix
    matching mirrors the CLI tool (pedal_config.py) and works with both the
    production firmware and any test firmware that does advertise the UUID.
    """
    print(f"Scanning for pedal ({timeout} s)…")

    def _is_pedal(d, adv) -> bool:
        uuids = [str(u).lower() for u in (adv.service_uuids or [])]
        if pedal_config.SERVICE_UUID.lower() in uuids:
            return True
        name = (adv.local_name or d.name or "")
        return name.startswith(NAME_PREFIX)

    device = await BleakScanner.find_device_by_filter(_is_pedal, timeout=timeout)
    return device


async def upload_json(client: BleakClient, json_str: str, hw: bool = False) -> str:
    """Upload *json_str* via BLE and return the CONFIG_STATUS string received."""
    char_uuid = pedal_config.CHAR_WRITE_HW_UUID if hw else pedal_config.CHAR_WRITE_UUID
    status_received: list[str] = []

    def on_notify(_char, data: bytearray) -> None:
        status_received.append(data.decode("utf-8", errors="replace"))

    # Give BlueZ time to complete service discovery before subscribing/writing.
    await asyncio.sleep(0.5)

    await client.start_notify(pedal_config.CHAR_STATUS_UUID, on_notify)

    chunks = pedal_config.make_chunks(json_str.encode())
    for chunk in chunks:
        await client.write_gatt_char(char_uuid, chunk, response=True)

    # Wait for STATUS notification
    deadline = asyncio.get_event_loop().time() + BLE_STATUS_TIMEOUT
    while asyncio.get_event_loop().time() < deadline:
        if status_received:
            break
        await asyncio.sleep(0.1)

    await client.stop_notify(pedal_config.CHAR_STATUS_UUID)
    return status_received[-1] if status_received else ""


# ---------------------------------------------------------------------------
# Test scenarios
# ---------------------------------------------------------------------------

async def run_tests(ser: serial.Serial) -> int:
    device = await find_pedal()
    if device is None:
        print("ERROR: no pedal device found — is the firmware running and advertising?",
              file=sys.stderr)
        return 1

    print(f"Found: {device.name} ({device.address})")

    valid_json = VALID_PROFILES_PATH.read_text()
    first_profile_name: str | None = None
    try:
        first_profile_name = json.loads(valid_json)["profiles"][0]["name"]
    except (KeyError, IndexError):
        pass

    # ------------------------------------------------------------------
    # Test 1 — Valid upload
    # ------------------------------------------------------------------
    print("\n[Test 1] Valid upload →")
    async with BleakClient(device) as client:
        status = await upload_json(client, valid_json)

    ok1 = status == "OK"
    report("Valid upload: CONFIG_STATUS == OK", ok1, f"got '{status}'")

    # Query active profile via serial command. Drain the input buffer first so
    # we don't read a stale boot-time "[BLE_TEST] PROFILE:Navigation" emit (the
    # firmware emits one on every profile change in loop() at line 132 of
    # test_main.cpp, and the boot default fires before the first upload).
    ser.reset_input_buffer()
    ser.write(b"PROFILE?\n")
    serial_line = read_serial_line(ser, timeout=5.0, keyword="[BLE_TEST] PROFILE:") or ""
    profile_match = re.search(r"\[BLE_TEST\] PROFILE:(.+)", serial_line)
    profile_name = profile_match.group(1) if profile_match else None

    if first_profile_name:
        ok_name = profile_name == first_profile_name
        report("Valid upload: active profile reported via serial",
               ok_name, f"expected '{first_profile_name}', got '{profile_name}'")

    # ------------------------------------------------------------------
    # Test 2 — LED confirmation (implicit in success path; confirm no error)
    # ------------------------------------------------------------------
    print("\n[Test 2] LED confirmation →")
    # LED blink timing is 3×150 ms on/off — hardware-visible; we confirm
    # no error status was returned (already verified in Test 1).
    report("LED confirmation: no error on success path", ok1)

    # ------------------------------------------------------------------
    # Test 3 — Error recovery
    # ------------------------------------------------------------------
    print("\n[Test 3] Error recovery →")
    async with BleakClient(device) as client:
        bad_status = await upload_json(client, "{bad json{{{{")

    ok3a = "ERROR" in bad_status
    report("Error recovery: malformed JSON → ERROR status",
           ok3a, f"got '{bad_status}'")

    # Upload valid JSON after error — service must have reset cleanly
    async with BleakClient(device) as client:
        recover_status = await upload_json(client, valid_json)

    ok3b = recover_status == "OK"
    report("Error recovery: valid upload after error → OK",
           ok3b, f"got '{recover_status}'")

    # ------------------------------------------------------------------
    # Test 4 — Persistence
    # ------------------------------------------------------------------
    print("\n[Test 4] Persistence after soft-reset →")
    # Capture active profile before reset
    ser.write(b"PROFILE?\n")
    pre_reset_line = read_serial_line(ser, timeout=5.0, keyword="[BLE_TEST] PROFILE:") or ""
    pre_match = re.search(r"\[BLE_TEST\] PROFILE:(.+)", pre_reset_line)
    pre_name = pre_match.group(1) if pre_match else None

    # Trigger soft-reset
    ser.write(b"RESET\n")
    reset_line = read_serial_line(ser, timeout=5.0, keyword="[BLE_TEST] RESET") or ""
    reset_ok = "[BLE_TEST] RESET" in reset_line
    report("Persistence: soft-reset acknowledged", reset_ok)

    # Wait for reboot and re-advertise
    print(f"  Waiting {RECONNECT_DELAY} s for reboot…")
    await asyncio.sleep(RECONNECT_DELAY)

    # Wait for READY again
    deadline = time.time() + SERIAL_TIMEOUT
    while time.time() < deadline:
        line = ser.readline().decode("utf-8", errors="replace").strip()
        if line:
            print(f"  serial: {line}")
        if "[BLE_TEST] READY" in line:
            break

    # Reconnect and query profile
    device2 = await find_pedal(timeout=BLE_SCAN_TIMEOUT)
    if device2 is None:
        report("Persistence: reconnect after reboot", False,
               "device not found after reboot")
    else:
        ser.write(b"PROFILE?\n")
        post_line = read_serial_line(ser, timeout=5.0, keyword="[BLE_TEST] PROFILE:") or ""
        post_match = re.search(r"\[BLE_TEST\] PROFILE:(.+)", post_line)
        post_name = post_match.group(1) if post_match else None

        ok4 = (pre_name is not None) and (pre_name == post_name)
        report("Persistence: profile survives reboot",
               ok4, f"before='{pre_name}', after='{post_name}'")

    return 0


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description="BLE Config Integration Test Runner")
    parser.add_argument(
        "--port", default="/dev/ttyUSB0",
        help="Serial port of the ESP32 (default: /dev/ttyUSB0)"
    )
    args = parser.parse_args()

    try:
        ser = wait_for_ready(args.port)
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1

    early_exit = 0
    try:
        early_exit = asyncio.run(run_tests(ser))
    finally:
        ser.close()

    # Summary
    print("\n" + "=" * 50)
    passed = sum(1 for _, ok in results if ok)
    total  = len(results)
    print(f"Results: {passed}/{total} passed")
    if early_exit != 0:
        print("OVERALL: FAIL")
        return 1
    if passed == total and total > 0:
        print("OVERALL: PASS")
        return 0
    else:
        failed = [n for n, ok in results if not ok]
        print(f"FAILED: {', '.join(failed)}")
        print("OVERALL: FAIL")
        return 1


if __name__ == "__main__":
    sys.exit(main())
