# Ideas Overview

**Open: 32** | **Archived: 19**

Ideas are lightweight, qualitative proposals tracked in [`open/`](open/) until they are either converted into structured tasks or archived. Archived ideas are kept for history in [`archived/`](archived/).

## Open Ideas

| ID | Category | Title | Description |
|----|----------|-------|-------------|
| [IDEA-003](open/idea-003-additional-hardware-support.md) | 🔧 hardware | Additional Hardware Support | Extend compatibility to platforms like Arduino Nano |
| [IDEA-004](open/idea-004-nrf-hardware-testing.md) | 🔧 hardware | nRF Hardware Testing | Thoroughly test and validate the nRF52840 implementation |
| [IDEA-007](open/idea-007-display-integration.md) | 🔧 hardware | Display Integration | Add a display to show profile info — small (profile name) or larger (full config) |
| [IDEA-008](open/idea-008-hybrid-tool-with-dsp.md) | 🔧 hardware | Hybrid Tool with DSP | More powerful hardware with DSP and dual audio jacks for guitar pedal effects |
| [IDEA-011](open/idea-011-pcb-board-design.md) | 🔧 hardware | PCB Board Design | Custom PCB to replace the breadboard/prototype setup for reliability and manufacturability |
| [IDEA-012](open/idea-012-two-button-soft-foot-switches.md) | 🔧 hardware | Two-Button Rugged Pedal — Metal/Wood Enclosure | A stomp-proof two-button pedal variant with a metal top panel, wooden side elements, LiPo battery, ESP32 with display, back panel, and dual LEDs per button — the production-grade alternative to the 3D-printed prototype. |
| [IDEA-013](open/idea-013-bus-system.md) | 🔧 hardware | Bus System | Daisy-chain bus for connecting multiple pedals instead of direct controller wiring |
| [IDEA-014](open/idea-014-automated-hardware-testing-rig.md) | 🛠️ tooling | Automated Hardware Testing Rig | A relay-based test interface with optocoupler output detection that enables fully automated on-device testing without human intervention |
| [IDEA-015](open/idea-015-marketing-material.md) | 📣 outreach | Marketing Material | Create compelling marketing material to attract musicians and builders |
| [IDEA-016](open/idea-016-articles-written-by-journalists.md) | 📣 outreach | Articles Written by Journalists | Write one article for each persona |
| [IDEA-017](open/idea-017-community-profiles-repository.md) | 📱 apps | Community Profiles Repository | A curated folder of ready-to-use profiles for popular tools and workflows, directly accessible from the web configurators and the mobile app — with a call for community contributions. |
| [IDEA-018](open/idea-018-bom-prototype.md) | 📖 docs | Bill of Materials — Prototype | A complete, sourced BOM for the default 4-button prototype with part numbers, quantities, costs, and substitution notes — replacing the current placeholder list in BUILD_GUIDE.md. |
| [IDEA-020](open/idea-020-mbse-sysmlv2.md) | 📖 docs | MBSE System Documentation with SysML v2 | Retroactively document the system using SysML v2 textual notation — stakeholder needs, derived requirements, structural architecture, behavioural models, and interface contracts — all as committed text files with CI validation and rendered output. |
| [IDEA-022](open/idea-022-mkdocs-documentation-site.md) | 📖 docs | MkDocs Documentation Site | Introduce MkDocs as a unified documentation platform for both developer-facing and public-facing content — replacing a growing flat collection of Markdown files with a navigable, searchable, CI-rendered site. |
| [IDEA-023](open/idea-023-led-resistor-guide.md) | 📖 docs | LED Current-Limiting Resistor Guide — Matched Brightness for 5 mm LEDs | A builder-facing reference table for selecting the correct Vorwiderstand for any common 5 mm LED colour so they appear at roughly equal perceived brightness when driven directly from a 3.3 V GPIO. |
| [IDEA-027](open/idea-027-circuit-skill.md) | 🛠️ tooling | Circuit-Skill — AI-Assisted Schematic Generation with ERC, BOM, and Netlist Export | > |
| [IDEA-027-erc-engine](open/idea-027-erc-engine.md) | 🛠️ tooling | ERC Engine — topology-only electrical rule check | > |
| [IDEA-029](open/idea-029-configurable-select-long-press-pairing.md) | ⚡ firmware | Configurable SELECT Long-Press to Activate BLE Pairing Mode | Very-long-press on the SELECT button activates BLE pairing mode; the feature is opt-in via config.json so it can be disabled for testing and CI. |
| [IDEA-030](open/idea-030-epic-suggested-branch-soft-enforcement.md) | 🛠️ tooling | Epic-Suggested Branch with Soft Enforcement | Each epic declares a suggested branch; task/commit flows nudge (not block) the user when work on an epic happens on a different branch, to encourage smaller, focused releases. |
| [IDEA-031](open/idea-031-extract-task-system-standalone-repo.md) | 🛠️ tooling | Extract the task-and-idea system into a standalone public GitHub repository | Lift the docs-as-code task + idea management system out of AwesomeStudioPedal into its own public repo so other small teams can use it and this project can consume it as an installable tool. |
| [IDEA-032](open/idea-032-extract-circuits-as-docs-standalone-repo.md) | 🛠️ tooling | Extract the circuits-as-docs system into a standalone public GitHub repository | Lift the Schemdraw-based schematic generator, pre-commit staleness hook, and docs-as-code wiring pattern into a standalone public repo so other hardware projects can document their circuits the same way. |
| [IDEA-033](open/idea-033-shorten-doublepress-window.md) | ⚡ firmware | Shorten double-press window to reduce single-press latency | 300 ms defer on every single-press feels sluggish — find the smallest window that still lets humans double-tap reliably. |
| [IDEA-034](open/idea-034-test-regime-reevaluation.md) | 🛠️ tooling | Test Regime Reevaluation — minimize human-in-the-loop after IDEA-028 | After the IDEA-028 HAL refactoring lands, do a thorough reevaluation of the entire test regime — close gaps, drop deprecated/duplicate tests, and minimize human-in-the-loop coverage (e.g. BLE is now testable headlessly). |
| [IDEA-035](open/idea-035-multi-user-task-docs-as-code.md) | 🛠️ tooling | Multi-User Workflow for Docs-as-Code Task System | How to evolve the docs-as-code task and idea system from "single user, current branch is the truth" to "two or more people working in parallel branches" without losing the things that make the current system pleasant. |
| [IDEA-037](open/idea-037-app-content-pages.md) | 📱 apps | App content pages — Info/About, How-To, and supporting screens | Add minimal in-app content pages — Info/About, a crisp How-To, and similar supporting screens — so the app is self-explanatory without external docs. |
| [IDEA-038](open/idea-038-comprehensive-review-framework.md) | 📖 docs | Comprehensive code & architecture review framework — define the dimensions | Iteratively define what "thorough review" means for this project — clarity, testability, documentation, usefulness, no dead-ends, extendability — into an actionable rubric. |
| [IDEA-039](open/idea-039-raw-hid-value-space-decision.md) | ⚡ firmware | Action Editor "Key (raw HID)" — pick a value space (USB Usage IDs vs firmware-internal codes) and align app, firmware, and docs | Decide whether the "Key (raw HID)" numeric value is the standard USB HID Usage ID (e.g. 0x28 = Enter) or the firmware-internal NicoHood/BleKeyboard code (e.g. 0xB0 = Enter), then align the app field, firmware parser, schema, and docs to a single answer. |
| [IDEA-042](open/idea-042-one-click-profile-contribution-from-app.md) | 📱 apps | One-click profile contribution from the app to the community profiles repository | A near-frictionless "Share this profile" flow inside the mobile app and web tools that submits a user's profile to the community repo for maintainer review — no Git, no fork, no PR knowledge required. |
| [IDEA-046](open/idea-046-ble-config-cpp-copy-loop-investigation.md) | ⚡ firmware | Investigate the ble_config_service.cpp copy loop — possible build-system smell |  |
| [IDEA-047](open/idea-047-device-resource-arbiter.md) | 🛠️ tooling | Device-resource arbiter for parallel test sessions | Mutual-exclusion lock around scarce hardware (ESP32, nRF52840, Pixel) so two parallel agent sessions can't grab the same device simultaneously, with self-recovery from hangs and crashes. |
| [IDEA-048](open/idea-048-off-the-shelf-pedalboard.md) | 🔧 hardware | Off-the-shelf pedalboard build — 2 pedals from mostly stock components | A 2-pedal board assembled from as many off-the-shelf parts as possible (enclosure, switches, jacks, hardware) — minimise custom fabrication. |
| [IDEA-049](open/idea-049-logo-more-prominent.md) | 📱 apps | Make the logo more prominent across apps and tools | Use the project logo more consistently — especially in the simulator and the config builders, which currently have none. |

