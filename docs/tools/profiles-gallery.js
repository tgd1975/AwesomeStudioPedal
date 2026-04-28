'use strict';

/**
 * Shared community profiles gallery — used by the Config Builder and the Simulator.
 *
 * Usage:
 *   const gallery = new ProfilesGallery({
 *     indexUrl:    'https://tgd1975.github.io/AwesomeStudioPedal/profiles/index.json',
 *     baseUrl:     'https://tgd1975.github.io/AwesomeStudioPedal/profiles/',
 *     onLoad:      (json) => { ... },   // called with the parsed profiles.json object
 *     buttonCount: 4,                   // optional — pre-selects the filter
 *   });
 *   gallery.open();
 */

class ProfilesGallery {
  constructor({ indexUrl, baseUrl, onLoad, buttonCount = null }) {
    this._indexUrl    = indexUrl;
    this._baseUrl     = baseUrl;
    this._onLoad      = onLoad;
    this._buttonCount = buttonCount;
    this._entries     = null;
    this._overlay     = null;
  }

  open() {
    if (this._overlay) return;
    this._overlay = this._buildOverlay();
    document.body.appendChild(this._overlay);
    this._fetchIndex();
  }

  close() {
    if (!this._overlay) return;
    document.body.removeChild(this._overlay);
    this._overlay = null;
  }

  // ── Private ────────────────────────────────────────────────────────────────

  _buildOverlay() {
    const overlay = document.createElement('div');
    overlay.id    = 'pg-overlay';
    overlay.style.cssText = [
      'position:fixed;inset:0;background:rgba(0,0,0,.45);z-index:9999',
      'display:flex;align-items:center;justify-content:center',
    ].join(';');

    overlay.innerHTML = `
      <div id="pg-dialog" style="
        background:#fff;border-radius:10px;box-shadow:0 8px 32px rgba(0,0,0,.25);
        width:min(820px,96vw);max-height:90vh;display:flex;flex-direction:column;
        overflow:hidden;">

        <div style="padding:16px 20px;border-bottom:1px solid #e5e7eb;
                    display:flex;align-items:center;gap:12px;flex-wrap:wrap;">
          <h2 style="font-size:18px;font-weight:600;margin:0;flex:1 1 auto;">
            Community Profiles
          </h2>

          <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
            <label for="pg-filter" style="font-size:13px;color:#555;">Buttons:</label>
            <select id="pg-filter" style="
              font-size:13px;border:1px solid #d1d5db;border-radius:6px;
              padding:4px 8px;background:#fff;">
              <option value="">Any</option>
              <option value="1">1-button</option>
              <option value="2">2-button</option>
              <option value="3">3-button</option>
              <option value="4">4-button</option>
            </select>
          </div>

          <button id="pg-close" title="Close"
            style="border:none;background:none;font-size:22px;cursor:pointer;
                   color:#6b7280;line-height:1;padding:0 4px;">&times;</button>
        </div>

        <div id="pg-body" style="padding:16px 20px;overflow-y:auto;flex:1;">
          <div id="pg-spinner" style="text-align:center;padding:48px;color:#6b7280;">
            Loading&hellip;
          </div>
          <div id="pg-error"  style="display:none;color:#991b1b;padding:16px;
            background:#fee2e2;border-radius:6px;"></div>
          <div id="pg-cards"  style="display:none;
            display:grid;grid-template-columns:repeat(auto-fill,minmax(240px,1fr));gap:12px;"></div>
        </div>
      </div>
    `;

    overlay.querySelector('#pg-close').onclick = () => this.close();
    overlay.addEventListener('click', (e) => {
      if (e.target === overlay) this.close();
    });

    const filterEl = overlay.querySelector('#pg-filter');
    if (this._buttonCount) filterEl.value = String(this._buttonCount);
    filterEl.onchange = () => this._renderCards(overlay);

    return overlay;
  }

