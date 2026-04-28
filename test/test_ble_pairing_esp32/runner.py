#!/usr/bin/env python3
"""
BLE Pairing Smoke Test Runner (Linux / BlueZ)
==============================================
Drives the full pairing-PIN cycle against an ESP32 with production firmware
and `pairing_pin: 12345` in data/config.json. Replaces the manual phone-based
verification step from TASK-238 with a fully automated check.

What it tests
-------------
1. Right passkey (012345) → pairing succeeds, Paired+Bonded both true.
2. Reconnect after pairing → no second prompt, bond reused.
3. Wrong passkey → pairing rejected, no bond created.

What it requires
----------------
- Linux with BlueZ (`bluetoothctl` 5.x).
- pexpect, esptool.py (PlatformIO bundles one).
- ESP32 connected via USB (default /dev/ttyUSB0).
- A repo-root checkout with a working `make upload-esp32` / `make uploadfs-esp32`.

What it changes
---------------
- Removes any existing host-side bond for the pedal.
- Erases the pedal's NVS partition (NimBLE bond store) via esptool.
- Reflashes production firmware + production filesystem.
- All bonds created during the test are removed at the end (clean teardown).

Usage
-----
    python3 test/test_ble_pairing_esp32/runner.py [--port /dev/ttyUSB0]

Return codes: 0 = all assertions passed, 1 = one or more failed.
"""

import argparse
import os
import pathlib
import re
import shutil
import subprocess
import sys
import time

try:
    import pexpect
except ImportError:
    print("ERROR: pexpect not installed. Run: pip install pexpect", file=sys.stderr)
    sys.exit(1)

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

REPO_ROOT          = pathlib.Path(__file__).resolve().parent.parent.parent
NAME_PREFIX        = "AwesomeStudioPe"
EXPECTED_PASSKEY   = "012345"   # data/config.json: pairing_pin = 12345
WRONG_PASSKEY      = "999999"
SCAN_TIMEOUT       = 15         # s
PAIR_TIMEOUT       = 30         # s
RECONNECT_TIMEOUT  = 15         # s

# NVS partition (matches config/esp32/partitions.csv:nvs)
NVS_OFFSET         = "0x9000"
NVS_SIZE           = "0x5000"

# Locate PlatformIO's bundled esptool.py.
ESPTOOL_CANDIDATES = [
    pathlib.Path.home() / ".platformio/packages/tool-esptoolpy/esptool.py",
]


# ---------------------------------------------------------------------------
# PASS/FAIL bookkeeping
# ---------------------------------------------------------------------------

PASSED, FAILED = [], []


def report(label: str, ok: bool, detail: str = "") -> None:
    bucket = PASSED if ok else FAILED
    bucket.append(label)
    tag = "[PASS]" if ok else "[FAIL]"
    print(f"  {tag} {label}" + (f" — {detail}" if detail else ""))


# ---------------------------------------------------------------------------
# Preflight
# ---------------------------------------------------------------------------

def preflight(port: str) -> pathlib.Path:
    """Verify required tooling is present. Returns esptool.py path."""
    if not shutil.which("bluetoothctl"):
        die("bluetoothctl not found. Install BlueZ: sudo apt install bluez")
    if not shutil.which("make"):
        die("make not found.")
    if not pathlib.Path(port).exists():
        die(f"Serial port not found: {port}")
    for c in ESPTOOL_CANDIDATES:
        if c.exists():
            return c
    die("PlatformIO esptool.py not found. Run any `pio run` first to install it.")


def die(msg: str) -> None:
    print(f"ERROR: {msg}", file=sys.stderr)
    sys.exit(2)


# ---------------------------------------------------------------------------
# Discovery — find the pedal's BD_ADDR by name prefix.
# ---------------------------------------------------------------------------

def discover_pedal(timeout: float = SCAN_TIMEOUT) -> str:
    """Scan BlueZ for a device whose name starts with NAME_PREFIX.

    Returns the BD_ADDR (e.g. '24:62:AB:D4:E0:D2'). Aborts on no match.
    """
    print(f"Scanning for {NAME_PREFIX}* (up to {int(timeout)} s)…")

    # First pass: existing devices BlueZ already remembers (cheap).
    addr = _scan_devices_command()
    if addr:
        print(f"  found cached: {addr}")
        return addr

    # Second pass: trigger a fresh scan via timed bluetoothctl.
    proc = subprocess.run(
        ["bluetoothctl", "--timeout", str(int(timeout)), "scan", "on"],
        capture_output=True, text=True,
    )
    for line in (proc.stdout + proc.stderr).splitlines():
        m = re.search(r"Device ([0-9A-Fa-f:]{17}) " + re.escape(NAME_PREFIX), line)
        if m:
            print(f"  found via scan: {m.group(1)}")
            return m.group(1)

    addr = _scan_devices_command()
    if addr:
        return addr
    die(f"No device matching '{NAME_PREFIX}' found within {int(timeout)} s. "
        "Is the pedal powered and advertising?")
    return ""  # unreachable