## Archived Ideas

| ID | Category | Title |
|----|----------|-------|
| [IDEA-000](archived/idea-000-initial-prototype.md) | 🔧 hardware | Initial Prototype |
| [IDEA-001](archived/idea-001-mobile-app-configuration.md) | 📱 apps | Mobile App Configuration |
| [IDEA-002](archived/idea-002-cli-tools.md) | 🛠️ tooling | CLI Tools |
| [IDEA-005](archived/idea-005-large-button-pedal-prototype.md) | 🔧 hardware | Large Button Pedal Prototype |
| [IDEA-006](archived/idea-006-macros.md) | ⚡ firmware | Macros |
| [IDEA-009](archived/idea-009-long-press-event.md) | ⚡ firmware | Long Press Event |
| [IDEA-010](archived/idea-010-double-press-event.md) | ⚡ firmware | Double Press Event |
| [IDEA-019](archived/idea-019-wiring-as-code.md) | 🛠️ tooling | Wiring as Code — Schematic-as-Code for Documentation and Netlist Generation |
| [IDEA-021](archived/idea-021-task_system_concept_for_small_teams.md) | 🛠️ tooling | Task and Idea System Concept for Small Teams |
| [IDEA-024](archived/idea-024-profile-independent-actions.md) | ⚡ firmware | Profile-Independent Actions |
| [IDEA-025](archived/idea-025-configurable-ble-device-name.md) | ⚡ firmware | Configurable BLE Device Name |
| [IDEA-026](archived/idea-026-task-group-improvements.md) | 🛠️ tooling | Task Group Improvements |
| [IDEA-028](archived/idea-028-hardware-abstraction-layer.md) | ⚡ firmware | Hardware Abstraction Layer — replace #ifdef soup with a platform class hierarchy |
| [IDEA-043](archived/idea-043-coordinated-task-system-rollout.md) | 🛠️ tooling | Coordinated task-system rollout — burn-up dashboard, post-hoc effort, paused status, and awesome-task-system carve-out |
| [IDEA-043-llm-effort-reassessment-on-close](archived/idea-043-llm-effort-reassessment-on-close.md) | 🛠️ tooling | Re-assess effort on task close (estimate vs. actual) |
| [IDEA-043-paused-as-first-class-task-status](archived/idea-043-paused-as-first-class-task-status.md) | 🛠️ tooling | "`paused` as a first-class task status" |
| [IDEA-043-release-burnup-chart](archived/idea-043-release-burnup-chart.md) | 🛠️ tooling | Release burn-up chart from closed tasks |
| [IDEA-044](archived/idea-044-action-editor-value-field-resets-on-type-change.md) | 📱 apps | Action Editor value field clears (or contextualises) when Action Type changes |
| [IDEA-045](archived/idea-045-validation-banner-revalidates-on-action-edit.md) | 📱 apps | Validation banner revalidates when an action is edited (not only when profile count changes) |
