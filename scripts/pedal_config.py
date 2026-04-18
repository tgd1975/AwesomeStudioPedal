#!/usr/bin/env python3
"""pedal_config.py — CLI tool for scanning, uploading, and validating pedal configs over BLE."""

import argparse
import asyncio
import json
import pathlib
import struct
import sys

REPO_ROOT = pathlib.Path(__file__).resolve().parent.parent
PROFILES_SCHEMA = REPO_ROOT / "data" / "profiles.schema.json"
CONFIG_SCHEMA   = REPO_ROOT / "data" / "config.schema.json"

# UUIDs from docs/developers/BLE_CONFIG_PROTOCOL.md
SERVICE_UUID       = "516515c0-4b50-447b-8ca3-cbfce3f4d9f8"
CHAR_WRITE_UUID    = "516515c1-4b50-447b-8ca3-cbfce3f4d9f8"
CHAR_WRITE_HW_UUID = "516515c2-4b50-447b-8ca3-cbfce3f4d9f8"
CHAR_STATUS_UUID   = "516515c3-4b50-447b-8ca3-cbfce3f4d9f8"

MTU          = 512
PAYLOAD_SIZE = MTU - 2   # 510 bytes
END_SEQ      = 0xFFFF


# ---------------------------------------------------------------------------
# Chunking helper (reusable by runner.py in test suite)
# ---------------------------------------------------------------------------

def make_chunks(data: bytes) -> list[bytes]:
    """Split *data* into MTU-sized BLE packets with 2-byte big-endian sequence numbers."""
    chunks = []
    seq = 0
    offset = 0
    while offset < len(data):
        payload = data[offset:offset + PAYLOAD_SIZE]
        chunks.append(struct.pack(">H", seq) + payload)
        seq += 1
        offset += PAYLOAD_SIZE
    # End-of-transfer sentinel: seq=0xFFFF, no payload
    chunks.append(struct.pack(">H", END_SEQ))
    return chunks


# ---------------------------------------------------------------------------
# Subcommand: validate
# ---------------------------------------------------------------------------

def cmd_validate(args: argparse.Namespace) -> int:
    try:
        import jsonschema
    except ImportError:
        print("ERROR: jsonschema not installed. Run: pip install jsonschema", file=sys.stderr)
        return 1

    json_path = pathlib.Path(args.file)
    if not json_path.exists():
        print(f"ERROR: file not found: {json_path}", file=sys.stderr)
        return 1

    schema_path = CONFIG_SCHEMA if args.hw else PROFILES_SCHEMA
    if not schema_path.exists():
        print(f"ERROR: schema not found: {schema_path}", file=sys.stderr)
        return 1

    try:
        data   = json.loads(json_path.read_text())
        schema = json.loads(schema_path.read_text())
    except json.JSONDecodeError as exc:
        print(f"ERROR: JSON parse error in {json_path}: {exc}", file=sys.stderr)
        return 1

    validator = jsonschema.Draft7Validator(schema)
    errors = list(validator.iter_errors(data))
    if errors:
        for err in errors:
            path = " → ".join(str(p) for p in err.absolute_path) or "(root)"
            print(f"  {path}: {err.message}")
        print(f"FAIL: {len(errors)} schema error(s) in {json_path}")
        return 1

    print(f"OK: {json_path} is valid")
    return 0


# ---------------------------------------------------------------------------
# Subcommand: scan
# ---------------------------------------------------------------------------

async def _scan() -> int:
    try:
        from bleak import BleakScanner
    except ImportError:
        print("ERROR: bleak not installed. Run: pip install bleak", file=sys.stderr)
        return 1

    print("Scanning for BLE devices (5 s)…")
    devices = await BleakScanner.discover(timeout=5.0)
    found = False
    for d in devices:
        uuids = [str(u).lower() for u in (d.metadata.get("uuids") or [])]
        marker = " ← pedal" if SERVICE_UUID.lower() in uuids else ""
        print(f"  {d.address}  RSSI={d.rssi:4d}  {d.name or '(unknown)'}{marker}")
        if marker:
            found = True
    if not found:
        print("  (no pedal device advertising the Config service found)")
    return 0


