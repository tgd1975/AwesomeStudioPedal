# doc-check

The user invokes this as `/doc-check` to validate documentation persona placement.

## Purpose

Enforces the three-persona documentation structure by analyzing `.md` files changed relative to `main` and suggesting correct persona directories.

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

## Notes

- This is an advisory tool only - it does not move or rename files
- The skill helps maintain documentation organization but doesn't enforce it
- Manual review is still required for final decisions
- Consider running this skill as part of PR validation or pre-commit hooks
