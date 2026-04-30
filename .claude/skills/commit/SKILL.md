---
name: commit
description: Commit user-named files atomically via git's pathspec form (no staging step), applying the CLAUDE.md "Pre-commit hook failures on unrelated changes" protocol when the hook fails. Never adds --no-verify silently — explicit user approval is required.
---

# commit

Invoked as `/commit "<message>" <file> [<file> …]`. The skill exists to
make the project's commit protocol — encoded in CLAUDE.md under
"Parallel sessions" and "Pre-commit hook failures on unrelated changes"
— consistent and auditable instead of relying on prose-only guidance.

## Atomicity property — why pathspec form is mandatory

This skill **always** commits via git's pathspec form:

```bash
git commit -m "..." -- <file> [<file> …]
```

It **never** uses the two-step `git add <files> && git commit -m "..."`.
The reason is parallel-session safety:

- Pathspec form builds a **temporary index** containing only the named
  files, runs the pre-commit hook against that temporary index, and
  only updates the real index if the commit succeeds. Hook failure
  leaves the real index untouched.
- Two-step form mutates the real index *before* the hook runs. If the
  hook fails, the named files stay staged in the shared index — and
  any foreign files staged by a parallel session are now mixed in
  with them. The next commit attempt (yours or theirs) starts dirty.

In a repo where multiple Claude Code sessions run concurrently, this
is the difference between "your hook failure is your problem" and
"your hook failure clobbers another session's staged work". Pathspec
keeps blast radius scoped to the single commit attempt.

## Steps

1. **Validate input.** If the user passed no files, stop and ask which
   files belong to this commit. Never sweep with `-A` / `.` / `-u`.
   Per the project's "Parallel sessions — commit only your own work"
   rule, the file list must be explicit.

2. **Attempt the commit in pathspec form** with the message via
   heredoc, ending with the standard
   `Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>` trailer
   (omit the trailer if the user is committing a non-agent change
   manually):

   ```bash
   git commit -m "$(cat <<'EOF'
   <commit message>

   Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>
   EOF
   )" -- <file> [<file> …]
   ```

   The `--` separates the message from the pathspec list. Files
   already staged for this commit are still committed; files that are
   only staged but not in the pathspec list are *not* — they stay
   staged for a future commit. Files not staged at all are committed
   straight from the working tree if they appear in the pathspec.

   Do **not** run `git add` first. Pathspec form replaces it.

3. **On hook success** — report the new commit's short hash + subject
   (one line). Done.

4. **On hook failure** — do **not** retry, do **not** add `--no-verify`
   silently. Pathspec form leaves the real index untouched, so any
   foreign staged files from parallel sessions are still where they
   were before; do not "clean up" `git status` between failure and
   retry. Run the three CLAUDE.md checks and present a structured
   message to the user before proposing a bypass:

   - **Pathspec files only** — are *all* files in the pathspec list
     unrelated to the hook failure? (e.g. only `.md` files in this
     commit, but C++ tests fail.) Identify the failing check from
     the hook's stderr; map it to a file class (`*.cpp`/`*.h` for
     clang-format / unit tests, `*.md` for markdownlint, etc.).
     Compare to the pathspec list — *not* to `git status`, since the
     real index is not touched in pathspec mode.
   - **Pre-existing breakage** — is the failing check broken on `main`
     or in the working tree already, not caused by this commit? Run
     the failing check against `HEAD` (or `main` if quick) to confirm.
   - **No silent regression** — would bypassing hide a real regression
     introduced by the pathspec changes? If the pathspec files
     plausibly could affect the failing check (even indirectly via
     includes / imports / generated code), the answer is "yes, fix
     it" — do not bypass.

5. **If all three checks pass**, present the standard message verbatim:

   > The pre-commit hook failed, but the failure is in `<file/check>`
   > which is unrelated to the files in this commit
   > (`<list pathspec files>`). This appears to be a pre-existing
   > issue. It may be OK to bypass the hook for this commit with
   > `--no-verify`. Do you want me to proceed with `--no-verify`, or
   > fix the hook failure first?

   Wait for explicit user approval. On approval, retry with the same
   pathspec form plus `--no-verify`:

   ```bash
   git commit --no-verify -m "..." -- <file> [<file> …]
   ```

   On refusal, stop — the user will fix the hook failure first.

6. **If any of the three checks fails**, do **not** offer `--no-verify`.
   Report which check failed, surface the relevant hook output, and
   stop — the user diagnoses or fixes.

## When NOT to use

- **Partial-hunk commits** (committing some hunks of a file but not
  others). Pathspec form has no equivalent of `git add -p`; the whole
  file is committed. Partial-hunk policy is decided in TASK-327. For
  now, split the file or commit the whole change.
- Multi-step commits where each commit needs human review of the
  message and contents (use `git commit` interactively).
- Amending an existing commit. The CLAUDE.md "Git Safety Protocol"
  (system-prompt level) prefers new commits over amends.
- Committing on `main`. `/check-branch` should fire first; this skill
  does not bypass that.

## Skill registration

Registered in [.vibe/config.toml](../../../.vibe/config.toml)'s
`enabled_skills` list per the project's CLAUDE.md skill-registration
rule.