  async _fetchIndex() {
    const spinner = this._overlay.querySelector('#pg-spinner');
    const errEl   = this._overlay.querySelector('#pg-error');
    try {
      const res = await fetch(this._indexUrl);
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const data = await res.json();
      this._entries = data.profiles || [];
      spinner.style.display = 'none';
      this._renderCards(this._overlay);
    } catch (err) {
      spinner.style.display = 'none';
      errEl.style.display   = 'block';
      errEl.textContent     = `Could not load community profiles: ${err.message}`;
    }
  }

  _renderCards(overlay) {
    const filterVal = overlay.querySelector('#pg-filter').value;
    const maxN      = filterVal ? parseInt(filterVal, 10) : Infinity;
    const cardsEl   = overlay.querySelector('#pg-cards');
    cardsEl.style.display = '';
    cardsEl.innerHTML = '';

    const visible = (this._entries || []).filter(e => e.minButtons <= maxN);

    if (visible.length === 0) {
      cardsEl.innerHTML =
        '<p style="color:#6b7280;grid-column:1/-1;">No profiles match this filter.</p>';
      return;
    }

    for (const entry of visible) {
      cardsEl.appendChild(this._buildCard(entry));
    }
  }

  _buildCard(entry) {
    const card = document.createElement('div');
    card.style.cssText = [
      'background:#fff;border:1px solid #e5e7eb;border-radius:8px;padding:14px',
      'display:flex;flex-direction:column;gap:8px;cursor:pointer',
      'transition:box-shadow .15s,border-color .15s',
    ].join(';');
    card.addEventListener('mouseenter', () => {
      card.style.boxShadow  = '0 4px 12px rgba(0,0,0,.12)';
      card.style.borderColor = '#2563eb';
    });
    card.addEventListener('mouseleave', () => {
      card.style.boxShadow  = '';
      card.style.borderColor = '#e5e7eb';
    });

    const fwWarning = entry.minFirmware
      ? `<span style="font-size:11px;background:#fef9c3;color:#78350f;
           border:1px solid #fde68a;border-radius:4px;padding:1px 5px;">
           Requires firmware &ge;&nbsp;${entry.minFirmware}</span>`
      : '';

    const tagsHtml = (entry.tags || [])
      .map(t => `<span style="font-size:11px;background:#f3f4f6;color:#374151;
           border-radius:4px;padding:1px 6px;">${t}</span>`)
      .join(' ');

    card.innerHTML = `
      <div style="display:flex;align-items:flex-start;gap:8px;">
        <span style="font-size:11px;background:#dbeafe;color:#1d4ed8;
          border-radius:4px;padding:2px 7px;white-space:nowrap;flex-shrink:0;">
          ${entry.minButtons}-btn
        </span>
        <strong style="font-size:14px;line-height:1.3;">${_esc(entry.name)}</strong>
      </div>
      ${fwWarning}
      <p style="font-size:12px;color:#555;line-height:1.5;
                display:-webkit-box;-webkit-line-clamp:3;-webkit-box-orient:vertical;
                overflow:hidden;">
        ${_esc(entry.description)}
      </p>
      <div style="display:flex;flex-wrap:wrap;gap:4px;">${tagsHtml}</div>
      <div style="font-size:12px;color:#6b7280;margin-top:auto;">
        ${entry.profileCount} profile${entry.profileCount !== 1 ? 's' : ''}
        &bull; by ${_esc(entry.author)}
      </div>
    `;

    card.onclick = () => this._loadEntry(entry);
    return card;
  }

  async _loadEntry(entry) {
    const url = this._baseUrl + entry.file;
    try {
      const res = await fetch(url);
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const json = await res.json();
      this.close();
      this._onLoad(json);
    } catch (err) {
      const errEl = this._overlay && this._overlay.querySelector('#pg-error');
      if (errEl) {
        errEl.style.display = 'block';
        errEl.textContent   = `Could not load profile: ${err.message}`;
      }
    }
  }
}

function _esc(str) {
  return String(str || '')
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}