def cmd_scan(args: argparse.Namespace) -> int:
    return asyncio.run(_scan())


# ---------------------------------------------------------------------------
# Subcommand: upload / upload-config
# ---------------------------------------------------------------------------

async def _upload(file_path: pathlib.Path, char_uuid: str) -> int:
    try:
        from bleak import BleakScanner, BleakClient
    except ImportError:
        print("ERROR: bleak not installed. Run: pip install bleak", file=sys.stderr)
        return 1

    # Read and chunk payload
    try:
        payload = file_path.read_bytes()
    except OSError as exc:
        print(f"ERROR: cannot read {file_path}: {exc}", file=sys.stderr)
        return 1

    chunks = make_chunks(payload)
    print(f"Payload: {len(payload)} bytes → {len(chunks)} chunks (incl. sentinel)")

    # Discover pedal
    print("Scanning for pedal…")
    device = await BleakScanner.find_device_by_filter(
        lambda d, adv: SERVICE_UUID.lower() in [str(u).lower() for u in (adv.service_uuids or [])],
        timeout=10.0,
    )
    if device is None:
        print("ERROR: no pedal device found", file=sys.stderr)
        return 1

    print(f"Found: {device.name} ({device.address})")

    status_received: list[str] = []

    def on_notify(_char, data: bytearray) -> None:
        status_received.append(data.decode("utf-8", errors="replace"))

    async with BleakClient(device) as client:
        await client.start_notify(CHAR_STATUS_UUID, on_notify)

        print(f"Uploading {len(chunks)} packets…")
        for i, chunk in enumerate(chunks):
            await client.write_gatt_char(char_uuid, chunk, response=False)
            if (i + 1) % 10 == 0:
                print(f"  {i + 1}/{len(chunks)}")

        # Wait up to 10 s for status notification
        for _ in range(100):
            if status_received:
                break
            await asyncio.sleep(0.1)

        await client.stop_notify(CHAR_STATUS_UUID)

    if not status_received:
        print("ERROR: no CONFIG_STATUS notification received", file=sys.stderr)
        return 1

    status = status_received[-1]
    if status == "OK":
        print(f"OK: upload successful")
        return 0
    else:
        print(f"ERROR: pedal responded: {status}", file=sys.stderr)
        return 1


def cmd_upload(args: argparse.Namespace) -> int:
    char = CHAR_WRITE_HW_UUID if getattr(args, "hw", False) else CHAR_WRITE_UUID
    return asyncio.run(_upload(pathlib.Path(args.file), char))


# ---------------------------------------------------------------------------
# Argument parser
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="pedal_config",
        description="AwesomeStudioPedal BLE config tool",
    )
    sub = parser.add_subparsers(dest="command", required=True)

    # scan
    sub.add_parser("scan", help="List nearby BLE devices advertising the pedal service")

    # upload
    up = sub.add_parser("upload", help="Upload profiles.json to pedal via BLE")
    up.add_argument("file", help="Path to profiles.json")

    # upload-config
    upc = sub.add_parser("upload-config", help="Upload config.json (hardware config) to pedal")
    upc.add_argument("file", help="Path to config.json")

    # validate
    val = sub.add_parser("validate", help="Validate JSON file against schema (no BLE needed)")
    val.add_argument("file", help="Path to JSON file")
    val.add_argument("--hw", action="store_true", help="Validate against config.schema.json instead")

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    if args.command == "scan":
        return cmd_scan(args)
    elif args.command == "upload":
        args.hw = False
        return cmd_upload(args)
    elif args.command == "upload-config":
        args.hw = True
        return cmd_upload(args)
    elif args.command == "validate":
        return cmd_validate(args)
    else:
        parser.print_help()
        return 1


if __name__ == "__main__":
    sys.exit(main())
