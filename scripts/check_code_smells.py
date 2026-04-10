#!/usr/bin/env python3
"""
Code smell detector for AwesomeGuitarPedal C++ sources.

Checks for common C++ anti-patterns that clang-tidy does not cover:
  - Functions longer than MAX_FUNCTION_LINES lines  (.cpp only)
  - .cpp files longer than MAX_CPP_FILE_LINES lines
  - Deeply nested blocks (more than MAX_NESTING_DEPTH levels)
  - TODO/FIXME/HACK/XXX comments left in source

Exit code: 0 = clean, 1 = smells found.
"""

import re
import sys
from pathlib import Path

# ---- Thresholds ----
MAX_FUNCTION_LINES = 60
MAX_CPP_FILE_LINES = 400
MAX_NESTING_DEPTH = 4

# ---- Source roots ----
SOURCE_DIRS = ["lib", "src"]
EXTENSIONS = {".cpp", ".h", ".hpp"}
EXCLUDE_PATTERNS = [".pio", "build", "_deps", "fakes", "arduino_shim"]

SMELL_COMMENTS = re.compile(r"\b(TODO|FIXME|HACK|XXX)\b")


def should_exclude(path: Path) -> bool:
    return any(pat in str(path) for pat in EXCLUDE_PATTERNS)


def collect_sources(root: Path) -> list[Path]:
    files = []
    for d in SOURCE_DIRS:
        src = root / d
        if src.exists():
            for f in src.rglob("*"):
                if f.suffix in EXTENSIONS and not should_exclude(f):
                    files.append(f)
    return sorted(files)


def check_file(path: Path) -> list[str]:
    issues = []
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    is_cpp = path.suffix == ".cpp"

    if is_cpp and len(lines) > MAX_CPP_FILE_LINES:
        issues.append(
            f"{path}:1: file has {len(lines)} lines (max {MAX_CPP_FILE_LINES})"
        )

    nesting = 0
    func_start = None
    func_line_count = 0
    in_function = False
    prev_nonempty = ""

    CONTAINER_RE = re.compile(r"^\s*(namespace|class|struct)\b")

    for lineno, line in enumerate(lines, 1):
        stripped = line.strip()

        # Smell comments — check both inside and outside block comments
        if SMELL_COMMENTS.search(stripped):
            m = SMELL_COMMENTS.search(stripped)
            issues.append(
                f"{path}:{lineno}: smell comment '{m.group()}' found"
            )

        # Track nesting depth (skip string literals — rough approximation)
        opens = stripped.count("{")
        closes = stripped.count("}")
        nesting += opens - closes

        if nesting > MAX_NESTING_DEPTH:
            issues.append(
                f"{path}:{lineno}: nesting depth {nesting} exceeds {MAX_NESTING_DEPTH}"
            )

        # Function length — only for .cpp files and only top-level bodies.
        # Skip namespace/class/struct blocks — they are containers, not functions.
        # The opening brace may appear on the same line as the keyword or on the
        # next non-empty line (Allman style), so we check both current and previous.
        is_container = CONTAINER_RE.match(line) or CONTAINER_RE.match(prev_nonempty)
        if is_cpp:
            if opens > closes and nesting == 1 and not in_function and not is_container:
                in_function = True
                func_start = lineno
                func_line_count = 1
            elif in_function:
                func_line_count += 1
                if closes > opens and nesting == 0:
                    if func_line_count > MAX_FUNCTION_LINES:
                        issues.append(
                            f"{path}:{func_start}: function/block body has "
                            f"{func_line_count} lines (max {MAX_FUNCTION_LINES})"
                        )
                    in_function = False
                    func_line_count = 0

        if stripped:
            prev_nonempty = line

    return issues


def main() -> int:
    root = Path(__file__).parent.parent
    sources = collect_sources(root)

    if not sources:
        print("No source files found.")
        return 0

    all_issues: list[str] = []
    for path in sources:
        all_issues.extend(check_file(path))

    if all_issues:
        print(f"Code smell check — {len(all_issues)} issue(s) found:\n")
        for issue in all_issues:
            print(f"  {issue}")
        print()
        return 1

    print(f"Code smell check — {len(sources)} file(s) checked, no issues.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
