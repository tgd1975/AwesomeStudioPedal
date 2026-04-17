---
name: format
description: Auto-format all C++ files with clang-format and report what changed
---

# format

Run clang-format across all C++ sources and report which files were modified.

Steps:

1. Record the current git status of C++ files:

   ```bash
   git diff --name-only -- '*.cpp' '*.h' '*.hpp'
   ```

2. Run:

   ```bash
   make format
   ```

3. Record the git status again and compute the diff to find which files were changed by
   the formatter.

4. Report:
   - Files reformatted (if any), one per line
   - "All C++ files already formatted." if nothing changed

5. If files were reformatted, remind the user to review and stage the changes before
   committing. The pre-commit hook will re-run clang-format on staged files automatically.

Do not stage or commit — leave that to the user.
