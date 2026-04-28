/**
 * ASP Design Tokens — TypeScript module
 *
 * Same values as tokens.css, exposed as a typed module for
 * TypeScript / React / React Native consumers.
 */

export const palette = {
  bg:            '#0F0F12',
  surface:       '#17171E',
  surfaceRaised: '#1E1E28',

  border:        'rgba(255,255,255,0.10)',
  borderStrong:  'rgba(255,255,255,0.18)',

  text:          '#E8E8F0',
  textMuted:     'rgba(232,232,240,0.55)',
  textDim:       'rgba(232,232,240,0.35)',
} as const;

export const accent = {
  hex:    '#F0B030', // warm amber — primary
  ink:    '#1A1208', // text on accent fills
  bright: '#E8A020',
  deep:   '#FF8A1F',
  pale:   '#FFB020',
} as const;

export const semantic = {
  success: '#34C87A',
  error:   '#E84040',
  warning: '#F0B030',
} as const;

export const led = {
  power:     '#44FF44',
  bluetooth: '#4488FF',
  profile:   '#FFB020',
} as const;

export const radius = {
  sm:     2,
  md:     3,
  card:   4,
  dialog: 6,
  pill:   999,
} as const;

export const fonts = {
  display: '"Metropolis", "Josefin Sans", system-ui, sans-serif',
  body:    '"Metropolis", "Josefin Sans", system-ui, sans-serif',
  mono:    '"JetBrains Mono", ui-monospace, "SF Mono", Menlo, monospace',
} as const;

export const type = {
  display:  { weight: 300, tracking: '0.01em' },
  wordmark: { weight: 400, tracking: '0.02em' },
  monogram: { weight: 700, tracking: '-0.04em' },
  label:    { tracking: '0.12em' },
  labelSm:  { tracking: '0.15em' },
} as const;

export const space = {
  s1:  4,  s2:  8,  s3: 12, s4: 16,
  s5: 20,  s6: 24,  s8: 32, s10: 40,
  s12: 48,
} as const;

export const tokens = {
  palette, accent, semantic, led,
  radius, fonts, type, space,
} as const;

export default tokens;