def _scan_devices_command() -> str:
    """Read currently-known devices from `bluetoothctl devices`."""
    proc = subprocess.run(["bluetoothctl", "devices"], capture_output=True, text=True)
    for line in proc.stdout.splitlines():
        m = re.match(r"Device ([0-9A-Fa-f:]{17}) " + re.escape(NAME_PREFIX), line)
        if m:
            return m.group(1)
    return ""


# ---------------------------------------------------------------------------
# Cleanup — clean both sides so each test starts unpaired.
# ---------------------------------------------------------------------------

def clean_host_bond(addr: str) -> None:
    """Remove the BlueZ bond record for *addr* (no-op if absent)."""
    subprocess.run(["bluetoothctl", "remove", addr], capture_output=True, text=True)


def erase_pedal_nvs(esptool: pathlib.Path, port: str) -> None:
    """Wipe the NimBLE bond store on the pedal."""
    print("Erasing pedal NVS partition (NimBLE bond store)…")
    proc = subprocess.run(
        ["python3", str(esptool), "--port", port,
         "erase_region", NVS_OFFSET, NVS_SIZE],
        capture_output=True, text=True,
    )
    if proc.returncode != 0:
        print(proc.stdout); print(proc.stderr, file=sys.stderr)
        die("esptool erase_region failed")


def reflash(env_marker: str = "Reflashing production firmware + filesystem") -> None:
    """Run `make upload-esp32 && make uploadfs-esp32`."""
    print(env_marker + "…")
    for target in ("upload-esp32", "uploadfs-esp32"):
        print(f"  make {target}")
        proc = subprocess.run(["make", target], cwd=REPO_ROOT,
                              capture_output=True, text=True)
        if proc.returncode != 0:
            tail = "\n".join((proc.stdout + proc.stderr).splitlines()[-30:])
            print(tail, file=sys.stderr)
            die(f"`make {target}` failed (see tail above)")


# ---------------------------------------------------------------------------
# bluetoothctl driver — uses pexpect so timing tracks the actual prompts
# instead of fixed sleep()s.
# ---------------------------------------------------------------------------

class BluetoothCtl:
    """Long-running bluetoothctl session with a registered passkey-capable agent."""

    # ANSI-coloured prompt: ESC[…m[<context>]> ESC[0m
    # <context> changes from "bluetoothctl" → "AwesomeStudioPe" once a device
    # is selected, so match the full envelope rather than the literal name.
    PROMPT = re.compile(r"\x1b\[[\d;]*m\[.+?\]> \x1b\[0m")

    def __init__(self):
        self.proc = pexpect.spawn("bluetoothctl", encoding="utf-8", timeout=10)
        self.proc.expect(self.PROMPT)
        self.send("agent KeyboardOnly")
        self.send("default-agent")

    def send(self, line: str) -> None:
        self.proc.sendline(line)
        self.proc.expect(self.PROMPT)

    def expect_any(self, patterns, timeout: float):
        """Wait for any of *patterns* (regex strings). Returns the index matched."""
        return self.proc.expect(patterns, timeout=timeout)

    def sendline(self, line: str) -> None:
        self.proc.sendline(line)

    def quit(self) -> None:
        try:
            self.proc.sendline("quit")
            self.proc.expect(pexpect.EOF, timeout=3)
        except (pexpect.TIMEOUT, OSError):
            pass
        finally:
            self.proc.close(force=True)


def info_paired_bonded(addr: str) -> tuple[bool, bool]:
    """Read Paired/Bonded from `bluetoothctl info <addr>`."""
    proc = subprocess.run(["bluetoothctl", "info", addr],
                          capture_output=True, text=True)
    paired = re.search(r"^\s*Paired:\s*yes\s*$", proc.stdout, re.MULTILINE) is not None
    bonded = re.search(r"^\s*Bonded:\s*yes\s*$", proc.stdout, re.MULTILINE) is not None
    return paired, bonded


# ---------------------------------------------------------------------------
# Test scenarios
# ---------------------------------------------------------------------------

def test_pair_correct_passkey(addr: str) -> None:
    print("\n[Test 1] Pair with correct passkey →")
    bt = BluetoothCtl()
    try:
        bt.sendline(f"pair {addr}")
        # The passkey prompt arrives mid-pairing; "Pairing successful" / failure
        # arrives at the end. Wait for whichever comes first.
        idx = bt.expect_any(
            [r"Enter passkey \(number in 0-999999\):",
             r"Pairing successful",
             r"Failed to pair",
             pexpect.TIMEOUT],
            timeout=PAIR_TIMEOUT,
        )
        if idx == 0:
            bt.sendline(EXPECTED_PASSKEY)
            idx = bt.expect_any(
                [r"Pairing successful", r"Failed to pair", pexpect.TIMEOUT],
                timeout=PAIR_TIMEOUT,
            )
            report("Passkey prompt appeared", True)
            report("Pairing succeeded with correct passkey", idx == 0,
                   "expected 'Pairing successful'")
        elif idx == 1:
            # No passkey prompt and pairing succeeded → MITM never asserted.
            report("Passkey prompt appeared", False,
                   "pairing went Just-Works (TASK-246 regression?)")
        else:
            report("Pairing succeeded with correct passkey", False, "timeout or error")
    finally:
        bt.quit()

    paired, bonded = info_paired_bonded(addr)
    report("bluetoothctl info: Paired: yes", paired)
    report("bluetoothctl info: Bonded: yes", bonded)


