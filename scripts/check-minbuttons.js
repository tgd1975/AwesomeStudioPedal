#!/usr/bin/env node
'use strict';

const fs   = require('fs');
const path = require('path');

const ROOT         = path.resolve(__dirname, '..');
const PROFILES_DIR = path.join(ROOT, 'profiles');
const SUBFOLDER_RE = /^(\d+)-button$/;

const BUTTON_ORDER = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');

function walkDir(dir, results = []) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const full = path.join(dir, entry.name);
    if (entry.isDirectory()) {
      walkDir(full, results);
    } else if (entry.isFile() && entry.name.endsWith('.json') && entry.name !== 'index.json') {
      results.push(full);
    }
  }
  return results;
}

const files  = walkDir(PROFILES_DIR).sort();
let   errors = 0;

for (const filePath of files) {
  const rel       = path.relative(PROFILES_DIR, filePath);
  const parts     = rel.split(path.sep);
  const subfolder = parts[0];
  const match     = SUBFOLDER_RE.exec(subfolder);

  if (!match) continue;

  const maxButtons = parseInt(match[1], 10);
  const allowed    = new Set(BUTTON_ORDER.slice(0, maxButtons));

  let data;
  try {
    data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
  } catch (err) {
    console.error(`PARSE ERROR  ${rel}: ${err.message}`);
    errors++;
    continue;
  }

  const profiles = data.profiles || [];
  let fileErrors = 0;

  for (const profile of profiles) {
    const buttons = profile.buttons || {};
    for (const key of Object.keys(buttons)) {
      if (!allowed.has(key.toUpperCase())) {
        console.error(
          `VIOLATION    ${rel}: profile "${profile.name}" uses button "${key}" ` +
          `but subfolder allows only buttons A–${BUTTON_ORDER[maxButtons - 1]}`
        );
        fileErrors++;
        errors++;
      }
    }
  }

  if (fileErrors === 0) {
    console.log(`OK           ${rel}`);
  }
}

if (errors > 0) {
  console.error(`\n${errors} button-count violation(s) found.`);
  process.exit(1);
} else {
  console.log(`\nAll ${files.length} profile file(s) pass the minButtons check.`);
}
