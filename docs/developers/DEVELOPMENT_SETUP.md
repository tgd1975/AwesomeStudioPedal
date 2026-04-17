# Development Setup

## Required Tools

> **Quickest setup:** open the project in a [VS Code Dev Container](https://code.visualstudio.com/docs/devcontainers/containers)
> or [GitHub Codespaces](https://github.com/features/codespaces) — all tools below are installed
> automatically via [.devcontainer/devcontainer.json](../../.devcontainer/devcontainer.json).

## Dev Container (local) and GitHub Codespaces

**Local dev container:** open the project folder in VS Code and accept the
"Reopen in Container" prompt, or run **Dev Containers: Reopen in Container** from the
command palette. Requires Docker and the
[Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers).

**Codespaces:** click **Code → Codespaces → Create codespace on `main`** (or your branch)
on the GitHub repo page.

Both environments start with all tools pre-installed and the CMake build directory configured.

**Works everywhere:**

- `make test-host` — host unit tests via GoogleTest (primary workflow)
- `make build-esp32` / `make build-nrf52840` — firmware compilation
- All quality tools: `/clang-tidy`, `/lint`, `/format`, `/fix-markdown`
- `/ci-status`, editing, reviewing, opening PRs

**Does not work in Codespaces (no USB):**

- `make upload-*` / `make run-*` — flashing requires a physical device connected via USB
- `make monitor-*` — serial monitor requires a physical device
- `make test-esp32-*` / `make test-nrf52840-*` — on-device tests require hardware

**PlatformIO note:** the PlatformIO CLI (`pio`) is installed once during container creation
(`postCreateCommand`). The first `pio run` downloads the ESP32/nRF52840 toolchains (~300 MB)
and caches them for the life of the container.

### Host folder mounts (local dev container only)

The following folders are bind-mounted from the host into the container so that AI tool
credentials are available without storing any secrets in the repository:

| Host path | Container path | Purpose |
|-----------|---------------|---------|
| `~/.claude` | `/home/vscode/.claude` | Claude Code authentication |

These mounts are transparent — credentials set up on the host are used automatically inside
the container. Nothing is copied into the image or committed to the repo.

> **Codespaces:** the mounts above are host-only and do not apply. Authenticate Claude Code
> and Mistral Vibe manually after the codespace starts, or pass `ANTHROPIC_API_KEY` /
> `MISTRAL_API_KEY` as Codespaces secrets.

### Mistral API key

The container reads `MISTRAL_API_KEY` from the host environment via `remoteEnv`. Add it to
your shell profile on the host (`~/.bashrc`, `~/.zshrc`, etc.):

```bash
export MISTRAL_API_KEY=your-key-here
```

The `vibe` CLI (installed via `uv tool install mistral-vibe`) picks this up automatically.

---

### All platforms

| Tool | Min version | Required for | Install |
|------|-------------|--------------|---------|
| [PlatformIO](https://platformio.org/install/cli) | any recent | Build + flash firmware | `pip install platformio` or VS Code extension |
| [CMake](https://cmake.org/download/) | 3.20 | Host unit tests | package manager or cmake.org |
| [Ninja](https://ninja-build.org/) | any | Host unit tests | `apt install ninja-build` / `brew install ninja` |
| [GCC / G++](https://gcc.gnu.org/) | 11+ | Host unit tests (Linux/macOS) | `apt install g++` / Xcode CLT |
| [clang-format](https://clang.llvm.org/docs/ClangFormat.html) | 14+ | C++ formatting (pre-commit hook) | `apt install clang-format` / `brew install llvm` |
| [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) | 14+ | Static analysis | `apt install clang-tidy` / `brew install llvm` |
| [Python 3](https://www.python.org/downloads/) | 3.9+ | Scripts, PlatformIO | system or python.org |
| [Node.js](https://nodejs.org/) | 20+ | markdownlint, Mermaid CLI | `apt install nodejs` / nodejs.org |
| [Git](https://git-scm.com/) | any | Version control | `apt install git` / Xcode CLT |
| [GitHub CLI (`gh`)](https://cli.github.com/) | any | `/ci-status`, `/pr` skills | `apt install gh` / `brew install gh` |

GoogleTest is fetched automatically by CMake (v1.15.2 via `FetchContent`) — no manual install needed.

### ESP32 flashing

| Tool | Notes | Install |
|------|-------|---------|
| **esptool** | Bundled with PlatformIO — no separate install needed | — |
| **USB-serial driver** | NodeMCU-32S uses CP2102 (CP210x). Linux: built-in kernel module. macOS: [Silicon Labs driver](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers). Windows: same Silicon Labs driver. | See link |
| **Serial port access (Linux)** | Add your user to the `dialout` group: `sudo usermod -aG dialout $USER`, then log out and back in. See [FLASHING.md](../builders/FLASHING.md). | — |

### nRF52840 flashing

> nRF52840 support is implemented but untested. Use at your own risk.

| Tool | Notes | Install |
|------|-------|---------|
| [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools) | `nrfjprog` for flashing via J-Link | nordicsemi.com |
| [J-Link software](https://www.segger.com/downloads/jlink/) | Required for SWD programming | segger.com |
| **USB driver** | Adafruit Feather nRF52840 uses native USB. Windows may need [Adafruit's driver](https://learn.adafruit.com/adafruit-feather-m0-express-designed-for-circuit-python-circuitpython/using-with-arduino-ide#windows-driver-installation-3-3). | See link |

### Optional tools

| Tool | Used for |
|------|----------|
| [lcov + genhtml](https://github.com/linux-test-project/lcov) | `make coverage` — HTML coverage report. `apt install lcov`. |
| [Doxygen](https://www.doxygen.nl/download.html) | `make docs` — API documentation. `apt install doxygen`. |

---

## Build command reference

| Command | What it does | Hardware needed |
|---------|--------------|----------------|
| `make build-esp32` | Build firmware for ESP32 | No |
| `make upload-esp32` | Flash firmware to ESP32 | Yes |
| `make uploadfs-esp32` | Upload `data/` filesystem image to ESP32 | Yes |
| `make run-esp32` | Build + upload firmware + upload filesystem + monitor | Yes |
| `make monitor-esp32` | Open serial monitor | Yes |
| `make test-host` | Run host unit tests (GoogleTest) | No |
| `make test-esp32-button` | Upload filesystem + on-device button tests | Yes (ESP32) |
| `make test-esp32-serial` | Upload filesystem + on-device serial tests | Yes (ESP32) |
| `make test-esp32-profilemanager` | Upload filesystem + on-device profile manager tests | Yes (ESP32) |
| `make build-nrf52840` | Build firmware for nRF52840 | No |
| `make upload-nrf52840` | Flash firmware to nRF52840 | Yes |
| `make uploadfs-nrf52840` | Upload `data/` filesystem image to nRF52840 | Yes |
| `make run-nrf52840` | Build + upload firmware + upload filesystem + monitor | Yes (nRF52840) |
| `make test-nrf52840-profilemanager` | Upload filesystem + on-device profile manager tests | Yes (nRF52840) |

## Updating the configuration without recompiling

`data/pedal_config.json` is the only file that needs to change when adding, removing, or editing
profiles and button actions. The firmware itself does not need to be rebuilt — only the filesystem
partition is re-flashed.

```bash
# Edit the config, then:
make uploadfs-esp32      # ESP32
make uploadfs-nrf52840   # nRF52840
```

The device reads the config at startup via `ConfigLoader::loadFromFile()`, so the next boot will
pick up the changes automatically. This is the normal workflow for profile customisation.

All `make test-esp32-*` and `make test-nrf52840-*` targets include the filesystem upload as a
prerequisite, so you never need to run it manually before running device tests.

## Host test build

Host tests run on the development machine via CMake and Ninja:

```bash
cmake -B .vscode/build-host -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build .vscode/build-host --target pedal_tests
```

Or use the Makefile shortcut:

```bash
make test-host
```

## Serial port setup on Linux

See [FLASHING.md](../builders/FLASHING.md) for the `dialout` group setup steps.

## Alternative serial monitor

If PlatformIO's built-in monitor has issues, use the Python script:

```bash
python3 scripts/serial_monitor.py /dev/ttyUSB0 115200
```

## Code coverage (local)

Generate an HTML coverage report locally (requires `lcov` and `genhtml`):

```bash
make coverage
# Output: docs/coverage/index.html
```

The CI enforces a **minimum 80% line coverage** threshold. The `coverage` job in
`test.yml` builds with `--coverage`, runs the test suite, and uses `gcovr` to
generate an HTML + XML report and fail the build if coverage drops below 80%.
The HTML report is uploaded as a `coverage-html` artifact on every CI run.

The `docs/coverage/` directory is gitignored — generated output is never committed.

## API documentation (local)

Generate Doxygen output locally:

```bash
doxygen Doxyfile
# Output: docs/api/html/index.html
```

The CI builds Doxygen on every push and pull request. On pushes to `main` the
output is deployed to GitHub Pages. HTML is also uploaded as an `api-docs-html`
artifact on every CI run.

The `docs/api/` directory is gitignored — generated output is never committed.
