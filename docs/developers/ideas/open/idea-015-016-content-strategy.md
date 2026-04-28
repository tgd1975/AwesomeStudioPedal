# IDEA-015 + IDEA-016: Unified Content & Marketing Strategy

> **Status**: Concept document — synthesizes the research, editorial frameworks, and drafts
> accumulated in IDEA-015 and IDEA-016 into a single actionable plan.
>
> Last updated: 2026-04-19

---

## 0. Owner Constraints — Read This Before the Strategy

This section exists so that the strategy does not drift into fantasy. Every plan in this
document must be realistic given one person with limited time and zero appetite for sustained
outreach effort.

### What will happen

- **Responding on GitHub** — issues, PRs, stars, Discussions. This is the natural and
  enthusiastic default. A fast, helpful, thoughtful response to the first person who opens an
  issue or sends a PR is worth more than any campaign. This is where the project's reputation
  will actually be built.
- **Reacting opportunistically** — if a post appears somewhere referencing the project, or if
  a relevant thread comes up in a forum the owner already reads, a brief authentic reply is
  fine and natural.
- **One-off forum posts, rarely** — posting something to an existing forum (GroupDIY,
  DIYStompboxes, Hackaday, r/diypedals) is possible on a "feeling daring" basis. One post,
  done well, is a valid strategy. It does not require a campaign.

### What will not happen

- **No active community building** — no Discord server to moderate, no newsletter, no regular
  posting schedule, no "engagement strategy".
- **No advertising or promotion campaigns** — no paid placement, no cross-posting across
  multiple platforms, no coordinated launch.
- **No sustained outreach** — no cold-emailing journalists, no pitching publications, no
  following up with people who did not respond.

### What this means for the strategy

Most of §5's deliverables are *optional background work* — things that make the project more
useful and credible to whoever stumbles across it organically. The forum pitch templates (§5.8)
and content matrix entries 3, 4, 5, 9, 11 describe *possible* actions, not a schedule.

The only deliverables that are both high-value and low-effort for this owner are:

1. **The README intro** — written once, works forever, seen by everyone who finds the repo.
2. **The BLE + enclosure technical note** — written once, prevents a recurring support question.
3. **The use-case scenario cards** — written once, give visitors the "aha" moment without
   requiring a conversation.
4. **The BOM** (IDEA-018) — written once, removes the biggest friction point for builders.

Everything else in this document is a menu to pick from when energy exists, not a to-do list.

> **The realistic growth model**: the GitHub repository is the product. If the code is good,
> the docs are honest, and the owner responds warmly to the first few people who show up —
> the community, if it comes, will build itself.

### AI involvement — be transparent, not apologetic

The owner is the product architect. Most code is written with AI assistance (Claude Code,
Mistral, and similar tools). The prototype was written by hand; ongoing development is not.
This is documented in `CONTRIBUTING.md` and is not a point of shame — the quality bar is
maintained by the toolchain, not by who typed the code.

**For content and messaging, this means:**

- Do not claim or imply that every line was hand-written by a solo developer working alone.
- Do not over-explain or over-justify the AI involvement — a brief, matter-of-fact mention
  is enough. Transparency without defensiveness.
- If the topic comes up in a forum post or community discussion, the honest answer is:
  *"Yes, I use AI tools heavily. The architecture, use cases, and final review are mine. The
  CI enforces a high quality bar regardless of how the code was produced."*
- This framing is actually a differentiator in the maker community, where "I built the whole
  thing myself" is often code for "the architecture is a mess." A project with a maintained
  architect and AI coding partners is arguably *more* trustworthy, not less.

**The contribution standard that follows from this:**

Any PR — human or AI-assisted — gets accepted or rejected based on one question: *does the
maintainer understand what this is trying to accomplish, and is it worth having?* A script
generated without the contributor understanding its purpose will be rejected. A well-reasoned
AI-assisted contribution with a clear issue reference will not be.

---

## Executive Summary

IDEA-015 (Marketing Material) and IDEA-016 (Articles Written by Journalists) are two sides of the
same coin. IDEA-015 defines *what* we say — the value propositions, use-case stories, and competitive
framing. IDEA-016 defines *how* we say it depending on the audience. Executed in isolation each idea
stays incomplete; executed together they form a **full content strategy** that can drive awareness,
adoption, and community growth.

