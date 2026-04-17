---
id: TASK-029
title: Create release cleanup script
status: closed
closed: 2026-04-16
opened: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Create `scripts/cleanup-releases.sh` that keeps only the current stable release plus the two
previous versions, deleting older GitHub releases and their tags.

## Acceptance Criteria

- [ ] `scripts/cleanup-releases.sh` exists and is executable
- [ ] Script lists releases, determines the 3 to keep, deletes the rest
- [ ] Uses `gh release delete --yes --cleanup-tag` to remove both release and tag
- [ ] Script is idempotent (safe to run multiple times)
- [ ] Usage documented in script header comment
- [ ] Script referenced from release process docs (TASK-023)

## Notes

Retention policy: keep current + 2 previous versions.
Example script skeleton:

```bash
#!/bin/bash
# Keep only current + 2 previous releases

tags=$(git tag -l 'v*' | sort -V)
keep_tags=$(echo "$tags" | tail -3)

for tag in $tags; do
  if ! echo "$keep_tags" | grep -q "$tag"; then
    echo "Deleting old release: $tag"
    gh release delete "$tag" --yes --cleanup-tag
  fi
done
```

Manual equivalent:

```bash
gh release list
gh release delete v1.0.0 --yes --cleanup-tag
```
