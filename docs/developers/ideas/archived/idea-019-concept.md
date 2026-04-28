# Circuit & Wiring Diagrams as Code — Research

> **DEPRECATED** — This concept document is superseded by the simplified solution.
> See [idea-019-simplified-solution.md](idea-019-simplified-solution.md).

## 1. Problem statement

In systems engineering, textual notations are replacing GUI-driven tools:

- **SysML v2** → replaces Rhapsody / Cameo
- **ReqIF / Sphinx-Needs / OpenFastTrace** → replace DOORS
- **PlantUML / Mermaid / Structurizr DSL** in Markdown → replace Visio for software architecture

For **electrical schematics, wiring diagrams, and PCB design** there is no equivalent first-class docs-as-code workflow. KiCad, Eagle, Altium and Fritzing are GUI-driven and store binary or non-diff-friendly formats. Goal:

> A textual notation for circuits that (a) lives next to source code, (b) renders inline in Markdown like Mermaid, (c) exports cleanly (one-way) to at least one production EDA tool (KiCad) — full round-tripping / back-annotation from PCB edits is *not* a goal, see §9.1 K7, (d) survives review/diff/merge in Git, and (e) serves both the engineer authoring the design and the builder reproducing the device.

## 2. Two personas

The documentation must serve two distinct readers. Each cares about different views derived from the same source.

| Aspect | **Engineer** (author of the circuit) | **Builder** (reproduces the device) |
|---|---|---|
| Primary view | Schematic (logical), netlist, ERC results, parameters/tolerances | Wiring diagram (physical), pinout tables, BOM, step-by-step assembly |
| Cares about | Correctness, reuse, assertions, PCB export | Which wire goes where, color codes, part numbers, photo/3D of the board |
| Edits the source? | Yes | No (reads only) |
| Tolerates a build toolchain? | Yes — willing to install compilers, EDA libs, etc. | No — should be able to read the rendered docs without installing anything |
| Wants inline rendering in Markdown? | Yes (review during edit) | Yes (read-only consumption on the docs site, or via the committed SVG when browsing the repo on GitHub — note that GitHub does *not* render any of the circuit DSLs natively; see §9) |
| Best matched tools | atopile / SKiDL / tscircuit | WireViz (harness) + schematic SVG and BOM rendered from the engineer's DSL |