The central insight from the accumulated research is the **Maker-Musician** persona: someone who
performs or records music and is comfortable enough with hardware and code to build their own tools.
This person is the natural first audience for AwesomeStudioPedal, and almost every piece of content
we produce should speak to them first — then fan out to adjacent audiences (pure developers,
3D-printing enthusiasts, pure musicians who are introduced by a maker friend).

---

## 1. Honest Positioning — Reality Check First

Before any marketing is written, the product needs an honest self-assessment. Getting this wrong
poisons every downstream message.

### 1.1 What AwesomeStudioPedal actually is

- A **wireless Bluetooth HID controller** (acts as a keyboard, no driver needed).
- **Fully open-source** firmware, configuration format, CAD files, and toolchain.
- **Multi-profile** — up to 7 modes, selectable with a single button, confirmed by an LED array.
- **JSON-configurable** — remapping requires editing a text file, not recompiling firmware.
- **Time-delayed action** — trigger a command and step into position before it fires.
- Built on commodity ESP32 hardware, housed in a **3D-printed prototype enclosure**.

### 1.2 What it is NOT (and must never be claimed to be)

| Claim to avoid | Why it is misleading | Honest alternative |
|---|---|---|
| "Stage-ready stomp pedal" | The enclosure is a 3D-printed prototype, not an injection-molded or metal housing. Structural integrity depends heavily on print quality and material. | "Studio and rehearsal controller; community members have reinforced enclosures for light stage use." |
| "Precise MIDI timing / sync" | BLE HID latency is real; the device is a keyboard, not a MIDI clock source | "Great for page turning, DAW transport, and hands-free control. Not designed for sample-accurate BLE timing." |
| "Indestructible" / "Stomp-proof" | Even PETG bolts and force-isolation design cannot match a steel enclosure | "Designed to protect the electronics from normal foot-press forces in a studio setting." |
| "Works with everything" | iOS is more restrictive; some apps do not accept BLE HID; Windows pairing is occasionally fiddly | "Tested on macOS, iOS, Android, and Windows. A few apps require specific pairing steps." |
| "Fits in any enclosure" | Metal enclosures (steel sheet, aluminium stomp-box cases) block or severely attenuate 2.4 GHz BLE signals. A fully enclosed metal box will degrade range to centimetres or kill the connection entirely. | "Works in any non-metal enclosure. For metal builds, use an ESP32 module with an external antenna connector and route the antenna outside the case." |

### 1.3 The Honest Sweet Spot

AwesomeStudioPedal sits in the **"smart page-turner / studio controller"** category, not the
"live-stage stomp box" category. Its true competitors are products like the Airturn Duo 500 (€99)
and IK Multimedia iRig BlueTurn (€75) — soft-touch BLE foot controllers for seated and studio
performers — not Boss or Strymon floor pedals.

**The competitive edge against those products is:**

1. Fully open-source — no black-box firmware, no vendor app required.
2. Arbitrary key mappings via JSON — any key combination, any app, any OS.
3. Multi-profile switching (7 profiles vs. typically 1–2 on commercial units).
4. Community-owned and extensible — builders can add buttons, swap hardware, fork the firmware.
5. Cost: BOM is well under €30; the commercial equivalents charge €75–€159 for similar capability.

---

## 2. The Persona Matrix

Four distinct audiences will encounter this project. Content should be deliberately layered so
any entry point leads naturally to the others.

### Persona A — The Maker-Musician *(primary audience, "North Star")*

**Who they are**: A guitarist, pianist, producer, or singer-songwriter who runs a home studio or
small rehearsal space. They own a soldering iron. They have used PlatformIO or at least flashed
firmware before. They are comfortable in JSON. They are frustrated that commercial controllers are
either too dumb (single-purpose page turners) or too expensive and locked-down (Airturn app
ecosystem).

**What they want**: A foot controller that works exactly the way they want, right now, without
waiting for a firmware update from a vendor.

**Where they spend time**: GroupDIY Audio Forum, Hackaday, YouTube maker channels, GitHub.

**The pitch**: "The open-source foot controller that finally puts *you* in control — not a vendor's
app."

---

### Persona B — The 3D Printing Enthusiast *(secondary audience)*

**Who they are**: A regular contributor to Printables or MakerWorld. They are less interested in the
music application than in the mechanical engineering challenge. They want interesting CAD, thoughtful
material choices, and a design that showcases their printer's capabilities.

**What they want**: A well-designed print with a clear BOM, honest print settings, and a result
that is visually satisfying on the shelf even before it is used.

