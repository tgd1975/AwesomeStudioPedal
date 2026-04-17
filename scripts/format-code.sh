#!/usr/bin/env bash
# Format all C++ files with clang-format
# Usage: ./format-code.sh

set -euo pipefail

# Find all C++ files (excluding .pio and build directories)
FILES=$(find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | grep -v \.pio | grep -v build | grep -v test)

echo "Formatting C++ files:"
echo "$FILES"

if [ -n "$FILES" ]; then
    if command -v clang-format >/dev/null 2>&1; then
        clang-format -i $FILES
        echo "Formatting complete!"
    else
        echo "clang-format not found. Please install it first."
        exit 1
    fi
else
    echo "No C++ files found to format"
fi
