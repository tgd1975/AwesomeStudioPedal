'use strict';

// Inline example data — no network fetch needed, works offline and on GitHub Pages.
const EXAMPLE_DATA = {"_doc":"Key reference: docs/builders/KEY_REFERENCE.md","profiles":[{"name":"01 Score Navigator","description":"Designed for musicians reading digital sheet music on a tablet or notebook (e.g., forScore, MobileSheets, or Adobe Acrobat). Uses standard navigation keys to turn pages and jump to sections.","buttons":{"A":{"type":"SendCharAction","name":"Prev Page","value":"KEY_PAGE_UP"},"B":{"type":"SendCharAction","name":"Next Page","value":"KEY_PAGE_DOWN"},"C":{"type":"SendCharAction","name":"First Page","value":"KEY_HOME"},"D":{"type":"SendCharAction","name":"Last Page","value":"KEY_END"}}},{"name":"02 Pixel Camera Remote","description":"Hands-free video and photo control for Google Pixel smartphones. Uses the Volume Up shortcut to trigger the shutter. Includes a 3-second delay on Button A to allow the performer to get into position.","buttons":{"A":{"type":"DelayedAction","name":"Delayed Shutter","delayMs":3000,"action":{"type":"SendMediaKeyAction","value":"KEY_VOLUME_UP"}},"B":{"type":"SendMediaKeyAction","name":"Instant Shutter","value":"KEY_VOLUME_UP"},"C":{"type":"SendCharAction","name":"Switch Mode","value":"KEY_TAB"},"D":{"type":"SendCharAction","name":"Gallery Preview","value":"KEY_ENTER"}}},{"name":"03 VLC Mobile Controller","description":"Optimized for the VLC app on smartphones. Ideal for musicians practicing along with video/audio recordings. Allows for quick pausing and speed adjustments to learn difficult passages.","buttons":{"A":{"type":"SendMediaKeyAction","name":"Play/Pause","value":"MEDIA_PLAY_PAUSE"},"B":{"type":"SendMediaKeyAction","name":"Stop","value":"MEDIA_STOP"},"C":{"type":"SendCharAction","name":"Slow Down","value":"["},"D":{"type":"SendCharAction","name":"Speed Up","value":"]"}}},{"name":"04 OBS Stream Deck","description":"A foot-operated switcher for OBS Studio on a PC/Mac. Uses high-range function keys (F13-F16) to switch scenes or mute audio without interfering with standard keyboard typing.","buttons":{"A":{"type":"SendCharAction","name":"Intro Scene","value":"KEY_F13"},"B":{"type":"SendCharAction","name":"Main Scene","value":"KEY_F14"},"C":{"type":"SendCharAction","name":"Mute Mic","value":"KEY_F15"},"D":{"type":"SendCharAction","name":"Start Stream","value":"KEY_F16"}}},{"name":"05 DAW Looper (Ableton)","description":"General purpose recording and looping profile for DAWs like Ableton Live. Focuses on the 'Solo Performer' workflow where undoing a bad take quickly is essential.","buttons":{"A":{"type":"SendCharAction","name":"Record","value":"F9"},"B":{"type":"SendCharAction","name":"Play/Stop","value":" "},"C":{"type":"SendStringAction","name":"Undo Take","value":"ctrl+z"},"D":{"type":"SendCharAction","name":"Metronome","value":"c"}}},{"name":"06 Social & Comms","description":"A productivity profile for interacting with Discord, Slack, or Zoom while playing an instrument. Includes a one-touch status update for rehearsals.","buttons":{"A":{"type":"SendStringAction","name":"Quick Msg","value":"Starting now, see you in the chat!"},"B":{"type":"SendStringAction","name":"Mute App","value":"ctrl+shift+m"},"C":{"type":"SendStringAction","name":"Deafen","value":"ctrl+shift+d"},"D":{"type":"SendCharAction","name":"Screenshot","value":"KEY_PRINTSCREEN"}}},{"name":"07 System Debug","description":"Technical health check profile for the AwesomeStudioPedal. Sends status messages to the Serial Monitor and provides basic OS navigation (Up/Down) for emergency menu control.","buttons":{"A":{"type":"SerialOutputAction","name":"Ping Pedal","value":"AwesomeStudioPedal: Profile 7 Active - All systems nominal."},"B":{"type":"SendStringAction","name":"Version Info","value":"v1.2-Extended-Func"},"C":{"type":"SendCharAction","name":"Nav Up","value":"KEY_UP_ARROW"},"D":{"type":"SendCharAction","name":"Nav Down","value":"KEY_DOWN_ARROW"}}}]};

