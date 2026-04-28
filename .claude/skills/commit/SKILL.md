---
name: commit
description: Stage user-named files only and commit, applying the CLAUDE.md "Pre-commit hook failures on unrelated changes" protocol when the hook fails. Never adds --no-verify silently — explicit user approval is required.
---

# commit

Invoked as `/commit "<message>" <file> [<file> …]`. The skill exists to
make the project's commit protocol — encoded in CLAUDE.md under
"Parallel sessions" and "Pre-commit hook failures on unrelated changes"
— consistent and auditable instead of relying on prose-only guidance.

## Steps

1. **Stage only the named files.** Per the project's
   "Parallel sessions — commit only your own work" rule:

   ```bash
   git add -- <file> [<file> …]
   ```

   Never use `git add -A` / `git add .`. If the user passed no files,
   stop and ask which files belong to this commit.

2. **Attempt the commit** with the message via heredoc, ending with
   the standard `Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>`
   trailer (omit the trailer if the user is committing a non-agent
   change manually):

   ```bash
   git commit -m "$(cat <<'EOF'
   <commit message>

   Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>
   EOF
   )"
   ```

3. **On hook success** — report the new commit's short hash + subject
   (one line). Done.

4. **On hook failure** — do **not** retry, do **not** add `--no-verify`
   silently. Run the three CLAUDE.md checks and present a structured
   message to the user before proposing a bypass:

   - **Staged files only** — are *all* staged files unrelated to the
     hook failure? (e.g. only `.md` files staged, but C++ tests fail.)
     Identify the failing check from the hook's stderr; map it to a
     file class (`*.cpp`/`*.h` for clang-format / unit tests,
     `*.md` for markdownlint, etc.). Compare to the staged list.
   - **Pre-existing breakage** — is the failing check broken on `main`
     or in the working tree already, not caused by this commit? Run
     the failing check against `HEAD` (or `main` if quick) to confirm.
   - **No silent regression** — would bypassing hide a real regression
     introduced by the staged changes? If the staged files plausibly
     could affect the failing check (even indirectly via includes /
     imports / generated code), the answer is "yes, fix it" — do not
     bypass.

5. **If all three checks pass**, present the standard message verbatim:

   > The pre-commit hook failed, but the failure is in `<file/check>`
   > which is unrelated to the staged files (`<list staged files>`).
   > This appears to be a pre-existing issue. It may be OK to bypass
   > the hook for this commit with `--no-verify`. Do you want me to
   > proceed with `--no-verify`, or fix the hook failure first?

   Wait for explicit user approval. On approval, retry with
   `git commit --no-verify -m …`. On refusal, stop — the user will
   fix the hook failure first.

6. **If any of the three checks fails**, do **not** offer `--no-verify`.
   Report which check failed, surface the relevant hook output, and
   stop — the user diagnoses or fixes.

## When NOT to use

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
