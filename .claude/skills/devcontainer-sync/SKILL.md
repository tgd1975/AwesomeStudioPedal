---
name: devcontainer-sync
description: Keep devcontainer.json in sync when software is installed, removed, or configured inside the dev container
---

# devcontainer-sync

**Policy:** Whenever you install, remove, or configure software while running inside a dev container,
you MUST also update `.devcontainer/devcontainer.json` so the change is persisted for the next
container rebuild. A change that exists only in the running container is lost on rebuild.

## Detecting a dev container

We are inside a dev container when **any** of the following is true:

- The environment variable `REMOTE_CONTAINERS` is set (VS Code Dev Containers).
- The environment variable `CODESPACES` is set (GitHub Codespaces).
- The file `/.dockerenv` exists (generic Docker runtime).
- The working directory is under `/workspaces/` (convention used by both VS Code and Codespaces).

Check with:

```bash
[ -n "$REMOTE_CONTAINERS" ] || [ -n "$CODESPACES" ] || [ -f /.dockerenv ] && echo "inside devcontainer"
```

If **none** of these match, we are on a native host. Skip this skill — no devcontainer update is needed.

## What to update in devcontainer.json

The file is at `.devcontainer/devcontainer.json`. Choose the right section:

| Change type | Where in devcontainer.json |
|---|---|
| `apt-get install <pkg>` | Add `<pkg>` to the `apt-get install` invocation inside `postCreateCommand` |
| `apt-get remove <pkg>` | Remove `<pkg>` from the `apt-get install` list in `postCreateCommand` |
| `npm install -g <pkg>` | Add/remove from the `npm install -g` call in `postCreateCommand` |
| `pip install <pkg>` | Add/remove from the `pip install` call in `postCreateCommand` |
| `uv tool install <pkg>` | Add/remove from the `uv tool install` calls in `postCreateCommand` |
| New feature (e.g. Docker-in-Docker) | Add an entry under `"features": {}` |
| VS Code extension | Add/remove from `customizations.vscode.extensions` |
| Environment variable | Add/remove from `"remoteEnv": {}` |
| Mounted volume | Add/remove from `"mounts": []` |

## Rules

1. **Always include a comment** next to the added/changed line explaining what the tool is for and
   which skill or CI step requires it. Match the comment style already used in the file.
2. **Keep `postCreateCommand` as a single shell string** joined with `&&`. Do not split it into an
   array — the existing format uses a string.
3. **Preserve all existing entries.** Only append, adjust, or remove the specific package that
   changed; do not reorder or reformat unrelated lines.
4. **Prefer `features` over `postCreateCommand`** for runtimes with an official Dev Container
   Feature (Node.js, Python, Go, Docker, etc.). Only fall back to `postCreateCommand` for packages
   that have no feature.
5. After editing the file, tell the user:
   > "`devcontainer.json` updated — rebuild the container (`Dev Containers: Rebuild Container`)
   > to apply the change to fresh environments."

## Example — adding `jq`

Before (excerpt):

```
"postCreateCommand": "sudo apt-get update -q && sudo apt-get install -y -q lcov doxygen && npm install -g markdownlint-cli2 ..."
```

After:

```
"postCreateCommand": "sudo apt-get update -q && sudo apt-get install -y -q lcov doxygen jq && npm install -g markdownlint-cli2 ..."
```

Add a comment near the `postCreateCommand` key (or inline if the JSON allows) explaining `jq` is needed for the relevant script/skill.