Implication: a single source (the engineer's DSL) must produce **multiple derived artefacts** — schematic SVG, wiring diagram, pinout table, BOM — each rendered into the appropriate Markdown page for the appropriate persona. (A textual assembly view is out of scope — see §9.)

## 3. Requirements catalogue

| # | Requirement | Engineer | Builder |
|---|---|---|---|
| R1 | Plain-text source, Git-friendly diffs | ✓ | — |
| R2 | Inline rendering in Markdown | ✓ | ✓ |
| R3 | Deterministic / stable layout | ✓ | ✓ |
| R4 | Hierarchical / modular composition | ✓ | — |
| R5 | Component library / parts metadata → BOM | ✓ | ✓ |
| R6 | Export to a real EDA format (KiCad netlist, Gerber path) | ✓ | — |
| R7 | Migration path from existing designs (via KiCad) | ✓ | — |
| R8 | ERC / assertions | ✓ | — |
| R9 | Build-system integration (cache invalidation, watch mode) | ✓ | — |
| R10 | LSP / formatter | ✓ | — |
| R11 | Wiring-harness / pinout view distinct from schematic | — | ✓ |
| R12 | Auto-generated BOM table next to the diagram | ✓ | ✓ |

## 4. Landscape of candidate tools

### 4.1 Render-only textual notations (good for docs, weak for engineering)

#### Schemdraw (Python)

```python
import schemdraw
import schemdraw.elements as elm

with schemdraw.Drawing(file='vdiv.svg'):
    elm.SourceV().up().label('5V')
    elm.Resistor().right().label('1kΩ')
    elm.Resistor().down().label('2kΩ')
    elm.Line().left()
```

- **Strengths**: beautiful output, large element library, easy install.
- **Weaknesses**: uses Python as the host language rather than a dedicated grammar (see §4a) — no netlist, no ERC, manual layout, no PCB export.

#### CircuiTikZ (LaTeX/TikZ)

```latex
\begin{circuitikz}
  \draw (0,0) to[V, l=5V] (0,2)
        to[R, l=1k] (2,2)
        to[R, l=2k] (2,0) -- (0,0);
\end{circuitikz}
```

- **Strengths**: publication-grade, mature, huge symbol set.
- **Weaknesses**: heavy LaTeX toolchain, manual coordinates, no semantics or netlist.

### 4.2 Hardware-description-as-code (real engineering tools)

#### SKiDL (Python eDSL → KiCad)

```python
from skidl import *

vin, vout, gnd = Net('VI'), Net('VO'), Net('GND')
r1, r2 = 2 * Part("Device", 'R', dest=TEMPLATE,
                  footprint='Resistor_SMD:R_0805_2012Metric')
r1.value, r2.value = '1K', '2K'
vin & r1 & vout & r2 & gnd
ERC()
generate_netlist(tool=KICAD9)  # KICAD9 = SKiDL constant targeting KiCad 9.x netlist format; use KICAD8 / KICAD7 for older toolchains
```

- Mature. `netlist_to_skidl` imports existing KiCad designs. Hierarchical, ERC, BOM. Note the version split: netlist generation targets modern KiCad (use `tool=KICAD9`), but SKiDL's built-in SVG schematic renderer still requires KiCad v5-format symbol libraries to be installed alongside — see the §11 pin note and §8 Option B caveat.

#### atopile (`.ato` declarative DSL → KiCad PCB)

```ato
import ElectricPower
import Resistor

module VoltageDivider:
    power_in  = new ElectricPower
    power_out = new ElectricPower

    r1 = new Resistor; r1.resistance = 1kohm  +/- 1%
    r2 = new Resistor; r2.resistance = 2kohm  +/- 1%

    power_in.hv ~> r1 ~> power_out.hv ~> r2 ~> power_in.lv
    power_out.lv ~ power_in.lv
```

- Modern. Units, tolerances and assertions are first-class. Compiler updates `.kicad_pcb` directly. Package registry, VS Code extension.

#### tscircuit (TypeScript / React JSX)

```tsx
export default () => (
  <board width="20mm" height="10mm">
    <resistor name="R1" resistance="1k" footprint="0805" pcbX={-2} pcbY={0} />
    <resistor name="R2" resistance="2k" footprint="0805" pcbX={ 2} pcbY={0} />
    <trace from=".R1 > .pin2" to=".R2 > .pin1" />
  </board>
)
```

- Browser-native rendering (true Mermaid-like inline). React component model. KiCad import/export, Gerber export, autorouter. Strong fit when the docs site is web-based.

### 4.3 Wiring-harness specific (the Builder's primary view)

#### WireViz (YAML → SVG + BOM)

```yaml
connectors:
  X1:
    type: D-Sub
    subtype: female
    pinlabels: [GND, RX, TX]
  X2:
    type: Molex KK 254
    subtype: female
    pinlabels: [GND, RX, TX]

cables:
  W1:
    gauge: 0.25 mm2
    length: 0.2
    color_code: DIN
    wirecount: 3

connections:
  - - X1: [1, 2, 3]
    - W1: [1, 2, 3]
    - X2: [1, 3, 2]
```

- YAML, Graphviz-rendered SVG, auto-BOM (TSV), color codes (DIN/IEC), gauges. Exactly the view the **Builder** needs. Not for PCB schematics.

## 4a. The DSL itself, judged as a docs-as-code citizen

A DSL can be powerful for engineering yet poor as a docs-as-code source (and vice versa). This dimension is independent of "does it generate a netlist" and deserves its own scoring.

### 4a.1 Criteria

| Criterion | Why it matters for docs-as-code |
|---|---|
| **C1 Declarative vs. imperative** | Declarative reads top-to-bottom like a spec. Imperative (Python script with side effects) reads like code; the diagram is hidden behind execution. |
| **C2 Source readable without the renderer** | Can a reviewer in a GitHub PR understand the diagram from the text alone? |
| **C3 Diff-friendly** | Small semantic change → small textual diff. Coordinates and auto-generated IDs are diff-hostile. |
| **C4 Order-independence / canonical form** | Reordering blocks should not change meaning or output. Otherwise diffs churn. |
| **C5 Schema / grammar** | Tooling, linting, autocompletion, AI assistance all benefit. YAML/JSON/grammar-defined > "anything Python can express". |
| **C6 Composition / include** | Can large diagrams be split across files like source code? |
| **C7 Inline-in-Markdown ergonomics** | Short, indentation-tolerant, no escape-hell inside fenced blocks. |
| **C8 Toolchain weight** | A LaTeX install or a Node-and-React build for one diagram is a docs-as-code anti-pattern. |
| **C9 Stability / governance** | Will the source still parse in three years? Spec-level stability matters for documentation that outlives the engineer. |
| **C10 Self-describing identifiers** | Refs like `X1.pin3` beat opaque `connector0` for human readability. |

### 4a.2 Scoring

Legend: ✓ strong, ○ acceptable, ✗ weak.

| DSL | C1 declarative | C2 readable | C3 diff | C4 canonical | C5 schema | C6 compose | C7 MD-inline | C8 toolchain | C9 stability | C10 ids |
|---|---|---|---|---|---|---|---|---|---|---|
| **WireViz** (YAML) | ✓ | ✓ | ✓ | ✓ | ✓ | ○ flat | ✓ | ✓ Python+Graphviz | ✓ stable spec | ✓ |
| **atopile** (`.ato`) | ✓ | ✓ | ✓ | ✓ | ✓ grammar | ✓ modules | ○ multi-line, indent-sensitive | ○ ato compiler | ⚠ young, evolving (see K1) | ✓ |
| **CircuiTikZ** (LaTeX) | ✗ macro-imperative | ○ once you know it | ✗ coordinates | ✗ | ○ macro-defined | ✓ `\input` | ✗ LaTeX inside fenced block is painful | ✗ full LaTeX | ✓ very stable | ○ |
| **SKiDL** (Python eDSL) | ✗ imperative | ○ for Python readers | ○ depends on style | ✗ Python order matters | ✗ "anything Python" | ✓ modules | ✗ Python imports + setup needed inline | ○ Python + KiCad libs (v5 *and* v9) | ✓ mature | ✓ |
| **tscircuit** (TS/JSX) | ✓ JSX is declarative-ish | ✓ for JS readers | ✓ | ✓ | ✓ TS types | ✓ components | ✓ JSX is indent-tolerant and reads cleanly inline | ✗ Node+React build required | ⚠ young, evolving (see K1) | ✓ |
| **Schemdraw** (Python) | ✗ pure imperative drawing | ✗ imperative drawing calls; reader can't reconstruct topology without executing | ✗ | ✗ | ✗ | ○ | ✗ | ○ Python only | ✓ mature | ✗ ad-hoc |

### 4a.3 Observations

- **WireViz is the cleanest docs-as-code citizen** of the lot. Plain YAML, declarative, schema-able, diff-friendly, near-zero toolchain. It is an existence proof that an electrical-domain DSL can match Mermaid's docs-as-code feel — but only because its scope is narrow (harnesses, not full schematics).
- **atopile is the closest thing to "WireViz for schematics"** in the docs-as-code sense: a real grammar, declarative syntax, units as language primitives, composition via modules. Its weakness is youth (spec still evolving) and the indentation-sensitive syntax is awkward inside Markdown fenced blocks.
- **tscircuit is declarative-in-spirit (JSX) but imperative-in-toolchain.** A reader staring at `<resistor name="R1" resistance="10k" />` understands it instantly and the JSX is indent-tolerant inside a fenced block (good C2/C7), but the docs site needs Node+React to render it (bad C8). It buys back some of that cost by being browser-renderable like Mermaid — which is unique.
- **SKiDL and Schemdraw are not really DSLs** in the docs-as-code sense. They are Python libraries. A reviewer reading `vin & r1 & vout & r2 & gnd` sees an operator-overloading trick, not a specification. Order of statements has semantic effect (Schemdraw especially: ordering *is* layout). This is a fundamental docs-as-code mismatch even though the engineering output is excellent.
- **CircuiTikZ** is wonderful for typeset PDFs but the worst fit for docs-as-code Markdown: it pulls in LaTeX, and its source is coordinate-driven.

### 4a.4 Implication for the recommendation

The "best engineering DSL" and "best docs-as-code DSL" are not the same:

| Axis winner | DSL |
|---|---|
| Best engineering substrate | atopile / SKiDL / tscircuit (tie, depending on team — engineering-only axis; the docs-as-code axis below, and §14's recommendation, break the tie in favour of atopile) |
| **Best docs-as-code citizen (narrow)** | **WireViz** |
| **Best docs-as-code citizen (broad: schematics)** | **atopile** (leading candidate; the SKiDL alternative in §10/§14 is kept in the spike to validate the docs-as-code judgement against the engineering-side criteria) |
| Best inline-in-web-Markdown rendering | tscircuit (only one that's truly browser-native) |

## 5. Comparison against requirements

Legend: ✓ supported / strong, ⚠ partial or with caveats, ✗ not supported, — not applicable. **build-hook** = no native Markdown integration; rendering happens via a separate build step (pre-commit hook, MkDocs/Sphinx plugin, or `make` target — see §6 pattern 1) that generates SVG referenced from Markdown. **browser-side** = renders client-side via a JS bundle (§6 pattern 3). In the **R9 build** column, **manual** / **scriptable** = no built-in build orchestration; the user wires it into their own toolchain. In the **R3 stable layout** column, **⚠ hand-placed** means the author positions each component by hand in source coordinates (no autorouter / auto-layout) — a separate concern from build orchestration; **⚠ improving** means auto-layout exists but is still actively maturing, so expect layout churn between releases.

| Tool | R1 text | R2 inline MD | R3 stable layout | R4 hierarchy | R5 BOM | R6 EDA export | R7 KiCad migration | R8 ERC | R9 build | R10 LSP | R11 harness view | R12 BOM table |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Schemdraw   | ✓ | build-hook | ⚠ hand-placed | ⚠ | ✗ | ✗ | ✗ | ✗ | manual | basic² | ✗ | ✗ |
| CircuiTikZ  | ✓ | build-hook | ⚠ hand-placed | ⚠ | ✗ | ✗ | ✗ | ✗ | manual | basic² | ✗ | ✗ |
| **SKiDL**   | ✓ | build-hook | ⚠ improving | ✓ | ✓ | ✓ KiCad | ✓ `netlist_to_skidl` | ✓ | scriptable | basic² | ✗ | ✓ |
| **atopile** | ✓ | build-hook | ⚠ improving | ✓ | ✓ registry | ✓ KiCad PCB | ⚠ via KiCad⁴ | ✓ assertions | `ato build` | ✓ VS Code | ✗ | ✓ |
| **tscircuit** | ✓ | ✓ browser-side | ✓ autorouter | ✓ | ✓ registry | ✓ KiCad/Gerber | ⚠ KiCad import¹ | partial | `tsci dev` | ✓ TS LSP | partial | ✓ |
| **WireViz** | ✓ | build-hook | ✓ Graphviz | ⚠ flat | ✓ | ✗ | ✗ | ✗ | scriptable | basic³ | ✓ | ✓ |

¹ tscircuit's KiCad import covers footprints and PCB layout reliably; full schematic-symbol round-trip is partial — treat KiCad → tscircuit as a one-shot migration aid, not a continuous sync.

² "basic" entries mean only the generic host-language LSP, with no DSL-aware diagnostics: Schemdraw and SKiDL get Pylance/Pyright (Python); CircuiTikZ gets whatever LaTeX tooling the editor provides (TeX language server, latexmk, etc.). atopile and tscircuit have purpose-built language servers; WireViz uses YAML schema validation only — see footnote 3.

³ WireViz LSP is whatever your editor's YAML schema support provides against the WireViz YAML schema; there is no dedicated WireViz language server.

⁴ atopile's KiCad migration is forward-only in practice: footprints and PCB layout transfer cleanly, but the schematic-side import is partial — treat KiCad → atopile as a greenfield-only path.

## 6. Inline-in-Markdown rendering: integration patterns

1. **Build-time SVG generation + image reference** (Schemdraw, WireViz, SKiDL, atopile, CircuiTikZ).
   - Pre-commit hook, MkDocs/Sphinx plugin, or `make` target generates `*.svg` from source; Markdown references the image.
   - Schemdraw, WireViz, SKiDL and atopile (`ato build`) emit SVG directly. CircuiTikZ renders via LaTeX to PDF/PNG; an extra step (`dvisvgm`, or `pdf2svg` on the LaTeX output) is needed to land in SVG.
   - **Pros**: works with any Markdown renderer, including raw GitHub view.
   - **Cons**: stale-rendering risk; two-file mental model.
   - **Mitigations**: content-hashed filenames, pre-commit hooks, watch-mode dev server, CI guard `git diff --exit-code` after regen.

2. **Markdown plugin with fenced code block** (Mermaid/PlantUML pattern).
   - Renderer parses ` ```circuit … ``` ` blocks at build time and inlines SVG.
   - **Pros**: single source of truth, no separate file.
   - **Cons**: tied to one renderer; GitHub web UI won't render natively.
   - **Status: aspirational.** As of April 2026, none of the circuit DSLs surveyed here ship a ready-made MkDocs/Sphinx/Docusaurus plugin of this shape; adopting this pattern means writing the plugin yourself. This is consistent with §13's recommendation to *not* build a custom extension yet.

3. **Browser-side rendering as web component** (tscircuit pattern).
   - Ship a JS bundle; `<schematic>…</schematic>` renders client-side.
   - **Pros**: live, interactive (zoom, pan).
   - **Cons**: requires JS on the docs site; not Markdown-portable.

If the tscircuit fallback in §14 is later activated, the stack can combine (1) for portability + (3) for an interactive playground page; otherwise pattern 1 alone is the recommended path.

## 7. Discussion of the user-raised pain points

### 7.1 "What if I update the source — do I rerender manually?"

- **pre-commit hook** runs the generator and `git add`s the output, OR fails if the SVG is stale.
- **MkDocs/Sphinx plugins** with file-watcher in `mkdocs serve` → automatic rebuild on save.
- **CI guard**: regenerate in CI and fail if `git status --porcelain` is non-empty.
- Alternative: never commit images; generate into `dist/` on every build (clean repo, no rendering on raw GitHub).

### 7.2 "How do I derive a PCB from this in KiCad/Fritzing?"

- Schemdraw and CircuiTikZ **cannot** — they are documentation artefacts.
- **SKiDL** → `generate_netlist(tool=KICAD9)` → import netlist into KiCad PCB editor.
- **atopile** → directly updates `.kicad_pcb` (compiler sync).
- **tscircuit** → `tsci build --kicad-library`, exports Gerbers directly.
- **Fritzing** has no useful textual *import*; the *export* direction (DSL → `.fzz`) is feasible but deferred per §12.

Any "source of truth" workflow must use atopile / SKiDL / tscircuit; the drawing-only tools are downstream documentation only. Note that all three are forward-only in practice — see §9 (and risk K7) on the back-annotation gap from PCB edits into the textual source.

### 7.3 "How do I migrate existing diagrams?"

KiCad is the lingua franca:

- **KiCad → SKiDL**: `netlist_to_skidl` CLI converts a KiCad netlist into Python.
- **KiCad → tscircuit**: footprint and PCB importers (`kicad-mod-converter`).
- **Eagle / Altium → KiCad → DSL**: KiCad has importers; chain through it.
- For atopile the import path is partial: footprints and PCB layout round-trip via KiCad, but the schematic-side import does not — treat it as forward-only for greenfield modules (see §5 footnote 4).

### 7.4 "Is this language a good DSL?"

Superseded by §4a. Short version: Schemdraw and SKiDL use Python as the host language rather than a dedicated grammar (see §4a); CircuiTikZ is coordinate-driven LaTeX; atopile and WireViz are real DSLs; tscircuit is a real DSL with a heavy toolchain. SKiDL remains a viable engineering substrate per §14, but is weaker as a docs-as-code citizen.

## 8. Recommended architectural directions

> **Note on ordering.** The three options below are presented neutrally for context (lowest → highest effort). The actual recommended ordering — atopile leading, SKiDL as engineering-side fallback, tscircuit as web-stack fallback — lives in §14 and supersedes the apparent symmetry of this menu.

Three coherent stacks, depending on ambition. Each is annotated with how it serves the two personas.

**Option A. Documentation only (lowest effort)**

- Schemdraw or CircuiTikZ, wired into the docs build via §6 **pattern 1** (build-time SVG generation invoked from a generic MkDocs/Sphinx file-watcher or `make` target — *not* §6 pattern 2, which is aspirational and would require writing a fenced-block plugin yourself).
- Engineering source remains in KiCad (binary).
- **Engineer**: still works in KiCad; writes Schemdraw snippets by hand for docs (duplication, drift risk).
- **Builder**: gets pretty inline diagrams but no guaranteed link to the actual board.
- Reasonable as an interim while migration is being decided.

**Option B. Single source of truth, batch-rendered (mid effort)**

- **SKiDL** as engineering source → generates KiCad netlist (engineering) **and** SVG + BOM (docs) from the same `.py`. Caveat: SKiDL's SVG renderer still requires KiCad v5-format symbol libraries installed alongside the KiCad 9 libs used for netlisting (see §4.2 / §11) — budget for both library sets.
- MkDocs/Sphinx plugin renders SVG inline; auto-generated BOM table sits next to it.
- Migration: `netlist_to_skidl` from existing KiCad work.
- **Engineer**: writes Python; full toolchain (ERC, hierarchy, version control). Caveat: the source is a Python program, not a spec (see §4a).
- **Builder**: reads generated SVG + BOM in Markdown; no toolchain required.
- Best fit for Python-centric teams.

**Option C. Recommended: modern, browser-native (highest payoff, newest tech)**

- **atopile** for new designs (declarative DSL with units/assertions; KiCad PCB sync). Strongest docs-as-code citizen for full schematics — this is the §14 leading recommendation.
- **tscircuit** is the §14 web-stack fallback: pick it only if the docs site is a web app (not Markdown-only) and you want true Mermaid-like inline web rendering plus Gerber export.
- **Engineer**: rich DX (LSP, package registry, KiCad PCB updated automatically).
- **Builder**: inline interactive schematic on the docs site; auto-BOM; downloadable Gerbers.
- Best fit for a greenfield workflow.

**Orthogonal: wiring harnesses for the Builder**

- Use **WireViz** for the physical wiring view (cables, connectors, pinouts, color codes) regardless of which option above is chosen for the schematic. WireViz is the strongest docs-as-code citizen of the field (§4a) and exactly the right scope for the Builder.
- The Builder's pages should contain: WireViz diagram → pinout table → BOM → free-text assembly notes (textual, hand-written — no DSL-generated assembly view; see §9 out-of-scope).

## 9. Gaps / unsolved problems

- **No common interchange format** across these tools; KiCad netlist is the partial common denominator.
- **Auto-layout for schematics** is still weaker than a human draftsperson — diffs may include layout churn.
- **Symbol standards** (IEC vs ANSI) vary across tools.
- **GitHub native rendering** of any circuit DSL does not exist (Mermaid is the only diagram DSL GitHub renders inline).
- **DSL-aware LSP coverage** is thin everywhere except tscircuit (TS) and atopile; the rest fall back to the generic host-language tooling described in §5 footnote 2.
- **Round-tripping with PCB layout is one-way in practice** — back-annotation of physical changes into the textual source is fragile.
- **No tool today produces both schematic and harness/wiring views** from one source — expect a multi-tool stack.

**Out of scope.** A textual *assembly view* (step-by-step "place this part here, solder these pads") is explicitly out of scope for this evaluation. None of the candidates target it; producing it from the same source would require a separate notation (closer to a work-instruction DSL than a circuit DSL) and is not pursued here.

### 9.1 Risk register

| # | Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|---|
| K1 | Chosen DSL (atopile / tscircuit) ships breaking changes pre-1.0 | High | Medium | Pin exact version (§11); gate upgrades behind a re-render diff review |
| K2 | Generated SVGs go stale relative to source | Medium | Medium | Pre-commit regen hook + CI `git diff --exit-code` guard (§7.1) |
| K3 | KiCad → DSL migration loses fidelity (especially tscircuit schematic symbols, atopile schematic-side import — see §5 footnote 4) | Medium | Medium | Treat migration as one-shot; keep KiCad source until DSL spike confirms parity |
| K4 | Layout churn in schematic diffs obscures real changes | Medium | Low | Prefer DSLs with deterministic layout (tscircuit autorouter, WireViz/Graphviz); review SVGs visually in PRs |
| K5 | Build-time renderer (Graphviz, KiCad CLI, Node) unavailable in CI | Low | High | Containerise the docs build; pin tool versions in CI image |
| K6 | GPL-licensed renderer (WireViz, KiCad) creates redistribution concerns | Low | Medium | Renderers run at build time only; output SVG/PDF is not a derived work for licence purposes — confirm with legal if redistributing the renderer itself |
| K7 | Back-annotation from PCB to source is fragile (see §7.2) | High | Medium | Treat DSL → PCB as forward-only; capture PCB-side fixes by editing the DSL, not by editing the `.kicad_pcb` directly |

## 10. Suggested next steps

1. Pick docs platform (MkDocs / Sphinx / Docusaurus) — drives the file-watcher / `make` target integration (no fenced-block plugin exists per §6 pattern 2).
2. **Spike**: build the same small example (e.g. an LM7805 regulator + LED indicator + 3-pin connector to a sensor) in the two candidates **atopile** (the leading docs-as-code candidate per §4a.4) and **SKiDL** (the fallback if the atopile spike exposes blocking engineering-side issues). tscircuit is intentionally excluded from this spike and parked as a separate fallback per §14. Wire each candidate into the docs platform, evaluate diff quality, regen UX, and PCB export.
3. **Spike**: add a **WireViz** page rendering the harness for the same example, oriented at the Builder.
4. **Prototype CI guard** for "stale rendering" detection.
5. **Migration trial**: take one existing KiCad project and attempt to import it into the leading candidate **atopile**; rate the result against §5 footnote 4's "partial / forward-only" claim. If the atopile fallback to SKiDL is activated, rerun the trial through `netlist_to_skidl`. (The KiCad → tscircuit importer is only worth trialling if tscircuit is later activated via the §14 fallback clause.)

## 11. References

Versions and licences captured at time of writing (April 2026); pin these in your build to keep rendering reproducible.

- Schemdraw — <https://schemdraw.readthedocs.io/> — MIT — pin `schemdraw==0.20`
- CircuiTikZ — <https://ctan.org/pkg/circuitikz> — LPPL 1.3c — pin via TeX Live 2025 (`circuitikz` v1.7.x)
- SKiDL — <https://github.com/devbisme/skidl> — MIT — pin `skidl==2.0`. Netlist generation uses KiCad 9 symbol libs (`tool=KICAD9`); the built-in SVG schematic renderer still requires KiCad v5-format symbol libraries installed in parallel — budget for both library sets if you need SVG output (see §4.2 and §8 Option B).
- atopile — <https://atopile.io/> , <https://github.com/atopile/atopile> — Apache-2.0 — pin `atopile~=0.6.0` (equivalently `>=0.6,<0.7`); spec still pre-1.0, pin tightly (see K1 in §9.1).
- tscircuit — <https://tscircuit.com/> , <https://github.com/tscircuit/tscircuit> — MIT — pre-1.0, and the project warns that any two `0.0.x` releases can break, so pin **exactly** (e.g. `"@tscircuit/core": "0.0.123"`, `"tsci": "0.0.45"`) rather than using a tilde/caret range; bump deliberately (see K1 in §9.1).
- WireViz — <https://github.com/wireviz/WireViz> — GPL-3.0 affects redistribution of the renderer binary only; your YAML source and the generated SVG/BOM are not derivative works (matches K6 in §9.1). Pin `wireviz==0.4`.
- KiCad — <https://www.kicad.org/> — GPL-3.0 — target KiCad 9.x file formats (if using SKiDL's SVG renderer, also install the KiCad v5 symbol library set — see SKiDL bullet above).
- Fritzing part file format — <https://github.com/fritzing/fritzing-app/wiki/2.1-Part-file-format> — app GPL-3.0; parts CC-BY-SA 3.0

## 12. Building a Fritzing importer/exporter — should we?

**Gap test**: Does anything else give the Builder a *breadboard view* (photo-realistic "plug jumper into row 12") generated from a textual source? **No.** WireViz gives harnesses (cables/connectors), schematic DSLs give schematic SVG, KiCad gives PCBs. The breadboard view is a unique-to-Fritzing artefact.

**Effort test**:

| Sub-deliverable | Effort | Verdict |
|---|---|---|
| Exporter: DSL → `.fzz` schematic-only | medium (well-documented XML, ZIP, reuse stock SVGs from `fritzing-parts`) | ✅ build candidate |
| Exporter: DSL → `.fzz` with auto-laid-out **breadboard view** | **high** (genuine layout problem; this is most of the value but most of the cost) | ⚠ defer; spike first to estimate honestly |
| Importer: `.fzz` → DSL | low–medium | ❌ skip — KiCad already serves as the migration lingua franca |

**Recommendation**

- **Don't build it now.** The cheap part of the exporter (schematic-only) duplicates what the chosen DSL already renders. The valuable part (breadboard view) is the expensive part.
- **Reconsider only if** the Builder persona repeatedly needs the breadboard view *and* a small spike confirms the breadboard auto-layout can be solved with a grid heuristic + Fritzing's own router (rather than a real layout engine).
- **Format facts to retain** (in case the spike happens later): `.fzz` is a ZIP of `.fz` (XML sketch) plus per-part `.fzp` (XML metadata) + four SVGs (icon/breadboard/schematic/pcb). Connectors reference SVG element `id`s. Stock parts are referenceable by `moduleId` from the `fritzing-parts` GitHub repo. There is no official schema; existing core parts are the de-facto spec. Fritzing 1.0.6 (Oct 2025) shows the project is alive.

## 13. VS Code extensions — should we build one?

**Gap test**: For each capability, what already exists?

| Capability | Existing solution | Gap? |
|---|---|---|
| Syntax / language services for the DSL | `atopile.atopile` (atopile, DSL-aware); native TS for tscircuit (DSL-aware via TS types); generic Pylance/Pyright for SKiDL and Schemdraw (no DSL-aware diagnostics); generic LaTeX tooling for CircuiTikZ; YAML schema validation for WireViz (see §5 footnote 2 and §9) | ⚠ partial gap if SKiDL/Schemdraw/CircuiTikZ/WireViz chosen (host-language LSP only); ❌ no gap for atopile/tscircuit |
| Live preview of the diagram while editing | atopile extension has a layout viewer; tscircuit has the `tsci dev` browser preview; CircuiTikZ has PDF preview via generic LaTeX tooling (e.g. LaTeX Workshop); SKiDL/Schemdraw/WireViz have **none** | ⚠ gap if SKiDL/Schemdraw/WireViz chosen |
| Markdown fenced-block inline rendering (à la Mermaid) | `bierner.markdown-mermaid` for Mermaid; **nothing** for circuit DSLs | ⚠ gap everywhere Markdown is rendered without the build pipeline (VS Code preview, GitHub web UI, etc.); the §14 stack works around it by committing SVG |
| Auto-regenerate SVG on save | Generic file-watcher tasks, pre-commit hook, `mkdocs serve` | ❌ no gap (CLI/pre-commit covers it) |
| ERC errors as squiggles in Problems panel | atopile extension does it; tscircuit surfaces partial ERC via TS diagnostics; SKiDL doesn't (ERC runs at script execution, not in-editor); Schemdraw/CircuiTikZ/WireViz have no ERC concept (n/a) | ⚠ gap if SKiDL chosen; ⚠ partial gap for tscircuit (only what TS diagnostics surface) |
| GitHub-side rendering of circuit DSLs | **None possible** — GitHub only renders Mermaid | ❌ unsolvable; not a VS-Code problem |

**Crucial-gap analysis**

- If the chosen DSL is **atopile** or **tscircuit**: existing tooling already covers preview + diagnostics. **Don't build anything.** Use `bierner.markdown-mermaid` as a reference and rely on the build pipeline (pre-commit / CI guard) for stale-SVG protection.
- If the chosen DSL is **SKiDL** (or Schemdraw/CircuiTikZ/WireViz for documentation): there is a real gap — no live preview, no on-save regen UX. *But* the gap is closable without an extension by combining `mkdocs serve` (or `sphinx-autobuild`) for live preview in the browser + a pre-commit hook for regen + a CI guard for staleness. That's three small config files, not a VS Code extension.

**Recommendation**

- **Don't build a VS Code extension.** In every realistic stack, the gap can be closed with off-the-shelf tools and a pre-commit/CI configuration. The VS Code extension would be ergonomic sugar, not a closing-the-gap deliverable.
- **Use instead**:
  - `bierner.markdown-mermaid` and the official extension for the chosen DSL (e.g. `atopile.atopile`).
  - `mkdocs serve` / `sphinx-autobuild` for live preview in the browser (works for *any* DSL, also for non-VS-Code users).
  - A `pre-commit` hook that regenerates SVGs and `git add`s them.
  - A CI job that regenerates and runs `git diff --exit-code` to fail the build if a diagram is stale.
- **Reconsider only if** after a few months of real use the engineers explicitly complain that "alt-tab to the browser preview" is too slow and a live in-editor preview would meaningfully change their workflow. Then build the **smallest possible extension** (preview pane only, one DSL), modelled on `bierner.markdown-mermaid`'s structure.

## 14. Net effect on the recommended stack

The "built-by-us" column collapses to **nothing** for now:

- Engineer source: **atopile** as the leading candidate (per §4a.4), with **SKiDL** as the fallback if the §10 spike exposes blocking engineering-side issues. tscircuit remains a separate fallback if the docs site later moves to a web-app stack that can host its browser-side renderer.
- Harness view for Builder: WireViz.
- PCB output: KiCad (tscircuit Gerber only if the fallback path below is later activated).
- Inline rendering in docs: build-time SVG via §6 pattern 1 (generic MkDocs/Sphinx file-watcher or `make` target invoking `ato build` / WireViz CLI — not a fenced-block plugin, which per §6 pattern 2 doesn't exist yet); commit SVG for GitHub-web rendering. This picks the *commit-the-SVG* branch of the choice in §7.1 (the alternative being "generate into `dist/` on every build, never commit"); the deciding factor is that browsing the repo on github.com must show rendered diagrams without a build step.
- Stale-SVG protection: `pre-commit` hook + CI `git diff --exit-code`.
- Editor ergonomics: existing marketplace extensions only.

Everything previously framed as "we could build…" (Fritzing exporter, custom VS Code extension) is parked behind an explicit gate: build only after a real, observed pain point that off-the-shelf tooling can't solve.