const PIN_TYPES = [
  'PinHighAction', 'PinLowAction', 'PinToggleAction',
  'PinHighWhilePressedAction', 'PinLowWhilePressedAction',
];

const WHILE_PRESSED_TYPES = ['PinHighWhilePressedAction', 'PinLowWhilePressedAction'];

const SLOT_NAMES = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');

// Colors matching the physical prototype button caps (A=orange, B=red, C=hot pink, D=purple).
// Slots beyond D fall back to the default CSS button border color.
const SLOT_COLORS = ['#f97316', '#dc2626', '#ec4899', '#7c3aed'];

// ── Hardware layout state ────────────────────────────────────────────────────
let numButtons = 4;
let numSelectLeds = 3;

// ── Simulator state ──────────────────────────────────────────────────────────
let config = null;
let currentProfile = 0;
let heldButtons = new Set();
let pendingDelays = {};
let bleConnected = false;

// ── IO pin state (for TASK-093) ──────────────────────────────────────────────
const pinState = {};  // pin number → 'HIGH' | 'LOW'

// ── BLE connection state ─────────────────────────────────────────────────────
function setBleConnected(connected) {
  bleConnected = connected;
  document.getElementById('ble-led').classList.toggle('lit', connected);
  const toggleBtn = document.getElementById('ble-toggle');
  if (toggleBtn) toggleBtn.textContent = connected ? 'Disconnect BLE' : 'Connect BLE';
}

// ── Output helpers ──────────────────────────────────────────────────────────
function appendOutput(logEl, message) {
  const now = new Date();
  const ts = [now.getHours(), now.getMinutes(), now.getSeconds()]
    .map(n => String(n).padStart(2, '0'))
    .join(':');
  const line = document.createElement('div');
  line.className = 'log-line';
  line.textContent = `[${ts}] ${message}`;
  logEl.appendChild(line);
  logEl.scrollTop = logEl.scrollHeight;
}

function bleLog(msg) {
  appendOutput(document.getElementById('ble-output'), msg);
}

function serialLog(msg) {
  appendOutput(document.getElementById('serial-output'), msg);
}

// ── IO state log (TASK-093) ──────────────────────────────────────────────────
function ioLog(action) {
  const logEl = document.getElementById('io-output');
  if (!logEl) return;

  let logMsg = '';

  if (action.type === 'PinHighAction') {
    pinState[action.pin] = 'HIGH';
    logMsg = `[Button] Pin ${action.pin} \u2192 HIGH`;
  } else if (action.type === 'PinLowAction') {
    pinState[action.pin] = 'LOW';
    logMsg = `[Button] Pin ${action.pin} \u2192 LOW`;
  } else if (action.type === 'PinToggleAction') {
    const current = pinState[action.pin] || 'LOW';
    const next = current === 'HIGH' ? 'LOW' : 'HIGH';
    pinState[action.pin] = next;
    logMsg = `[Button] Pin ${action.pin} \u2192 ${next} (toggle from ${current})`;
  } else if (action.type === 'PinHighWhilePressedAction') {
    pinState[action.pin] = 'HIGH';
    logMsg = `[Press]  Pin ${action.pin} \u2192 HIGH`;
  } else if (action.type === 'PinLowWhilePressedAction') {
    pinState[action.pin] = 'LOW';
    logMsg = `[Press]  Pin ${action.pin} \u2192 LOW`;
  } else if (action.type === 'PinRelease') {
    // Release: invert the held state
    const released = action.source === 'PinHighWhilePressedAction' ? 'LOW' : 'HIGH';
    pinState[action.pin] = released;
    logMsg = `[Rel]    Pin ${action.pin} \u2192 ${released}`;
  } else {
    return;
  }

  appendOutput(logEl, logMsg);
  updatePinSummary();
}

function updatePinSummary() {
  const summaryEl = document.getElementById('io-summary');
  if (!summaryEl) return;
  const keys = Object.keys(pinState).sort((a, b) => Number(a) - Number(b));
  if (keys.length === 0) {
    summaryEl.textContent = 'No pins triggered yet';
  } else {
    summaryEl.textContent = keys.map(p => `Pin ${p}: ${pinState[p]}`).join('  |  ');
  }
}

