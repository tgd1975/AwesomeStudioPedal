# ASP — Design

Brand identity, mobile app, and web tool designs for
**AwesomeStudioPedal** — an open-source, BLE-capable multi-action
foot switch for musicians.

## What's in here

| File                        | For whom                          | What it is                                           |
|-----------------------------|-----------------------------------|------------------------------------------------------|
| `ASP-design-review.html`    | Everyone                          | Interactive canvas — open in a browser, poke around  |
| `ASP-design-review.pdf`     | Reviewers, investors, print       | Print-ready export of the full design review         |
| `handoff/`                  | Developers                        | Tokens, logo SVGs, Flutter theme, component specs    |

## Start here

- **Just want to see what it looks like?** Open
  [`ASP-design-review.html`](ASP-design-review.html) in a browser.
  No install, no build.
- **Need a PDF to share?** [`ASP-design-review.pdf`](ASP-design-review.pdf).
- **Implementing it?** Go to [`handoff/`](handoff/) and follow its README.

## System summary

- **Direction:** WEIRD / Metropolitan — a 1920s-moderne workshop
  aesthetic. Feels like a vintage studio nameplate rather than a
  generic consumer app.
- **Logo:** *Top-Down Build* — a literal top-down view of the pedal
  enclosure: square plate, four Phillips-head corner screws, bezel
  ring, amber footswitch cap, lit LED hub.
- **Wordmark:** `AwesomeStudioPedal` in Metropolis, CamelCase. "Pedal"
  is accent-coloured.
- **Type:** Metropolis for display and body, JetBrains Mono for
  labels, codes, and JSON.
- **Palette:** Dark-mode only. Warm amber (`#F0B030`) as the single
  saturated accent; everything else is warm greys.
- **Voice:** Honest, mechanical, a little dry. "Scan for pedal", not
  "Let's find your pedal!".

## Surfaces covered

1. **Brand identity sheet** — logo constructions, wordmark, monogram,
   palette, type, components, full application.
2. **Mobile app (5 screens)** — Home, Profile List, Profile Editor,
   Action Editor, Upload-in-progress. Flutter / Material You, themed.
3. **Web tools (3 pages)** — Simulator (virtual pedal + MIDI / Serial
   / IO consoles), Profile Builder (form + live JSON),
   Configuration Builder (hardware pin mapping + live JSON).

## Updating the design

This folder is a *snapshot*. The canonical design project lives
elsewhere — if something needs to change, update it there and
regenerate this folder. Don't edit `ASP-design-review.html` or the
handoff files in place; they are exports.

## License

Design files are released under the same license as the rest of the
ASP project. Use freely, modify freely, attribute where reasonable.
