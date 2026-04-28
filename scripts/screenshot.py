#!/usr/bin/env python3
"""Capture an Android screenshot via adb and resize so the longest edge fits
within MAX_DIM, keeping the PNG well below the 2000 px many-image limit.

Usage:
    python3 scripts/screenshot.py [out_path] [--max 1600]

Default: writes to /tmp/asp_shot.png with longest side resized to 1600 px.
"""
from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from PIL import Image

MAX_DIM = 1600


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("out", nargs="?", default="/tmp/asp_shot.png")
    parser.add_argument("--max", type=int, default=MAX_DIM)
    parser.add_argument("--serial", default=None, help="adb device serial")
    args = parser.parse_args()

    if not shutil.which("adb"):
        print("adb not found in PATH", file=sys.stderr)
        return 1

    cmd = ["adb"]
    if args.serial:
        cmd += ["-s", args.serial]
    cmd += ["exec-out", "screencap", "-p"]

    with tempfile.NamedTemporaryFile(suffix=".png", delete=False) as raw:
        raw_path = Path(raw.name)
        proc = subprocess.run(cmd, stdout=raw, check=False)
        if proc.returncode != 0:
            print("adb screencap failed", file=sys.stderr)
            return proc.returncode

    img = Image.open(raw_path)
    longest = max(img.size)
    if longest > args.max:
        scale = args.max / longest
        new_size = (int(img.size[0] * scale), int(img.size[1] * scale))
        img = img.resize(new_size, Image.LANCZOS)

    out_path = Path(args.out)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    img.save(out_path, optimize=True)
    raw_path.unlink(missing_ok=True)
    print(f"{out_path}  {img.size[0]}x{img.size[1]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
