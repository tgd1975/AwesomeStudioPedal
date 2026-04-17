---
name: clang-tidy
description: Run clang-tidy on C++ files changed relative to main and report violations
---

# clang-tidy

Run clang-tidy on every C++ file that differs from `main` and report violations with
file:line context.

Steps:

1. Check that `.vscode/build/compile_commands.json` exists. If not, tell the user to run
   `cmake -B .vscode/build` first and stop.

2. Find changed C++ files:

   ```bash
   git diff --name-only main...HEAD -- '*.cpp' '*.h' '*.hpp'
   ```

   Also include any untracked files in `src/`, `lib/`, or `include/` that are not in
   `.pio/`, `build/`, or `_deps/`.

   If no C++ files are changed, report "No C++ files changed relative to main." and stop.

3. For each file, run:

   ```bash
   clang-tidy -p .vscode/build <file>
   ```

4. Collect all output. Report:
   - A summary line per file: OK or the number of warnings/errors
   - Full violation details (file:line:col: warning/error: message [check-name]) for
     any file with findings
   - An overall count: N files checked, M violations found

If clang-tidy is not installed, report the error and stop.
Do not auto-fix — report only.
