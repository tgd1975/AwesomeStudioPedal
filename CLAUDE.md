# Project: AwesomeStudioPedal

## OS context

This project is developed on both **Windows 11** and **Ubuntu**. At the start of every session, check the platform from the system environment info (or run `uname -s`) and apply the correct shell syntax:

| Situation | Rule |
|---|---|
| Windows 11 (Git Bash / MSYS2) | Use Unix shell syntax (bash). Absolute paths use forward slashes: `C:/Users/...`. No `cmd.exe` commands (`dir`, `type`, `copy`, `del`). |
| Ubuntu / Linux | Standard bash. `/dev/null`, `apt`, etc. all work as expected. |

When in doubt, run `/os-context` to detect and announce the current OS before issuing shell commands.

## Missing executables

When a CLI tool or executable is not found (e.g. `markdownlint`, `clang-tidy`, `jq`):

1. Try once with the most obvious alternative location or invocation (e.g. `npx`, full path).
2. If it still fails, **stop and ask the user** to install the tool — do not attempt further workarounds or reimplement the tool's logic in shell/Python.

Example message: "I can't find `markdownlint`. Can you install it (e.g. `npm install -g markdownlint-cli`) and let me know when it's ready?"

Never spiral through multiple fallback strategies for a missing tool. One retry, then ask.

## Dev container software changes

When running inside a dev container (detected by `REMOTE_CONTAINERS`, `CODESPACES`, `/.dockerenv`, or a `/workspaces/` path) and any software is installed, removed, or configured, **always** invoke `/devcontainer-sync` to update `.devcontainer/devcontainer.json` so the change persists across rebuilds.

## Testing policy

Every implementation — bug fix, new feature, or refactor — **must** be accompanied by fitting tests before it is committed.

**Decision rule:**

| What you changed | Where to write tests |
|---|---|
| Pure logic (no GPIO, no BLE, no Arduino API) | Host tests — `test/unit/test_*.cpp` (GoogleTest) |
| Hardware interaction (GPIO, UART, BLE, ISR timing) | On-device tests — `test/test_*_esp32/` or `test/test_*_nrf52840/` (Unity) |
| Both | Both layers |

**Host test checklist** (run via `make test-host`):

1. Add a `test/unit/test_<feature>.cpp` (or extend the relevant existing file).
2. Register the source file in `test/CMakeLists.txt` under `pedal_tests`.
3. Run `/test` to verify all tests pass before committing.

**On-device test checklist** (run via `make test-esp32-*` / `make test-nrf52840-*`):

1. Add or extend a test in the matching `test/test_*_esp32/` or `test/test_*_nrf52840/` directory.
2. Document that the test requires hardware in the task or PR description.
3. Run `/test-device <target>` (e.g. `/test-device esp32-button`) with a connected device before marking the task done.

If a piece of code can be shimmed (see `test/fakes/arduino_shim.h` and the `HOST_TEST_BUILD` guard pattern), prefer a host test over an on-device test — host tests run faster, require no hardware, and are enforced by the pre-commit hook.

When in doubt, ask: "Can I fake the hardware dependency with a shim or mock?" If yes → host test.

## Human interaction — batch questions, don't loop

Keep the number of back-and-forth exchanges to a minimum. When you have multiple questions
before starting a task, ask them **all in one message** rather than one at a time.

**Rule:** If N questions can be asked simultaneously, ask all N at once. Only use a
sequential ask→do→ask→do loop when each answer genuinely depends on the previous one
(e.g. the answer to Q1 determines whether Q2 is even relevant).

**Examples:**

| Situation | Wrong | Right |
|---|---|---|
| Need package name + target file before editing | Ask for name → edit → ask for file | Ask for both upfront |
| Ambiguous scope with 3 unknowns | Ask one, wait, ask next | Ask all three together |
| Q2 depends on answer to Q1 | — | Sequential is correct here |

This does **not** mean reducing oversight — steering questions and approval checkpoints
are still important. The goal is to avoid unnecessary round-trips where the human sits
idle while you ask things you could have asked together.

## Pre-commit hook failures on unrelated changes

When a commit fails because the pre-commit hook (e.g. unit tests, clang-format) runs against
**unstaged or pre-existing changes that are unrelated to the files being committed**, bypassing
the hook (`--no-verify`) may be acceptable — but it is always a case-by-case decision.

**Before suggesting `--no-verify`, check all of the following:**

| Check | Question |
|---|---|
| Staged files only | Are all staged files unrelated to the hook failure? (e.g. only `.md` files staged, but C++ tests fail) |
| Pre-existing breakage | Is the failing check broken on `main` / in the working tree already, not caused by this commit? |
| No silent regression | Would bypassing hide a real regression introduced by the staged changes? |

**If all three are true**, present this to the user:

> "The pre-commit hook failed, but the failure is in `[file/check]` which is unrelated to
> the staged files (`[list staged files]`). This appears to be a pre-existing issue.
> It may be OK to bypass the hook for this commit with `--no-verify`.
> Do you want me to proceed with `--no-verify`, or fix the hook failure first?"

**Never bypass silently.** Always name the failing check, explain why it appears unrelated,
and get explicit user approval before using `--no-verify`.

## Skill registration

When adding a new skill (creating `.claude/skills/<name>/SKILL.md`), always also add `<name>` to `enabled_skills` in [.vibe/config.toml](.vibe/config.toml). Failing to do so means the skill exists on disk but is not registered and may not be loaded.
