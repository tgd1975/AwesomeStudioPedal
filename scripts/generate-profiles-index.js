#!/usr/bin/env node
'use strict';

/**
 * Walks profiles/**\/*.json (excluding index.json), reads each file's _meta
 * block and profiles array, and writes profiles/index.json.
 *
 * The `generated` field uses the SOURCE_DATE_EPOCH env var when set (for
 * reproducible CI builds). In CI the staleness check diffs the whole file
 * except the generated field — see .github/workflows/profiles.yml.
 */

const fs   = require('fs');
const path = require('path');

const ROOT          = path.resolve(__dirname, '..');
const PROFILES_DIR  = path.join(ROOT, 'profiles');
const INDEX_PATH    = path.join(PROFILES_DIR, 'index.json');
const SUBFOLDER_RE  = /^(\d+)-button$/;

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

const files = walkDir(PROFILES_DIR).sort();
const entries = [];

for (const filePath of files) {
  const rel       = path.relative(PROFILES_DIR, filePath);
  const parts     = rel.split(path.sep);
  const subfolder = parts[0];
  const match     = SUBFOLDER_RE.exec(subfolder);

  if (!match) {
    console.error(`Skipping ${rel}: not inside an N-button subfolder`);
    continue;
  }

  const minButtons = parseInt(match[1], 10);
  let data;
  try {
    data = JSON.parse(fs.readFileSync(filePath, 'utf8'));
  } catch (err) {
    console.error(`Failed to parse ${rel}: ${err.message}`);
    process.exit(1);
  }

  const meta = data._meta;
  if (!meta || !meta.id || !meta.author || !meta.tags) {
    console.error(`Missing required _meta fields in ${rel}`);
    process.exit(1);
  }

  const profiles      = data.profiles || [];
  const profileCount  = profiles.length;
  const profileNames  = profiles.map(p => p.name || '');
  const name          = profiles[0]?.name || meta.id;
  const description   = profiles[0]?.description || '';

  const entry = {
    id:           meta.id,
    file:         rel.replace(/\\/g, '/'),
    minButtons,
    name,
    description,
    author:       meta.author,
    tags:         meta.tags,
    tested:       meta.tested || '',
    profileCount,
    profiles:     profileNames,
  };

  if (meta.minFirmware) {
    entry.minFirmware = meta.minFirmware;
  }

  entries.push(entry);
}

const timestamp = process.env.SOURCE_DATE_EPOCH
  ? new Date(parseInt(process.env.SOURCE_DATE_EPOCH, 10) * 1000).toISOString()
  : new Date().toISOString();

const index = {
  version:   1,
  generated: timestamp,
  profiles:  entries,
};

fs.writeFileSync(INDEX_PATH, JSON.stringify(index, null, 2) + '\n', 'utf8');
console.log(`Written ${INDEX_PATH} with ${entries.length} profile set(s).`);
