#!/usr/bin/env bash
# ui_dump.sh — single-call wrapper for "dump UI tree, optionally find / tap a node by text".
# See SKILL.md for usage. Invoked by the `/ui-dump` skill.

set -euo pipefail

resolve_serial() {
  if [ -n "${ANDROID_SERIAL:-}" ]; then
    printf '%s' "$ANDROID_SERIAL"
    return 0
  fi
  local devices
  devices=$(adb devices | awk 'NR>1 && $2=="device" {print $1}')
  local count
  count=$(printf '%s\n' "$devices" | grep -c .)
  if [ "$count" -eq 1 ]; then
    printf '%s' "$devices"
    return 0
  fi
  if [ "$count" -eq 0 ]; then
    echo "ui-dump: no Android device detected. Plug one in and enable USB debugging, or set ANDROID_SERIAL." >&2
  else
    echo "ui-dump: $count devices detected. Set ANDROID_SERIAL=<serial> to disambiguate. Available:" >&2
    printf '%s\n' "$devices" >&2
  fi
  return 1
}

dump_to_tmp() {
  local ser="$1"
  adb -s "$ser" shell uiautomator dump /sdcard/ui.xml >/dev/null
  adb -s "$ser" pull /sdcard/ui.xml /tmp/ui.xml >/dev/null
}

find_node() {
  local needle="$1"
  python3 - "$needle" <<'PY'
import sys, xml.etree.ElementTree as ET
needle = sys.argv[1]
tree = ET.parse('/tmp/ui.xml').getroot()
for n in tree.iter('node'):
    t, d = n.get('text', ''), n.get('content-desc', '')
    if needle in t or needle in d:
        print(f"bounds={n.get('bounds')} text={t!r} content-desc={d!r}")
        sys.exit(0)
sys.exit(1)
PY
}

tap_node() {
  local ser="$1" needle="$2"
  local cx cy
  read -r cx cy < <(python3 - "$needle" <<'PY'
import sys, re, xml.etree.ElementTree as ET
needle = sys.argv[1]
tree = ET.parse('/tmp/ui.xml').getroot()
for n in tree.iter('node'):
    t, d = n.get('text', ''), n.get('content-desc', '')
    if needle in t or needle in d:
        m = re.match(r'\[(\d+),(\d+)\]\[(\d+),(\d+)\]', n.get('bounds', ''))
        if not m:
            sys.exit(2)
        x1, y1, x2, y2 = map(int, m.groups())
        print((x1+x2)//2, (y1+y2)//2)
        sys.exit(0)
sys.exit(1)
PY
  )
  adb -s "$ser" shell input tap "$cx" "$cy"
  echo "tapped $needle at ($cx,$cy)"
}

main() {
  local ser
  ser=$(resolve_serial)
  dump_to_tmp "$ser"

  case "${1:-dump}" in
    dump)
      cat /tmp/ui.xml
      ;;
    find)
      [ $# -ge 2 ] || { echo "usage: ui_dump.sh find <needle>" >&2; exit 2; }
      if ! find_node "$2"; then
        echo "ui-dump: '$2' not found in current UI tree" >&2
        exit 1
      fi
      ;;
    tap)
      [ $# -ge 2 ] || { echo "usage: ui_dump.sh tap <needle>" >&2; exit 2; }
      if ! tap_node "$ser" "$2"; then
        echo "ui-dump: '$2' not found in current UI tree" >&2
        exit 1
      fi
      ;;
    *)
      echo "ui-dump: unknown mode '$1' (expected: dump | find <needle> | tap <needle>)" >&2
      exit 2
      ;;
  esac
}

main "$@"