**Where they spend time**: Printables.com, MakerWorld, Reddit r/3Dprinting.

**The pitch**: "An electronics project with a genuinely clever mechanical design — force isolation,
LED light pipes, modular pedal stack."

**Important caveat for this audience**: Be explicit that the current enclosure is a **studio-use
prototype**. It is not designed for "stomping" in the way a Metal Zone pedal can be kicked across
a stage. This is honest, and the audience will respect it. It also invites them to *improve* the
design — which is exactly what a good open-source hardware project does.

---

### Persona B2 — The Craftsperson *(secondary audience, parallel to B)*

**Who they are**: A guitarist or musician who is comfortable with hand tools and traditional
fabrication — woodworking, sheet metalworking (bending and drilling 1–2 mm *Blech*), or
sourcing off-the-shelf stomp-box enclosures (Hammond 1590-series or equivalent aluminium
die-cast boxes). They want their pedal to look like *real gear*: wood grain, a drilled and
painted metal top panel, knurled switches, a professional finish. They find 3D-printed
plastic aesthetically unsatisfying for a permanent build.

They are a specialisation of Persona A. Unlike the pure 3D-printing enthusiast (Persona B),
they play the instrument — the pedal is a tool for their music, not a showcase for a printer.
Unlike the Maker-Musician (Persona A), their build path is the workshop, not the printer
and breadboard.

**What they want**: Build instructions, a drilling template, and a BOM that includes
soft foot switches (the standard 12 mm momentary switches used in guitar-pedal builds)
rather than 24 mm arcade-style buttons. They want confidence that the result will look
and feel professional on the floor next to their other pedals.

**Where they spend time**: DIYStompboxes.com, GroupDIY Audio Forum, YouTube guitar-pedal
build channels, Reddit r/diypedals.

**The pitch**: "Build it in wood or metal. It looks like the rest of your pedalboard —
because you built it the same way."

**Critical technical constraint for this audience — BLE and metal enclosures:**

Standard guitar-pedal enclosures are aluminium die-cast (Hammond 1590-series) or steel.
Both materials attenuate 2.4 GHz RF signals significantly. A fully enclosed metal box
will reduce BLE range to a few centimetres or break the connection entirely. This is not
negotiable physics and must be addressed honestly and practically in any content aimed at
this persona. Three viable approaches:

| Approach | How | Trade-off |
|---|---|---|
| **Non-metal lid** | Metal body + wood or acrylic top panel. The ESP32's PCB antenna faces the non-metal surface. | Looks slightly hybrid, but this is actually common in boutique pedal builds and can look intentional. |
| **All-wood or all-acrylic enclosure** | Routed or laser-cut enclosure with soft foot switches in standard drill holes. Completely RF-transparent. | Requires woodworking skill or a laser cutter. Wood is less durable than metal under heavy stomping without reinforcement. |
| **External antenna mod** | Use an ESP32-WROOM-32U (the U suffix = U.FL/IPEX connector). Add a short pigtail cable terminating in an SMA bulkhead connector mounted in the enclosure wall. The antenna sits outside the metal box. | Small additional BOM cost (~€2–5 for the pigtail + bulkhead). Requires soldering a connector or buying the pre-wired module variant. The SMA jack on the outside of a metal box is a legitimate look in professional hardware. |

The non-metal lid approach is the recommended default to document because it requires no
special ESP32 variant and no antenna soldering. The external antenna approach should be
documented as the option for builders who want a fully enclosed metal build.

---

### Persona C — The Software / Embedded Developer *(tertiary audience)*

**Who they are**: A developer with embedded systems or Bluetooth experience who wants a
real-world open-source project to learn from or contribute to. They read Hacker News, follow
ESP32 projects, and value code quality over feature count.

**What they want**: An architecturally clean codebase they can read, learn from, fork, and extend.

**Where they spend time**: Hacker News, GitHub, r/embedded, c't Make (Heise).

**The pitch**: "A production-quality BLE HID implementation on ESP32, cleanly separated into
hardware, logic, and config layers."

---

### Persona D — The Pure Musician *(aspirational, reached through A)*

**Who they are**: A performing musician who has heard about this from a maker friend or a
music-tech blog. They have no interest in building or coding. They might buy a pre-built unit from
a community member, or they might be using the Simulator first.

**What they want**: To understand if this solves their problem (page turning, hands-free recording
control, DAW transport) *in plain language*.

**Where they spend time**: YouTube music tutorials, Reddit r/WeAreTheMusicMakers, music school
communities.

