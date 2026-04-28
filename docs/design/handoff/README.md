# ASP — Developer Handoff

Design tokens, logo assets, Flutter theme, and component specs for
**AwesomeStudioPedal** — WEIRD / Metropolitan direction (locked).

This folder is the machine-readable half of the design system. For
the human-readable review, see `../ASP-design-review.html` (interactive)
or `../ASP-design-review.pdf` (print).

## Contents

| Path                          | What it is                                      |
|-------------------------------|-------------------------------------------------|
| `tokens.css`                  | CSS custom properties — one source of truth     |
| `tokens.ts`                   | Same tokens as a typed TypeScript module        |
| `logo/asp-mark.svg`           | Top-Down Build V2 — primary mark, 64×64         |
| `logo/asp-wordmark.svg`       | "AwesomeStudioPedal" CamelCase wordmark         |
| `logo/asp-monogram.svg`       | Tight "ASP" — compact / app-icon use            |
| `logo/asp-lockup.svg`         | Mark + wordmark horizontal lockup               |
| `flutter/asp_theme.dart`      | Drop-in ThemeData for the mobile app            |
| `components/COMPONENTS.md`    | Dimensions + behavior for core components       |
| `screens/`                    | PNG references of every screen / tool           |

## Getting started

### Web

```html
<link rel="stylesheet" href="tokens.css" />
```

```css
.asp-button {
  background: var(--asp-accent);
  color: var(--asp-accent-ink);
  font-family: var(--asp-font-mono);
  letter-spacing: var(--asp-label-tracking);
  text-transform: uppercase;
  border-radius: var(--asp-radius-sm);
}
```

### TypeScript / React

```ts
import { palette, accent, fonts } from './tokens';

const Button = styled.button`
  background: ${accent.hex};
  color: ${accent.ink};
  font-family: ${fonts.mono};
`;
```

### Flutter

Copy `flutter/asp_theme.dart` into your project and apply:

```dart
import 'asp_theme.dart';

MaterialApp(
  theme: AspTheme.dark,
  // ...
);
```

You'll also need to add Metropolis and JetBrains Mono fonts to
`pubspec.yaml` — see the comments at the top of `asp_theme.dart`.

## Non-negotiables

1. **Dark mode only.** No light theme.
2. **Warm amber (`#F0B030`) is the only saturated colour.** Use it for
   interactive hotspots, the pedal cap, keys in JSON, and active states —
   nothing else.
3. **No gradients. No emoji.** Hairline rules and mono labels do the
   heavy lifting.
4. **Monospace for labels and codes.** JetBrains Mono, small caps,
   wide tracking. Body text is Metropolis.
5. **CamelCase wordmark.** Always `AwesomeStudioPedal`, never
   `AWESOMESTUDIOPEDAL` or `Awesome Studio Pedal`.
6. **Top-Down Build mark.** The logo is a literal top-down portrait of
   the pedal. Do not redraw, reinterpret, or simplify it.

## Questions

Open an issue or comment on the design review HTML. The canonical
source for this system is the design project; this folder is a
snapshot — if you need a change, the design gets updated first and
this folder is regenerated.