// ── Action formatting ────────────────────────────────────────────────────────
function formatActionOutput(action) {
  if (!action || !action.type) return '(no action)';
  switch (action.type) {
    case 'SendCharAction':
    case 'SendKeyAction':
      return `KEY: ${action.value}`;
    case 'SendStringAction':
      return `TYPE: "${action.value}"`;
    case 'SendMediaKeyAction':
      return `MEDIA: ${action.value}`;
    case 'SerialOutputAction':
      return `SERIAL: ${action.value}`;
    case 'DelayedAction':
      return `DELAY ${action.delayMs}ms \u2192 ${formatActionOutput(action.action)}`;
    case 'PinHighAction':
      return `PIN ${action.pin}: HIGH`;
    case 'PinLowAction':
      return `PIN ${action.pin}: LOW`;
    case 'PinToggleAction':
      return `PIN ${action.pin}: TOGGLE`;
    case 'PinHighWhilePressedAction':
      return `PIN ${action.pin}: HIGH (held)`;
    case 'PinLowWhilePressedAction':
      return `PIN ${action.pin}: LOW (held)`;
    default:
      return `${action.type}`;
  }
}

function dispatchAction(action) {
  if (!action || !action.type) return;
  if (action.type === 'SerialOutputAction') {
    serialLog(action.value);
  } else if (PIN_TYPES.includes(action.type)) {
    ioLog(action);
  } else {
    if (!bleConnected) {
      bleLog(`\u26a0 BLE disconnected \u2014 keystroke not sent: ${formatActionOutput(action)}`);
    } else {
      bleLog(formatActionOutput(action));
    }
  }
}

// ── Hardware layout rendering ────────────────────────────────────────────────
function activeSlots() {
  return SLOT_NAMES.slice(0, numButtons);
}

function renderButtonGrid() {
  const grid = document.getElementById('button-grid');
  if (!grid) return;

  // Detach old event listeners by replacing each button
  grid.innerHTML = '';

  for (const slot of activeSlots()) {
    const btn = document.createElement('button');
    btn.className = 'pedal-btn';
    btn.id = `btn-${slot}`;
    btn.dataset.btn = slot;
    btn.innerHTML = `<span class="btn-slot">${slot}</span><span class="btn-name"></span>`;

    const slotColor = SLOT_COLORS[SLOT_NAMES.indexOf(slot)];
    if (slotColor) btn.style.setProperty('--slot-color', slotColor);

    btn.addEventListener('mousedown', () => simulateButtonPress(slot));
    btn.addEventListener('mouseup', () => simulateButtonRelease(slot));
    btn.addEventListener('mouseleave', () => simulateButtonRelease(slot));
    btn.addEventListener('touchstart', (e) => { e.preventDefault(); simulateButtonPress(slot); });
    btn.addEventListener('touchend', (e) => { e.preventDefault(); simulateButtonRelease(slot); });

    grid.appendChild(btn);
  }

  // Refresh labels for current profile
  if (config) {
    const profile = config.profiles[currentProfile];
    for (const slot of activeSlots()) refreshButtonLabel(slot, profile);
  }
}

function renderProfileLeds() {
  const strip = document.getElementById('profile-leds');
  if (!strip) return;
  strip.innerHTML = '';
  for (let i = 0; i < numSelectLeds; i++) {
    const led = document.createElement('div');
    led.className = 'led';
    led.id = `profile-led-${i}`;
    led.title = `Profile LED ${i} (bit ${i})`;
    strip.appendChild(led);
  }
}

function setHardwareLayout(newNumButtons, newNumSelectLeds) {
  numButtons = Math.max(1, Math.min(26, newNumButtons));
  numSelectLeds = Math.max(1, Math.min(6, newNumSelectLeds));

  // Sync toolbar spinners
  const btnInput = document.getElementById('hw-num-buttons');
  const ledInput = document.getElementById('hw-num-select-leds');
  if (btnInput) btnInput.value = numButtons;
  if (ledInput) ledInput.value = numSelectLeds;

  renderButtonGrid();
  renderProfileLeds();

  if (config) {
    updateProfileLeds(currentProfile);
    const profile = config.profiles[currentProfile];
    for (const slot of activeSlots()) refreshButtonLabel(slot, profile);
  }
}

