---
name: doc-check
description: After creating or moving any `.md` file under `docs/` (excluding `docs/developers/tasks/` and `docs/developers/ideas/`), invoke this skill to validate persona placement (builder / musician / developer). On a Mismatch verdict, the skill proposes a `git mv` and waits for user confirmation before running it.
---

# doc-check

The user invokes this as `/doc-check`, but the agent should also invoke
it automatically after creating or moving any `.md` file under `docs/`
(excluding `docs/developers/tasks/` and `docs/developers/ideas/`, which
are internal scaffolding with their own conventions).

## Purpose

Enforces the three-persona documentation structure by analyzing `.md` files changed relative to `main` and suggesting correct persona directories. On a clear Mismatch, offers to move the file with a proposed `git mv` command — but never moves silently.

## Workflow

1. **Identify changed files**: Run `git diff --name-only main...HEAD -- '*.md'` to get list of modified documentation files
2. **Exclude internal files**: Filter out `docs/developers/tasks/` (internal scaffolding)
3. **Classify files**: For each file, determine persona based on path and content analysis
4. **Report results**: Show current vs. suggested persona with confidence level and reasoning

## Implementation Details

### File Classification Algorithm

1. **Path-based classification** (primary):
   - `docs/builders/` → Builder persona
   - `docs/musicians/` → Musician persona
   - `docs/developers/` → Developer persona

2. **Content-based validation** (secondary):
   - Analyze file content for persona-specific keywords
   - Calculate confidence score based on keyword matches

### Content Heuristics

**Builder keywords**:

- "solder", "wiring", "GPIO", "breadboard", "schematic", "enclosure", "hardware", "build", "assemble", "circuit", "resistor", "LED", "button wiring", "pin assignment"

**Musician keywords**:

- "press button", "profile", "Bluetooth", "footswitch", "pedal", "play", "perform", "use", "operate", "select profile", "change profile", "connect", "pair", "battery", "power on"

**Developer keywords**:

- "CMake", "PR", "architecture", "clang", "API", "contributing", "code", "program", "compile", "build system", "pull request", "merge", "branch", "repository", "version control", "debug", "test", "unit test", "integration"

### Confidence Levels

- **High**: 3+ persona-specific keywords + correct path
- **Medium**: 1-2 persona-specific keywords or mixed signals
- **Low**: No clear keywords or conflicting signals
- **Mismatch**: Keywords suggest different persona than path

## Output Format

```
Documentation Persona Check
============================

File: [filepath]
Current persona: [builder|musician|developer] (from path)
Suggested persona: [builder|musician|developer] (from content)
Confidence: [High|Medium|Low|Mismatch]
Reason: [brief explanation with keyword examples]

[Repeat for each file]

Summary:
- Total files checked: X
- Correctly placed: Y
- Needs review: Z
```

## Example Output

```
Documentation Persona Check
============================

File: docs/builders/user-guide.md
Current persona: builder
Suggested persona: musician
Confidence: Mismatch
Reason: Contains musician keywords: "press button", "profile", "Bluetooth"

File: docs/developers/hardware-setup.md  
Current persona: developer
Suggested persona: builder
Confidence: High
Reason: Contains builder keywords: "wiring", "GPIO", "circuit"

File: docs/musicians/advanced-features.md
Current persona: musician
Suggested persona: musician
Confidence: High
Reason: Contains musician keywords: "profile", "footswitch", "pedal"

Summary:
- Total files checked: 3
- Correctly placed: 1
- Needs review: 2
```

## Edge Cases

1. **No documentation changes**: "No documentation changes to check."
2. **Mixed persona content**: Flag for manual review with both personas suggested
3. **Unclear persona**: Suggest manual review with low confidence
4. **Files in wrong directories**: Suggest correct path based on content analysis

## Skill Execution

The skill performs these steps:

1. **Get changed files**: `git diff --name-only main...HEAD -- '*.md'`
2. **Filter files**: Exclude `docs/developers/tasks/` directory
3. **Check for empty result**: If no files, output "No documentation changes to check."
4. **Process each file**:
   - Determine path-based persona
   - Read file content
   - Count persona-specific keywords
   - Calculate confidence and suggested persona
5. **Generate report**: Format output as shown above

## Move offer (on Mismatch)

For each file with verdict `Mismatch`:

1. Compute the proposed target path by replacing the current persona
   segment in the path with the suggested persona. For example,
   `docs/builders/user-guide.md` with suggested persona `musician`
   becomes `docs/musicians/user-guide.md`.

2. Print the proposed move on its own line, formatted so the user can
   copy-paste or accept it:

   ```
   Proposed move:
     git mv docs/builders/user-guide.md docs/musicians/user-guide.md
   Reason: content keywords match musician (press button, profile, Bluetooth)
   ```

3. Ask the user: "Run this move?" Wait for explicit confirmation.

4. **On approval**, run `git mv <old> <new>`. Report the new path.

5. **On refusal or no answer**, do nothing — the file stays put. The
   verdict remains in the report for the user's record.

Do not run the move silently. Do not batch-move multiple files in one
prompt — ask file-by-file so the user can approve each independently.

## Notes

- The skill is no longer purely advisory — it offers to move on Mismatch
  but only with explicit user approval per move.
- Manual review is still required for final decisions on Low / Medium
  confidence verdicts.
- Consider running this skill as part of PR validation or pre-commit hooks