**The pitch**: "Turn any Bluetooth device into a hands-free studio assistant — no app, no driver,
just press a button."

---

## 3. The Core Brand Message

Every piece of content ultimately expresses one of these truths:

> **AwesomeStudioPedal**: Open-source. Wireless. Yours.

Breaking it down:

- **Open-source** → no black box, no vendor dependency, inspect and modify everything.
- **Wireless** → BLE HID, driverless, works with macOS / iOS / Android / Windows out of the box.
- **Yours** → JSON profiles, multi-mode, extensible firmware, community-driven hardware.

### Supporting Value Propositions (in priority order)

1. **Driverless "plug and play" Bluetooth** — pair once, works everywhere, no app.
2. **Multi-profile switching** — 7 configurable modes, instant LED feedback.
3. **JSON configuration** — change every button without touching code.
4. **Time-delayed actions** — the unique feature no competitor has (trigger and step away).
5. **Fully open** — firmware, CAD, JSON schema, toolchain — all on GitHub under MIT.
6. **Community cost** — BOM under €30, no vendor markup.

---

## 4. The Content Matrix

For each piece of content: what, for whom, on which channel, in what format.

| # | Content Piece | Primary Persona | Channel | Format | Status |
|---|---|---|---|---|---|
| 1 | README intro section | All | GitHub README | 2-paragraph plain prose | Draft exists (needs musician-friendly revision) |
| 2 | Feature list (North Star) | Maker-Musician | README / feature page | Scannable list with musical benefits | Draft in `idea-016-feature-list-article.md` |
| 3 | Maker-Musician long article | A | Music-tech blog / Hackaday | 800–1200 word feature | Editorial handbook in `idea-016-musician-article.md` |
| 4 | Developer deep-dive | C | HN / embedded blog / c't Make | 1000–1500 word technical audit | Editorial handbook in `idea-016-developer-article.md` |
| 5 | Builder/3D print article | B | Printables.com / c't Make | Dual-audience: engineering + print guide | Editorial handbook in `idea-016-builder-article.md` |
| 6 | Printables.com model description | B | Printables.com | 200-word product copy + tech table | Needs writing |
| 7 | Use-case scenarios | D | README / musician docs | Short scenario blocks | Draft in `idea-015-marketing-material.md` |
| 8 | Competitor positioning | Internal | Internal reference only | Table | Done in `idea-015` |
| 9 | Forum pitch templates | A, C | GroupDIY / Hackaday | 3-5 sentence intro | Needs writing |
| 10 | Video script framework | A, D | YouTube | Narrated walkthrough | Not started |
| 11 | Craftsperson build guide | B2 | DIYStompboxes / GroupDIY | 800–1200 word build article + drilling template | Needs writing — see §5.6 |
| 12 | BLE + enclosure material technical note | B2 | README / BUILD_GUIDE / builder docs | Short technical reference with the three approaches | Needs writing — see §5.7 |

---

## 5. Deliverable Specifications

### 5.1 README Intro Revision (Content #1)

The current README opens with a good technical summary but is written for someone who already
understands what a BLE HID controller is. The revision should lead with the musician's problem.

**Proposed structure:**

```
[Hero image of pedal in use]
[One-sentence problem] + [One-sentence solution]
[Persona table — already exists, keep it]
[3-5 bullet feature highlights]
[Technical summary — already exists, keep it]
```

**Draft hook paragraph:**
> You are recording. Both hands are occupied. The punch-in button is across the room.
> AwesomeStudioPedal is a wireless Bluetooth foot controller — it appears as a keyboard
> on any device, no driver, no app, no cable. Press a button: punch in, turn the page,
> trigger the shutter, change the patch. Seven profiles, one pedal, your rules.

---

### 5.2 North Star Feature List (Content #2)

The draft in `idea-016-feature-list-article.md` is the best single piece of writing in the
existing idea files. It should become a dedicated page or prominent README section.

**Required adaptation for this project's reality:**

The draft mentions "PETG-Flex Fasteners" and "Stomp-Safe Chassis" as a marketing headline. This
needs careful rewriting to be honest. Replace:

> ~~"Modular 'Stomp-Safe' Chassis: A 3D-printed enclosure designed for structural isolation..."~~

With something like:

> **Studio-Proof Construction**: A modular 3D-printed enclosure designed to transfer foot-press
> forces to the outer shell, protecting the ESP32 and solder points during normal studio and
> rehearsal use. CAD files are parametric — builders adapt the design for their own pedalboard.