// ── Profile LED encoding ────────────────────────────────────────────────────
function updateProfileLeds(index) {
  const bits = index + 1;
  for (let i = 0; i < numSelectLeds; i++) {
    const el = document.getElementById(`profile-led-${i}`);
    if (el) el.classList.toggle('lit', (bits & (1 << i)) !== 0);
  }
}

// ── Button simulation ────────────────────────────────────────────────────────
function simulateButtonPress(id) {
  if (!config) return;
  const profile = config.profiles[currentProfile];
  if (!profile) return;
  const action = profile.buttons && profile.buttons[id];
  if (!action || !action.type) return;

  const btnEl = document.getElementById(`btn-${id}`);

  if (action.type === 'DelayedAction') {
    bleLog(formatActionOutput(action));
    if (btnEl) btnEl.classList.add('delay-active');

    let remaining = action.delayMs;
    const startTime = Date.now();

    function updateCountdown() {
      remaining = action.delayMs - (Date.now() - startTime);
      if (btnEl) {
        const secs = Math.max(0, Math.ceil(remaining / 1000));
        const nameEl = btnEl.querySelector('.btn-name');
        if (nameEl) nameEl.textContent = `${secs}s\u2026`;
      }
    }

    const tickInterval = setInterval(updateCountdown, 200);
    const timer = setTimeout(() => {
      clearInterval(tickInterval);
      if (btnEl) {
        btnEl.classList.remove('delay-active');
        refreshButtonLabel(id, profile);
      }
      delete pendingDelays[id];
      dispatchAction(action.action);
    }, action.delayMs);

    pendingDelays[id] = { timer, tickInterval, profile: currentProfile };

  } else if (WHILE_PRESSED_TYPES.includes(action.type)) {
    heldButtons.add(id);
    if (btnEl) btnEl.classList.add('held');
    ioLog(action);
  } else {
    dispatchAction(action);
  }
}

function simulateButtonRelease(id) {
  if (!heldButtons.has(id)) return;
  heldButtons.delete(id);
  const btnEl = document.getElementById(`btn-${id}`);
  if (btnEl) btnEl.classList.remove('held');

  if (!config) return;
  const profile = config.profiles[currentProfile];
  if (!profile) return;
  const action = profile.buttons && profile.buttons[id];
  if (!action) return;

  if (WHILE_PRESSED_TYPES.includes(action.type)) {
    ioLog({ type: 'PinRelease', pin: action.pin, source: action.type });
  }
}

function cancelPendingDelays() {
  for (const id of Object.keys(pendingDelays)) {
    const { timer, tickInterval } = pendingDelays[id];
    clearTimeout(timer);
    clearInterval(tickInterval);
    const btnEl = document.getElementById(`btn-${id}`);
    if (btnEl) {
      btnEl.classList.remove('delay-active');
      if (config) {
        const profile = config.profiles[currentProfile];
        refreshButtonLabel(id, profile);
      }
    }
    bleLog('DELAY CANCELLED');
  }
  pendingDelays = {};
}

// ── Profile switching ────────────────────────────────────────────────────────
function selectProfile(index) {
  if (!config) return;
  const maxProfiles = (1 << numSelectLeds) - 1;
  const availableProfiles = Math.min(config.profiles.length, maxProfiles);
  if (availableProfiles < 1) return;

  currentProfile = ((index % availableProfiles) + availableProfiles) % availableProfiles;

  cancelPendingDelays();
  updateProfileLeds(currentProfile);

  const profile = config.profiles[currentProfile];
  document.getElementById('profile-name').textContent =
    profile ? (profile.name || `Profile ${currentProfile + 1}`) : '\u2014';

  for (const slot of activeSlots()) {
    refreshButtonLabel(slot, profile);
  }
}

function refreshButtonLabel(slot, profile) {
  const btnEl = document.getElementById(`btn-${slot}`);
  if (!btnEl) return;
  const action = profile && profile.buttons && profile.buttons[slot];
  const nameEl = btnEl.querySelector('.btn-name');
  if (nameEl) nameEl.textContent = (action && action.name) ? action.name : '';
}

