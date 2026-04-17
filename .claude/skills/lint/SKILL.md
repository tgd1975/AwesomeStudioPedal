---
name: lint
description: Run all local linters — markdown, code smells, and Mermaid diagrams
---

# lint

Run all local linters and report findings. This mirrors the checks performed by CI.

Steps:

1. **Markdown** — run `make lint-markdown` (auto-fixes issues in place via markdownlint-cli2).
   Report which files were changed, if any.

2. **Code smells** — run `python scripts/check_code_smells.py`.
   Report any functions exceeding 60 lines, files exceeding 400 lines,
   nesting depth beyond 4 levels, or leftover TODO/FIXME/HACK/XXX comments.

3. **Mermaid diagrams** — run `python scripts/validate_mermaid.py`.
   Report any diagrams that fail validation.

After all three checks, print a summary:

```
Markdown:   OK  (or: N files fixed)
Code smells: OK  (or: list of issues)
Mermaid:    OK  (or: list of failures)
```

If markdown auto-fixed files, remind the user to review and stage the changes.
Do not run clang-format or clang-tidy — those require a connected device build environment.