This stays true to the mechanical design intent without overclaiming.

**Full section rewrites needed:**

| Original claim | Issue | Revised framing |
|---|---|---|
| "Sub-10ms latency — essential for tight DAW punching" | BLE latency is variable; "essential" is a stretch | "Optimised BLE stack targeting <10ms average command latency for responsive DAW transport control" |
| "PETG-Flex Fasteners" (as a headline feature) | Sounds odd to a musician | Move to builder section; replace headline with "Modular, Serviceable Design" |
| "Future-Proof Logic" | Vague marketing language | "Extensible architecture: Long-Press macros, Double-Tap, and MIDI transport are all planned extensions in the open issue tracker" |

---

### 5.3 Maker-Musician Article (Content #3)

The editorial handbook in `idea-016-musician-article.md` is a *writing guide*, not a finished
article. The actual article needs to be written, following the handbook's five-step structure:

1. **The Session Hook** — lead with a studio problem, not a spec list.
   Suggested scenario: *A solo performer recording at 11pm needs to punch in vocal takes without
   walking to the DAW between each one.*

2. **The Specs (translated to benefits)** — use the North Star feature list, translated:
   - BLE HID → "No driver, instant pairing, works with your iPad on stage and your Mac in the studio"
   - JSON Profiles → "Change every button assignment in a text editor — no recompile, no app update"
   - 7 Profiles → "DAW mode, Page-Turner mode, OBS mode — switch in one step"
   - Time-Delayed Action → "Trigger the camera shutter and step into frame in 5 seconds"

3. **The Code Audit (1-2 paragraphs)** — non-blocking architecture means the pedal always listens;
   JSON abstraction means remapping is a 30-second edit.

4. **The Builder's Journey** — honest timeline: one evening to source parts, one afternoon to
   print and assemble, one hour to flash and configure.

5. **Customisation Ideas** — examples: orchestral pit page-turner, podcast recording remote,
   Lightroom culling pedal, OBS scene switcher.

**Target publications**: Hackaday (free submission), Music Tech Magazine (pitch), AudioTechnology,
German: c't Make sidebar, Amazona.de.

---

### 5.4 Developer Deep-Dive (Content #4)

The handbook in `idea-016-developer-article.md` already provides the exact structure. The article
needs to be written. Key sections:

- **Architecture block diagram** (BLE stack / Logic Layer / Config / GPIO)
- **Highlight the JSON abstraction**: This is genuinely clever — the config lives in LittleFS and
  is parsed at runtime, meaning the firmware never needs reflashing for remapping.
- **The FSM**: Profile switching and delayed-action logic are clean state machine problems — worth
  auditing positively.
- **Honest gaps**: Note the nRF52840 port is unvalidated, and the BLE connection interval tuning
  is an open area.
- **Hack Value**: Easy to fork into: smart home controller, presentation remote, accessibility
  device, industrial machine trigger.

**Target publications**: Hacker News "Show HN" post, Hackaday.io project page, c't Make.

---

### 5.5 Builder / 3D Print Article (Content #5)

This serves two readers and should probably be *two separate documents*:

**5.5a — c't Make Engineering Audit**

Audience: Engineers and developers who enjoy building. Focus on:

- Dual-core ESP32 architecture (BLE on Core 0, application on Core 1)
- LittleFS / JSON config layer — how it decouples config from firmware
- GPIO debounce implementation
- HID-over-GATT profile and report descriptor design

**5.5b — Printables.com Model Description**

Audience: 3D printing enthusiasts. Focus on:

- Print time, estimated filament (grams per material)
- Recommended print settings: 3–4 perimeters, 25% gyroid infill for structural parts
- Material rationale: PLA for visible surfaces, PETG for mechanical parts (better flex/fatigue life)
- LED light pipe design: translucent / white PLA diffuses LED glow
- Assembly: printed threads vs. M3 inserts, cable management
- **Honest difficulty rating**: "Intermediate. Requires a printer calibrated to ±0.2mm for
  button fits. Some trimming of button holes is expected."
- **Honest use-case rating**: "Studio and rehearsal use. The enclosure is not designed for
  aggressive outdoor stomping. It handles the force of a normal foot press reliably."

---

### 5.6 Printables.com Model Description (Content #6)

This is its own short deliverable. The project already has a Printables listing
(`https://www.printables.com/model/1683455-awesomestudiopedal`). The description needs:

```
Short hook (2 sentences)
What it does (non-technical, musician-first)
Print specs table: materials / time / weight / difficulty
Assembly notes
Links: GitHub, Build Guide, User Guide
```

---

### 5.7 Use-Case Scenario Cards (Content #7)

Short, self-contained blocks that can be embedded in README, used in articles, dropped in forums.
Each card follows this micro-format:

```
**[Scenario title]**
*Who*: [one sentence persona description]
*Problem*: [one sentence]
*Setup*: [two-step config description]
*Result*: [one sentence, concrete benefit]
```

Proposed set of 6 cards:

1. **The Vocal Punch-In** — solo home-recording producer, hands full, DAW punch-in button far away.
2. **The Sheet Music Turner** — classical pianist performing from tablet, needs page down hands-free.
3. **The Delayed Camera Trigger** — solo performer, trigger shutter and step into frame in 5 sec.
4. **The Practice Session Launcher** — configure profile to start metronome + open score + arm track.
5. **The OBS Scene Switcher** — streamer/YouTuber switches camera or scene without leaving frame.
6. **The Podcast Mute Toggle** — podcaster mutes/unmutes mic with a foot press during recording.

Note on the "starting a backing track in sync" scenario from the existing idea-015 draft: this was
correctly flagged as out-of-scope (BLE HID is not a clock source). That scenario card should not
appear in marketing material. The delay-trigger card is fine because it is a *time-to-fire* delay
(user-defined seconds), not a sample-accurate sync.

---

### 5.6b Craftsperson Build Guide (Content #11)

Audience: Persona B2 — musicians who build with traditional materials (wood, sheet metal,
off-the-shelf stomp-box enclosures). Target channels: DIYStompboxes.com forum, GroupDIY,
YouTube build-log style video or post.

**Article structure:**

1. **The premise** — "You already know how to drill a Hammond box. Here's the firmware half."
   This persona does not need to be taught to fabricate; they need to know the electronics are
   straightforward and the firmware is open.

2. **Enclosure material options** — presented as a choice the builder makes, not a prescription:
   - **Wood**: completely RF-transparent, traditional look, pairs well with soft foot switches and
     recessed LED bezels. Router or laser-cut for precision holes.
   - **2 mm sheet metal (Blech)**: professional workshop aesthetic, requires a drill press and
     step drill bit for clean 12 mm holes. Powder-coat or rattle-can finish.
   - **Off-the-shelf Hammond 1590BB / equivalent**: the fastest path to a finished-looking pedal.
     Aluminium die-cast. Fits 2–4 buttons easily, available in various colours pre-drilled or blank.

3. **The BLE + metal enclosure problem — addressed directly** (see §5.7):
   Readers in this community know that metal enclosures affect RF. They will ask. Answer it upfront
   and present the three options (non-metal lid, wood/acrylic build, external antenna mod) without
   burying them in a footnote. For a metal enclosure the external antenna mod is the clean solution
   and is honestly not difficult — it is the same skill as adding an input jack to a guitar pedal.

4. **Soft foot switches** — these builders already stock these:
   - Standard 3PDT or SPST momentary, 12 mm thread, fits standard pedal enclosure drill holes.
   - No adapter needed. The firmware doesn't care about switch type — any normally-open momentary
     connects directly to the ESP32 GPIO with the internal pull-up enabled.
   - A 4-button Hammond 1590BB build with soft switches is physically indistinguishable from a
     commercial boutique pedal.

5. **Drilling template** — a printable PDF showing the hole positions for a 4-button + 1 SELECT
   layout on a 1590BB footprint, with LED positions. This is the practical deliverable that makes
   this article genuinely useful over a generic "you could do this" post.

6. **LED visibility** — in metal and wood enclosures, LEDs need recessed bezels or drilled holes
   with light pipes. Standard 5 mm LED bezels (chrome or black) screw into a 5 mm hole and look
   professional. Mention this — it is a detail the audience will care about.

**Tone**: peer-to-peer, workshop-fluent. This audience respects expertise in fabrication. Lead with
the physical build, treat the firmware as the straightforward part, not the other way around.

---

### 5.7b BLE + Enclosure Material Technical Note (Content #12)

A short, permanent reference in the builder documentation. Not an article — a technical note
that lives in `docs/builders/` and is linked from `BUILD_GUIDE.md`, the BOM (IDEA-018), and
the craftsperson article.

**Content:**