// ── Infer hardware layout from profiles.json ─────────────────────────────────
function inferLayoutFromProfiles(profiles) {
  let maxSlotIndex = 0;
  for (const p of profiles) {
    if (!p.buttons) continue;
    for (const slot of Object.keys(p.buttons)) {
      const idx = SLOT_NAMES.indexOf(slot.toUpperCase());
      if (idx > maxSlotIndex) maxSlotIndex = idx;
    }
  }
  const inferredButtons = maxSlotIndex + 1;
  // Select LEDs: minimum bits to represent all profiles (1-indexed binary)
  const inferredLeds = Math.max(1, Math.ceil(Math.log2(profiles.length + 1)));
  return { numButtons: Math.max(1, inferredButtons), numSelectLeds: inferredLeds };
}

// ── Config loading ───────────────────────────────────────────────────────────
function loadConfig(jsonData) {
  if (!jsonData || !Array.isArray(jsonData.profiles) || jsonData.profiles.length === 0) {
    bleLog('ERROR: Invalid config \u2014 missing or empty "profiles" array');
    return;
  }

  config = jsonData;
  currentProfile = 0;
  heldButtons.clear();
  cancelPendingDelays();

  // Infer layout from profiles
  const layout = inferLayoutFromProfiles(config.profiles);
  setHardwareLayout(layout.numButtons, layout.numSelectLeds);

  setBleConnected(true);
  selectProfile(0);
}

function loadHardwareConfig(jsonData) {
  if (typeof jsonData.numButtons === 'number') {
    setHardwareLayout(
      jsonData.numButtons,
      typeof jsonData.numSelectLeds === 'number' ? jsonData.numSelectLeds : numSelectLeds
    );
  }
}

// ── Export ───────────────────────────────────────────────────────────────────
function exportConfig() {
  if (!config) {
    bleLog('ERROR: No config loaded to export');
    return;
  }
  const blob = new Blob([JSON.stringify(config, null, 2)], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'profiles.json';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
}

// ── Wire up DOM events ───────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  // Initial render
  renderButtonGrid();
  renderProfileLeds();

  // Hardware layout spinners
  document.getElementById('hw-num-buttons').addEventListener('change', (e) => {
    const v = parseInt(e.target.value) || numButtons;
    setHardwareLayout(v, numSelectLeds);
  });

  document.getElementById('hw-num-select-leds').addEventListener('change', (e) => {
    const v = parseInt(e.target.value) || numSelectLeds;
    setHardwareLayout(numButtons, v);
  });

  // SELECT button
  document.getElementById('select-btn').addEventListener('click', () => {
    if (!config) return;
    selectProfile(currentProfile + 1);
  });

  // Load profiles.json from file
  document.getElementById('config-file').addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (ev) => {
      try {
        const json = JSON.parse(ev.target.result);
        loadConfig(json);
        document.getElementById('config-name').textContent = file.name;
      } catch (err) {
        bleLog(`ERROR: Parse error \u2014 ${err.message}`);
      }
    };
    reader.readAsText(file);
    e.target.value = '';
  });

  // Load config.json from file
  document.getElementById('hw-config-file').addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = (ev) => {
      try {
        const json = JSON.parse(ev.target.result);
        loadHardwareConfig(json);
      } catch (err) {
        bleLog(`ERROR: config.json parse error \u2014 ${err.message}`);
      }
    };
    reader.readAsText(file);
    e.target.value = '';
  });

  // BLE connect/disconnect toggle
  document.getElementById('ble-toggle').addEventListener('click', () => {
    setBleConnected(!bleConnected);
    if (!bleConnected) {
      bleLog('\u26a0 BLE disconnected \u2014 keystrokes not sent');
    }
  });

  // Load example (inline — no network fetch)
  document.getElementById('load-example').addEventListener('click', () => {
    loadConfig(EXAMPLE_DATA);
    document.getElementById('config-name').textContent = 'example (default) \u2014 load your own profiles.json to replace';
  });

  // Clear output
  document.getElementById('clear-btn').addEventListener('click', () => {
    document.getElementById('ble-output').innerHTML = '';
    document.getElementById('serial-output').innerHTML = '';
    const ioEl = document.getElementById('io-output');
    if (ioEl) ioEl.innerHTML = '';
    Object.keys(pinState).forEach(k => delete pinState[k]);
    updatePinSummary();
  });

  // Export
  document.getElementById('export-btn').addEventListener('click', exportConfig);

  // Cancel pending delays on page unload
  window.addEventListener('beforeunload', () => {
    cancelPendingDelays();
  });

  // Auto-load example on page open (TASK-092)
  loadConfig(EXAMPLE_DATA);
  document.getElementById('config-name').textContent = 'example (default) \u2014 load your own profiles.json to replace';
});
