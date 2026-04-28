# Ideas Overview

**Open: 39** | **Archived: 8**

Ideas are lightweight, qualitative proposals tracked in [`open/`](open/) until they are either converted into structured tasks or archived. Archived ideas are kept for history in [`archived/`](archived/).

## Open Ideas

| ID | Title | Description |
|----|-------|-------------|
| [IDEA-003](open/idea-003-additional-hardware-support.md) | Additional Hardware Support | Extend compatibility to platforms like Arduino Nano |
| [IDEA-004](open/idea-004-nrf-hardware-testing.md) | nRF Hardware Testing | Thoroughly test and validate the nRF52840 implementation |
| [IDEA-007](open/idea-007-display-integration.md) | Display Integration | Add a display to show profile info — small (profile name) or larger (full config) |
| [IDEA-008](open/idea-008-hybrid-tool-with-dsp.md) | Hybrid Tool with DSP | More powerful hardware with DSP and dual audio jacks for guitar pedal effects |
| [IDEA-011](open/idea-011-pcb-board-design.md) | PCB Board Design | Custom PCB to replace the breadboard/prototype setup for reliability and manufacturability |
| [IDEA-012](open/idea-012-two-button-soft-foot-switches.md) | Two-Button Rugged Pedal — Metal/Wood Enclosure | A stomp-proof two-button pedal variant with a metal top panel, wooden side elements, LiPo battery, ESP32 with display, back panel, and dual LEDs per button — the production-grade alternative to the 3D-printed prototype. |
| [IDEA-013](open/idea-013-bus-system.md) | Bus System | Daisy-chain bus for connecting multiple pedals instead of direct controller wiring |
| [IDEA-014](open/idea-014-automated-hardware-testing-rig.md) | Automated Hardware Testing Rig | A relay-based test interface with optocoupler output detection that enables fully automated on-device testing without human intervention |
| [IDEA-015](open/idea-015-marketing-material.md) | Marketing Material | Create compelling marketing material to attract musicians and builders |
| [IDEA-016](open/idea-016-articles-written-by-journalists.md) | Articles Written by Journalists | Write one article for each persona |
| [IDEA-017](open/idea-017-community-profiles-repository.md) | Community Profiles Repository | A curated folder of ready-to-use profiles for popular tools and workflows, directly accessible from the web configurators and the mobile app — with a call for community contributions. |
| [IDEA-018](open/idea-018-bom-prototype.md) | Bill of Materials — Prototype | A complete, sourced BOM for the default 4-button prototype with part numbers, quantities, costs, and substitution notes — replacing the current placeholder list in BUILD_GUIDE.md. |
| [IDEA-020](open/idea-020-mbse-sysmlv2.md) | MBSE System Documentation with SysML v2 | Retroactively document the system using SysML v2 textual notation — stakeholder needs, derived requirements, structural architecture, behavioural models, and interface contracts — all as committed text files with CI validation and rendered output. |
| [IDEA-021](open/idea-021-task_system_concept_for_small_teams.md) | Task and Idea System Concept for Small Teams | Target design for the docs-as-code task and idea management system — covering file formats, workflow, generated dashboards, skills, and distribution. |
| [IDEA-022](open/idea-022-mkdocs-documentation-site.md) | MkDocs Documentation Site | Introduce MkDocs as a unified documentation platform for both developer-facing and public-facing content — replacing a growing flat collection of Markdown files with a navigable, searchable, CI-rendered site. |
| [IDEA-023](open/idea-023-led-resistor-guide.md) | LED Current-Limiting Resistor Guide — Matched Brightness for 5 mm LEDs | A builder-facing reference table for selecting the correct Vorwiderstand for any common 5 mm LED colour so they appear at roughly equal perceived brightness when driven directly from a 3.3 V GPIO. |
| [IDEA-024](open/idea-024-profile-independent-actions.md) | Profile-Independent Actions | An optional always-active pseudo-profile whose actions fire in parallel with the active profile's actions on every button event — regardless of which profile is selected. If the block is absent, behaviour is identical to today. |
| [IDEA-025](open/idea-025-configurable-ble-device-name.md) | Configurable BLE Device Name | Allow users to configure the BLE device name via config.json |
| [IDEA-026](open/idea-026-task-group-improvements.md) | Task Group Improvements | Enhance task group organization and visibility in OVERVIEW.md |
| [IDEA-027](open/idea-027-circuit-skill.md) | Circuit-Skill — AI-Assisted Schematic Generation with ERC, BOM, and Netlist Export | > |
| [IDEA-027-erc-engine](open/idea-027-erc-engine.md) | ERC Engine — topology-only electrical rule check | > |
| [IDEA-028](open/idea-028-hardware-abstraction-layer.md) | Hardware Abstraction Layer — replace #ifdef soup with a platform class hierarchy | Introduce an abstract PedalApp base class and per-hardware specializations to eliminate platform #ifdefs from main.cpp and PedalLogic sources. |
| [IDEA-029](open/idea-029-configurable-select-long-press-pairing.md) | Configurable SELECT Long-Press to Activate BLE Pairing Mode | Very-long-press on the SELECT button activates BLE pairing mode; the feature is opt-in via config.json so it can be disabled for testing and CI. |
| [IDEA-030](open/idea-030-epic-suggested-branch-soft-enforcement.md) | Epic-Suggested Branch with Soft Enforcement | Each epic declares a suggested branch; task/commit flows nudge (not block) the user when work on an epic happens on a different branch, to encourage smaller, focused releases. |
| [IDEA-031](open/idea-031-extract-task-system-standalone-repo.md) | Extract the task-and-idea system into a standalone public GitHub repository | Lift the docs-as-code task + idea management system out of AwesomeStudioPedal into its own public repo so other small teams can use it and this project can consume it as an installable tool. |
| [IDEA-032](open/idea-032-extract-circuits-as-docs-standalone-repo.md) | Extract the circuits-as-docs system into a standalone public GitHub repository | Lift the Schemdraw-based schematic generator, pre-commit staleness hook, and docs-as-code wiring pattern into a standalone public repo so other hardware projects can document their circuits the same way. |
| [IDEA-033](open/idea-033-shorten-doublepress-window.md) | Shorten double-press window to reduce single-press latency | 300 ms defer on every single-press feels sluggish — find the smallest window that still lets humans double-tap reliably. |
| [IDEA-034](open/idea-034-test-regime-reevaluation.md) | Test Regime Reevaluation — minimize human-in-the-loop after IDEA-028 | After the IDEA-028 HAL refactoring lands, do a thorough reevaluation of the entire test regime — close gaps, drop deprecated/duplicate tests, and minimize human-in-the-loop coverage (e.g. BLE is now testable headlessly). |
| [IDEA-035](open/idea-035-multi-user-task-docs-as-code.md) | Multi-User Workflow for Docs-as-Code Task System | How to evolve the docs-as-code task and idea system from "single user, current branch is the truth" to "two or more people working in parallel branches" without losing the things that make the current system pleasant. |
| [IDEA-037](open/idea-037-app-content-pages.md) | App content pages — Info/About, How-To, and supporting screens | Add minimal in-app content pages — Info/About, a crisp How-To, and similar supporting screens — so the app is self-explanatory without external docs. |
| [IDEA-038](open/idea-038-comprehensive-review-framework.md) | Comprehensive code & architecture review framework — define the dimensions | Iteratively define what "thorough review" means for this project — clarity, testability, documentation, usefulness, no dead-ends, extendability — into an actionable rubric. |
| [IDEA-039](open/idea-039-raw-hid-value-space-decision.md) | Action Editor "Key (raw HID)" — pick a value space (USB Usage IDs vs firmware-internal codes) and align app, firmware, and docs | Decide whether the "Key (raw HID)" numeric value is the standard USB HID Usage ID (e.g. 0x28 = Enter) or the firmware-internal NicoHood/BleKeyboard code (e.g. 0xB0 = Enter), then align the app field, firmware parser, schema, and docs to a single answer. |
| [IDEA-042](open/idea-042-one-click-profile-contribution-from-app.md) | One-click profile contribution from the app to the community profiles repository | A near-frictionless "Share this profile" flow inside the mobile app and web tools that submits a user's profile to the community repo for maintainer review — no Git, no fork, no PR knowledge required. |
| [IDEA-043](open/idea-043-coordinated-task-system-rollout.md) | Coordinated task-system rollout — burn-up dashboard, post-hoc effort, paused status, and awesome-task-system carve-out | Sequenced rollout for the burn-up dashboard, post-hoc effort reassessment, paused-as-first-class status, and the awesome-task-system source-of-truth carve-out. |
| [IDEA-043-llm-effort-reassessment-on-close](open/idea-043-llm-effort-reassessment-on-close.md) | Re-assess effort on task close (estimate vs. actual) | When closing a task, record a post-hoc effort size alongside the original estimate — same vocabulary, same flag-or-judgment flow as `/ts-task-new`. Feeds IDEA-043-release-burnup-chart and surfaces estimation drift. |
| [IDEA-043-paused-as-first-class-task-status](open/idea-043-paused-as-first-class-task-status.md) | "`paused` as a first-class task status" | Promote pause from an "active task with prerequisites" overload to a real status alongside open / active / closed, with its own folder and index visibility. |
| [IDEA-043-release-burnup-chart](open/idea-043-release-burnup-chart.md) | Release burn-up chart from closed tasks | Auto-generated burn-up section embedded in tasks/OVERVIEW.md — tasks-closed and epics-closed shown side-by-side, plus an effort-hours chart with estimate-vs-actual lines (per IDEA-043-llm-effort-reassessment-on-close). |
| [IDEA-044](open/idea-044-action-editor-value-field-resets-on-type-change.md) | Action Editor value field clears (or contextualises) when Action Type changes | When the user changes Action Type in the Action Editor, the value field currently keeps its previous text — so switching from "Key (named)" / KEY_PAGE_UP to "Media Key" leaves the field reading `KEY_PAGE_UP`, which is meaningless under the new type. Make the field reset, or at least show a type-appropriate placeholder. |
| [IDEA-045](open/idea-045-validation-banner-revalidates-on-action-edit.md) | Validation banner revalidates when an action is edited (not only when profile count changes) | The Profile List validation banner currently only re-runs validation when the number of profiles changes. Editing a button's action value (e.g. fixing a typo) does not refresh the banner — it stays red until the user navigates away and back. Trigger revalidation on any ProfilesState change. |

## Archived Ideas

| ID | Title |
|----|-------|
| [IDEA-000](archived/idea-000-initial-prototype.md) | Initial Prototype |
| [IDEA-001](archived/idea-001-mobile-app-configuration.md) | Mobile App Configuration |
| [IDEA-002](archived/idea-002-cli-tools.md) | CLI Tools |
| [IDEA-005](archived/idea-005-large-button-pedal-prototype.md) | Large Button Pedal Prototype |
| [IDEA-006](archived/idea-006-macros.md) | Macros |
| [IDEA-009](archived/idea-009-long-press-event.md) | Long Press Event |
| [IDEA-010](archived/idea-010-double-press-event.md) | Double Press Event |
| [IDEA-019](archived/idea-019-wiring-as-code.md) | Wiring as Code — Schematic-as-Code for Documentation and Netlist Generation |