```
## Enclosure Materials and BLE

The ESP32's 2.4 GHz antenna requires a line of sight through non-conductive material.

| Enclosure material | BLE effect | Recommendation |
|---|---|---|
| Wood | None — fully RF-transparent | No restrictions on ESP32 placement |
| 3D-printed PLA/PETG | None — fully RF-transparent | No restrictions |
| Acrylic / polycarbonate | None — fully RF-transparent | No restrictions |
| Aluminium (Hammond enclosures) | Significant attenuation inside a closed box | Use non-metal lid OR external antenna module |
| Steel / sheet metal (Blech) | Severe attenuation / connection loss inside a closed box | External antenna module required for reliable connection |

### Option 1 — Non-metal lid (simplest)

Use a metal body with a wood, acrylic, or 3D-printed top panel. The ESP32's PCB antenna
faces the non-metal surface. No hardware changes required.

### Option 2 — All-wood or all-acrylic enclosure

Build the entire enclosure from RF-transparent material. Soft foot switches (12 mm
threaded, standard guitar-pedal type) fit 12 mm drill holes without adapters.

### Option 3 — External antenna (fully enclosed metal)

Use an ESP32-WROOM-32U (note the "U" suffix — it has a U.FL/IPEX antenna connector
instead of the built-in PCB antenna). Add a short U.FL-to-SMA pigtail cable and mount
an SMA bulkhead female connector in the enclosure wall. The external antenna (stub or
whip) sits outside the metal shell.

Parts needed:
- ESP32-WROOM-32U module (or NodeMCU-32S with U.FL variant)
- U.FL to SMA pigtail, ~10 cm (~€2)
- SMA bulkhead connector, panel-mount (~€1)
- 2.4 GHz stub antenna (~€1–3)

The SMA connector on the outside of a metal enclosure is a recognisable professional
aesthetic — it is used in commercial RF test equipment and boutique wireless audio gear.
```

---

### 5.8 Forum Pitch Templates (Content #9)

Short, authentic introductions for posting in community spaces. These should never read as ads.

**GroupDIY Audio Forum template:**
> "I built a wireless foot controller for my studio — BLE HID, works driverless with any DAW on
> macOS/iOS/Windows/Android. JSON-configurable, 7 profiles, open-source on GitHub. Happy to share
> schematics and firmware. [link] Built it because page-turners are dumb and Airturn costs too much."

**Hackaday.io template:**
> "Show HN: AwesomeStudioPedal — open-source BLE HID foot controller for DAW / page-turning /
> studio automation. ESP32 + ArduinoJson + LittleFS. No proprietary app, no driver. 7 remappable
> profiles stored in JSON on-device. The killer feature: configurable time-delayed actions.
> MIT license. [GitHub link] Feedback welcome on the HID descriptor and debounce logic."

**Reddit r/WeAreTheMusicMakers template:**
> "I built my own wireless foot pedal for hands-free DAW control — page turning, punch-in,
> OBS switching, whatever. It pairs like a keyboard, no app needed. Open-source if you want to
> build one (~€25 in parts). [link]"

---

## 6. Content Production Sequence

The order matters: foundation pieces should be finished before articles are published, because
every article links back to the README and Build Guide.

```
Phase 1 — Foundation (do first)
├── README intro revision (#1)
├── North Star Feature List, adapted for project reality (#2)
└── 6× Use-Case Scenario Cards (#7)

Phase 2 — Community Platform Copy (depends on Phase 1)
├── Printables.com model description (#6)
└── Forum pitch templates (#9)

Phase 3 — Long-Form Articles (depends on Phase 1)
├── Maker-Musician article (#3) — target Hackaday
├── Developer deep-dive (#4) — target HN / c't Make
├── Builder / c't Make audit (#5a)
├── 3D Print guide (#5b)
└── Craftsperson build guide (#11) — target DIYStompboxes / GroupDIY

Phase 2b — Technical Reference (can run in parallel with Phase 2)
└── BLE + enclosure material note (#12) — lives in docs/builders/

Phase 4 — Video (depends on Phase 3 for framing)
└── Video script framework (#10) — narrated build + demo walkthrough
```

---

## 7. What Already Exists (Do Not Recreate)

The idea files contain substantial completed work. Before writing anything new, check here:

| Asset | Location | Status |
|---|---|---|
| Competitor analysis (8 products, pricing, user feedback) | `idea-015-marketing-material.md` §Research Insights | Complete |
| Maker-Musician editorial handbook | `idea-016-musician-article.md` §The Maker-Musician Editorial Handbook | Complete |
| c't Make engineering editorial handbook | `idea-016-builder-article.md` §The "c't Make" Editorial Handbook | Complete |
| 3D Printing editorial handbook | `idea-016-builder-article.md` §The 3D-Printing Editorial Handbook | Complete |
| Developer code-audit editorial handbook | `idea-016-developer-article.md` §The Developer's Deep-Dive Handbook | Complete |
| North Star feature list (raw draft) | `idea-016-feature-list-article.md` | Draft — needs adaptation |
| Draft README/marketing copy | `idea-015-marketing-material.md` §Draft Marketing Material | Draft — needs polish |
| Use-case scenario drafts (4 scenarios) | `idea-015-marketing-material.md` §Concept for Showing Use Cases | Draft — use as source |

---

## 8. Tone and Language Guardrails

Distilled from all four editorial handbooks:

### Use

- "Hands-free" — concrete, universally understood
- "No driver" / "No app" — addresses a real pain point every musician has hit
- "Profile" — familiar to musicians (amp presets, DAW templates)
- "Studio floor" / "rehearsal room" — honest about use context
- "Community-supported" / "open-source" — factual and a genuine differentiator
- "Punch-in" / "page turn" / "scene switch" — concrete use cases

### Avoid

- "Revolutionary" / "game-changing" — empty superlatives
- "Stomp-proof" / "stage-ready" — overclaims the enclosure's durability
- "Precise sync" / "tight timing" — BLE HID is not a clock source
- "Works with everything" — too broad, pairing edge cases exist
- "Professional grade" without qualification — it is a DIY project
- "Easy" without context — depends entirely on the reader's background
- "Works in any enclosure" — metal enclosures block BLE; this is not a soft caveat, it is a hard physics fact that must be addressed before claiming broad compatibility

### The Humour Trap

The existing draft marketing copy in `idea-015` contains fictional testimonials with humorous
framing (Ludwig van Footworks, Jimmy "The Maestro" McKeys, etc.). This kind of humour works in
internal ideation but **must not appear in published marketing material**. It signals that the
project is not serious. Reserve humour for community channels like Discord or GitHub Discussions
where tone is already established.

---

## 9. Open Questions and Decisions Needed

Before Phase 1 content can be finalized, the following need answers:

| # | Question | Why it matters | Owner |
|---|---|---|---|
| Q1 | What is the official "primary use case" the project wants to be known for? (Page turner? DAW transport? Universal controller?) | Determines the hook for every piece of content | Tobias |
| Q2 | Is there a plan for a more robust enclosure (PCB + metal shell, see IDEA-011)? | If yes, "studio prototype" messaging can include a roadmap reference. If no, we commit to honest DIY framing permanently. | Tobias |
| Q3 | Are there real users willing to share a photo or brief quote? | The difference between "we claim it works" and "here is someone using it" is enormous for credibility. Fictional testimonials do not substitute. | Tobias |
| Q4 | Which publication or community should receive the first long-form article? | Determines which editorial handbook to execute first (Hackaday vs. c't Make vs. Printables). | Tobias |
| Q5 | Is there video footage of the pedal in use? | A 30-second clip of a page being turned hands-free is worth 10 articles for the musician persona. | Tobias |
| Q6 | Has the BLE antenna behaviour inside a metal enclosure been tested? | The technical note in §5.7b is based on known 2.4 GHz RF physics, but a confirmed test result (e.g., "ESP32-WROOM-32U + SMA bulkhead in a 1590BB gives full range") would make the documentation concrete rather than theoretical. | Tobias |
| Q7 | Is a printable drilling template for the Hammond 1590BB a reasonable scope item, or should it link to an existing guitar-pedal drill template resource? | The drilling template is the most practically useful deliverable for the craftsperson persona, but it requires CAD or precise layout work. | Tobias |

---

## 10. Summary: The Minimum Viable Content Launch

If resources allow only a limited first push, prioritize in this order:

1. **Rewrite the README intro** (two paragraphs, musician-first, links to all personas).
2. **Write the 6 scenario cards** and embed them in the musician docs.
3. **Post a Printables.com description update** with honest print specs and a link to the Build Guide.
4. **Write one Maker-Musician article** and submit to Hackaday.

These four deliverables cover the three most reachable audiences (musicians via scenario cards,
builders via Printables, makers via Hackaday) with minimal writing effort and establish the honest,
credible voice the project deserves.
