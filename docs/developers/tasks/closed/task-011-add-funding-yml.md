---
id: TASK-011
title: Add FUNDING.yml
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add `.github/FUNDING.yml` to enable sponsorship buttons on the GitHub repository page.
The project uses `tgd1975` as the username across all platforms.

## Acceptance Criteria

- [ ] `.github/FUNDING.yml` exists and is valid
- [ ] GitHub Sponsors entry uses `tgd1975`
- [ ] Liberapay and Buy Me a Coffee custom URLs are included

## Notes

Intended file content:

```yaml
github: tgd1975
custom: [
  "https://liberapay.com/tgd1975/donate",
  "https://buymeacoffee.com/tgd1975"
]
```

Platform notes:

- GitHub Sponsors uses the GitHub username automatically
- Liberapay provides European-friendly payment options (SEPA)
- Buy Me a Coffee offers simple one-time donations
- All platforms use consistent `tgd1975` username for branding
