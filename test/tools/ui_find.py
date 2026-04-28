#!/usr/bin/env python3
"""Find UI element centers in a uiautomator dump.

Usage:
    python3 scripts/ui_find.py <dump.xml> "<query>" [--all]

Query is matched (case-insensitive substring) against text and
content-desc attributes. Prints "<x> <y>  <text> | <content-desc>"
for the first match (or all matches with --all).
"""
from __future__ import annotations

import argparse
import re
import sys


NODE_RE = re.compile(r"<node\b[^>]+>", re.DOTALL)
ATTR_RE = re.compile(r'(text|content-desc)="([^"]*)"', re.DOTALL)
BOUNDS_RE = re.compile(r'bounds="\[(\d+),(\d+)\]\[(\d+),(\d+)\]"')


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("query")
    ap.add_argument("--all", action="store_true")
    args = ap.parse_args()

    with open(args.dump) as f:
        xml = f.read()

    needle = args.query.lower()
    found = 0
    for m in NODE_RE.finditer(xml):
        node = m.group(0)
        attrs = dict(ATTR_RE.findall(node))
        text = attrs.get("text", "")
        cd = attrs.get("content-desc", "")
        if needle not in text.lower() and needle not in cd.lower():
            continue
        b = BOUNDS_RE.search(node)
        if not b:
            continue
        x1, y1, x2, y2 = map(int, b.groups())
        cx, cy = (x1 + x2) // 2, (y1 + y2) // 2
        print(f"{cx} {cy}  {text!r} | {cd!r}")
        found += 1
        if not args.all:
            break

    if found == 0:
        print(f"no match for {args.query!r}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