def test_reconnect_reuses_bond(addr: str) -> None:
    print("\n[Test 2] Reconnect reuses bond (no second passkey prompt) →")
    # disconnect, then connect, watch for the absence of a passkey prompt.
    subprocess.run(["bluetoothctl", "disconnect", addr], capture_output=True, text=True)
    time.sleep(2)

    bt = BluetoothCtl()
    try:
        bt.sendline(f"connect {addr}")
        idx = bt.expect_any(
            [r"Enter passkey \(number in 0-999999\):",
             r"Connection successful",
             r"Failed to connect",
             pexpect.TIMEOUT],
            timeout=RECONNECT_TIMEOUT,
        )
        if idx == 0:
            report("Reconnect did not re-prompt for passkey", False,
                   "BlueZ asked for passkey again — bond not honoured")
            bt.sendline(EXPECTED_PASSKEY)  # so the session ends cleanly
        elif idx == 1:
            report("Reconnect did not re-prompt for passkey", True)
        else:
            report("Reconnect succeeded", False, "timeout or connect error")
    finally:
        bt.quit()


def test_pair_wrong_passkey(addr: str, esptool: pathlib.Path, port: str) -> None:
    print("\n[Test 3] Pair with wrong passkey is rejected →")
    # Both sides need to forget the previous bond so this attempt is fresh.
    clean_host_bond(addr)
    erase_pedal_nvs(esptool, port)
    # Wait for advertising to come back after the reset, then re-discover so
    # BlueZ knows the device exists again — `bluetoothctl remove` purges its
    # cache and `pair` would otherwise return "Device not available".
    time.sleep(5)
    discover_pedal()

    bt = BluetoothCtl()
    try:
        bt.sendline(f"pair {addr}")
        idx = bt.expect_any(
            [r"Enter passkey \(number in 0-999999\):",
             r"Pairing successful",
             r"Failed to pair",
             pexpect.TIMEOUT],
            timeout=PAIR_TIMEOUT,
        )
        if idx == 0:
            bt.sendline(WRONG_PASSKEY)
            idx = bt.expect_any(
                [r"Pairing successful",
                 r"Failed to pair",
                 r"Authentication Failed",
                 pexpect.TIMEOUT],
                timeout=PAIR_TIMEOUT,
            )
            outcomes = ["Pairing successful (BAD)", "Failed to pair",
                        "Authentication Failed", "(timeout)"]
            report("Wrong passkey rejected (got: " + outcomes[idx] + ")",
                   idx != 0)
        elif idx == 1:
            report("Passkey prompt appeared (negative test)", False,
                   "pairing went Just-Works")
        else:
            report("Wrong passkey rejected", False, "no clear outcome")
    finally:
        bt.quit()

    paired, bonded = info_paired_bonded(addr)
    report("After wrong passkey: Paired: no", not paired)
    report("After wrong passkey: Bonded: no", not bonded)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[1])
    ap.add_argument("--port", default="/dev/ttyUSB0",
                    help="ESP32 serial port (default: /dev/ttyUSB0)")
    ap.add_argument("--skip-flash", action="store_true",
                    help="Skip the make upload step (useful when iterating)")
    args = ap.parse_args()

    esptool = preflight(args.port)

    # Always start from a known-clean state so test outcomes are reproducible.
    print("Cleaning prior bonds…")
    # Best-effort: device may not exist yet on the host side.
    addr_hint = _scan_devices_command()
    if addr_hint:
        clean_host_bond(addr_hint)

    erase_pedal_nvs(esptool, args.port)

    if not args.skip_flash:
        reflash()
    else:
        print("Skipping reflash (--skip-flash).")

    # After flash + NVS erase the device reboots; give it time to advertise.
    print("Waiting for pedal to advertise…")
    time.sleep(5)

    addr = discover_pedal()

    try:
        test_pair_correct_passkey(addr)
        test_reconnect_reuses_bond(addr)
        test_pair_wrong_passkey(addr, esptool, args.port)
    finally:
        # Teardown: leave the host clean regardless of test outcome.
        print("\nTeardown: removing host bond…")
        clean_host_bond(addr)

    print("\n" + "=" * 50)
    total = len(PASSED) + len(FAILED)
    print(f"Results: {len(PASSED)}/{total} passed")
    if FAILED:
        for f in FAILED:
            print(f"  FAILED: {f}")
        print("OVERALL: FAIL")
        return 1
    print("OVERALL: PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
