#pragma once

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>ESP32-C3 RC Car</title>
<style>
:root {
  --bg: #0b0f17;
  --card: #131a26;
  --card2: #1a2333;
  --border: #27344a;
  --text: #e6edf7;
  --muted: #8ea0ba;
  --accent: #38bdf8;
  --green: #22c55e;
  --red: #ef4444;
  --amber: #f59e0b;
}
* { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
body {
  margin: 0;
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
  background: var(--bg);
  color: var(--text);
  line-height: 1.4;
}
.wrap {
  max-width: 960px;
  margin: 0 auto;
  padding: 14px;
  display: grid;
  gap: 14px;
}
header {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 14px;
  padding: 12px 16px;
}
.title { font-size: 1.15rem; font-weight: 700; letter-spacing: 0.2px; }
.sub { font-size: 0.8rem; color: var(--muted); }
.badges { display: flex; gap: 8px; flex-wrap: wrap; align-items: center; }
.badge {
  font-size: 0.78rem;
  font-weight: 600;
  padding: 4px 10px;
  border-radius: 999px;
  border: 1px solid var(--border);
  background: var(--card2);
}
.badge.ok { color: var(--green); border-color: rgba(34,197,94,.4); background: rgba(34,197,94,.12); }
.badge.warn { color: var(--amber); border-color: rgba(245,158,11,.4); background: rgba(245,158,11,.12); }
.badge.err { color: var(--red); border-color: rgba(239,68,68,.4); background: rgba(239,68,68,.12); }

.grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 14px;
}
@media (max-width: 760px) {
  .grid { grid-template-columns: 1fr; }
}
.card {
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 14px;
  padding: 16px;
}
.card h2 {
  margin: 0 0 12px 0;
  font-size: 0.98rem;
  font-weight: 700;
  color: var(--accent);
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.telem-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 10px;
}
.stat {
  background: var(--card2);
  border: 1px solid var(--border);
  border-radius: 10px;
  padding: 10px;
  text-align: center;
}
.stat .lbl { font-size: 0.72rem; color: var(--muted); text-transform: uppercase; }
.stat .val { font-size: 1.15rem; font-weight: 700; margin-top: 2px; font-variant-numeric: tabular-nums; }

.steer-bar-wrap {
  margin-top: 10px;
  background: var(--card2);
  border: 1px solid var(--border);
  border-radius: 999px;
  height: 12px;
  position: relative;
  overflow: hidden;
}
.steer-bar-mid {
  position: absolute;
  left: 50%;
  top: 0; bottom: 0;
  width: 2px;
  background: var(--muted);
  opacity: 0.5;
}
.steer-dot {
  position: absolute;
  top: 1px;
  width: 10px;
  height: 10px;
  border-radius: 50%;
  background: var(--accent);
  transform: translateX(-50%);
  transition: left 40ms linear;
}

/* WASD Pad */
.wasd-wrap {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
  user-select: none;
}
.wasd-row {
  display: flex;
  gap: 8px;
}
.key-btn {
  width: 68px;
  height: 62px;
  border-radius: 12px;
  border: 1px solid var(--border);
  background: var(--card2);
  color: var(--text);
  font-size: 1.1rem;
  font-weight: 700;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  touch-action: none;
  transition: transform .05s, background .08s, border-color .08s;
}
.key-btn span { font-size: 0.65rem; color: var(--muted); font-weight: 500; }
.key-btn.active {
  background: rgba(56,189,248,.22);
  border-color: var(--accent);
  color: #fff;
  transform: scale(0.96);
}
.stop-btn {
  width: 100%;
  max-width: 220px;
  height: 44px;
  border-radius: 10px;
  border: 1px solid rgba(239,68,68,.5);
  background: rgba(239,68,68,.16);
  color: #fecaca;
  font-weight: 700;
  cursor: pointer;
  touch-action: none;
}
.stop-btn:active, .stop-btn.active { background: var(--red); color: #fff; }

/* Sliders */
.ctrl-group { margin-bottom: 14px; }
.ctrl-head {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 6px;
  font-size: 0.88rem;
  font-weight: 600;
}
.ctrl-val { font-variant-numeric: tabular-nums; color: var(--accent); }
input[type=range] {
  width: 100%;
  accent-color: var(--accent);
  height: 28px;
  cursor: pointer;
}
.chk-row {
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 0.82rem;
  color: var(--muted);
}

/* Settings */
.form-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 12px;
  padding: 9px 0;
  border-bottom: 1px solid rgba(39,52,74,.6);
}
.form-row:last-child { border-bottom: none; }
.form-lbl { font-size: 0.88rem; font-weight: 600; }
.form-hint { font-size: 0.74rem; color: var(--muted); }

.switch {
  position: relative;
  display: inline-block;
  width: 48px;
  height: 26px;
  flex-shrink: 0;
}
.switch input { opacity: 0; width: 0; height: 0; }
.slider-sw {
  position: absolute;
  cursor: pointer;
  inset: 0;
  background: #2b384e;
  border-radius: 999px;
  transition: .2s;
}
.slider-sw:before {
  content: "";
  position: absolute;
  height: 20px;
  width: 20px;
  left: 3px;
  bottom: 3px;
  background: #fff;
  border-radius: 50%;
  transition: .2s;
}
.switch input:checked + .slider-sw { background: var(--accent); }
.switch input:checked + .slider-sw:before { transform: translateX(22px); }

input[type=text], input[type=password], input[type=number], select {
  background: var(--card2);
  color: var(--text);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 7px 10px;
  font-size: 0.88rem;
  width: 180px;
  max-width: 100%;
}
.btn-row {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 12px;
}
.btn {
  padding: 9px 14px;
  border-radius: 9px;
  border: 1px solid var(--border);
  background: var(--card2);
  color: var(--text);
  font-weight: 600;
  font-size: 0.85rem;
  cursor: pointer;
}
.btn.primary { background: var(--accent); color: #041322; border-color: var(--accent); }
.btn.warn { border-color: rgba(245,158,11,.5); color: #fde68a; }
.btn.danger { border-color: rgba(239,68,68,.5); color: #fecaca; }
.toast {
  font-size: 0.8rem;
  color: var(--green);
  min-height: 1.1em;
  margin-top: 6px;
}
.pin-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 0.82rem;
}
.pin-table td, .pin-table th {
  text-align: left;
  padding: 6px 8px;
  border-bottom: 1px solid rgba(39,52,74,.6);
}
.pin-table th { color: var(--muted); font-weight: 600; }
code {
  background: var(--card2);
  padding: 2px 6px;
  border-radius: 5px;
  font-family: ui-monospace, SFMono-Regular, Menlo, monospace;
  font-size: 0.8rem;
  color: var(--accent);
}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <div>
      <div class="title">ESP32-C3 SuperMini RC Car</div>
      <div class="sub">AP Web Controller &bull; Servo 50&deg;&ndash;130&deg; &bull; 2-Pin PWM/PWM H-Bridge</div>
    </div>
    <div class="badges">
      <span id="connBadge" class="badge warn">Connecting...</span>
      <span id="fsBadge" class="badge">Failsafe: Ready</span>
      <span id="cliBadge" class="badge">Clients: 1</span>
    </div>
  </header>

  <div class="card">
    <h2>Live Telemetry</h2>
    <div class="telem-grid">
      <div class="stat">
        <div class="lbl">Throttle In</div>
        <div class="val" id="telThr">0%</div>
      </div>
      <div class="stat">
        <div class="lbl">Motor Out (PWM)</div>
        <div class="val" id="telMotor">STOP (0)</div>
      </div>
      <div class="stat">
        <div class="lbl">Steering (50&ndash;130&deg;)</div>
        <div class="val" id="telServo">90&deg;</div>
      </div>
    </div>
    <div class="steer-bar-wrap" title="Servo Steering Angle (50 to 130 deg)">
      <div class="steer-bar-mid"></div>
      <div class="steer-dot" id="steerDot" style="left:50%"></div>
    </div>
  </div>

  <div class="grid">
    <!-- WASD Controls -->
    <div class="card">
      <h2>WASD / Touch Pad <span class="sub">Keyboard: W A S D + Space</span></h2>
      <div class="wasd-wrap">
        <div class="wasd-row">
          <button class="key-btn" id="btnW" data-key="w">W<span>FWD</span></button>
        </div>
        <div class="wasd-row">
          <button class="key-btn" id="btnA" data-key="a">A<span>LEFT</span></button>
          <button class="key-btn" id="btnS" data-key="s">S<span>REV</span></button>
          <button class="key-btn" id="btnD" data-key="d">D<span>RIGHT</span></button>
        </div>
        <button class="stop-btn" id="btnStop">SPACE &mdash; EMERGENCY STOP</button>
      </div>
      <div class="ctrl-group" style="margin-top:14px;margin-bottom:0">
        <div class="ctrl-head">
          <span>WASD Key Power</span>
          <span class="ctrl-val" id="wasdPowerVal">100%</span>
        </div>
        <input type="range" id="wasdPower" min="10" max="100" value="100">
      </div>
    </div>

    <!-- Analog Sliders -->
    <div class="card">
      <h2>Analog Sliders <span class="sub">No input = 0</span></h2>
      <div class="ctrl-group">
        <div class="ctrl-head">
          <span>Throttle (-100% Rev .. +100% Fwd)</span>
          <span class="ctrl-val" id="thrSliderVal">0%</span>
        </div>
        <input type="range" id="thrSlider" min="-100" max="100" value="0">
        <label class="chk-row">
          <input type="checkbox" id="thrSpring" checked> Return throttle to 0 on release
        </label>
      </div>

      <div class="ctrl-group" style="margin-bottom:4px">
        <div class="ctrl-head">
          <span>Steering (-100% Left .. +100% Right)</span>
          <span class="ctrl-val" id="steerSliderVal">0% (90&deg;)</span>
        </div>
        <input type="range" id="steerSlider" min="-100" max="100" value="0">
        <label class="chk-row">
          <input type="checkbox" id="steerSpring" checked> Center steering (90&deg;) on release
        </label>
      </div>
    </div>
  </div>

  <!-- Settings Card (Saved to Flash) -->
  <div class="card">
    <h2>Settings <span class="sub">Saved to ESP32 Flash (NVS)</span></h2>
    <div class="grid">
      <div>
        <div class="form-row">
          <div>
            <div class="form-lbl">Invert Steering</div>
            <div class="form-hint">Flips 50&deg; &harr; 130&deg; direction</div>
          </div>
          <label class="switch">
            <input type="checkbox" id="invSteer">
            <span class="slider-sw"></span>
          </label>
        </div>

        <div class="form-row">
          <div>
            <div class="form-lbl">Invert Motor Direction</div>
            <div class="form-hint">Flips forward &harr; reverse H-bridge polarity</div>
          </div>
          <label class="switch">
            <input type="checkbox" id="invMotor">
            <span class="slider-sw"></span>
          </label>
        </div>

        <div class="form-row" style="display:block">
          <div class="ctrl-head">
            <span>Motor Min Speed (when moving)</span>
            <span class="ctrl-val" id="minSpdVal">25%</span>
          </div>
          <input type="range" id="minSpd" min="0" max="100" value="25">
          <div class="form-hint">0 input always stops (0 PWM). Non-zero input scales between Min and Max.</div>
        </div>

        <div class="form-row" style="display:block">
          <div class="ctrl-head">
            <span>Motor Max Speed</span>
            <span class="ctrl-val" id="maxSpdVal">100%</span>
          </div>
          <input type="range" id="maxSpd" min="0" max="100" value="100">
        </div>
      </div>

      <div>
        <div class="form-row">
          <div>
            <div class="form-lbl">Failsafe Timeout (ms)</div>
            <div class="form-hint">Falls back to 0 if connection drops</div>
          </div>
          <input type="number" id="failsafeMs" min="100" max="5000" step="50" value="500">
        </div>

        <div class="form-row">
          <div>
            <div class="form-lbl">Wi-Fi AP Name (SSID)</div>
            <div class="form-hint">Reboot required after changing</div>
          </div>
          <input type="text" id="apSsid" maxlength="32" value="ESP32-RC-Car">
        </div>

        <div class="form-row">
          <div>
            <div class="form-lbl">Wi-Fi AP Password</div>
            <div class="form-hint">Min 8 chars (or empty for open AP)</div>
          </div>
          <input type="password" id="apPass" maxlength="64" placeholder="12345678">
        </div>
      </div>
    </div>

    <div class="btn-row">
      <button class="btn primary" id="btnSave">Save Settings to Flash</button>
      <button class="btn warn" id="btnSaveReboot">Save &amp; Reboot</button>
      <button class="btn danger" id="btnReset">Reset Defaults</button>
    </div>
    <div class="toast" id="toast"></div>
  </div>

  <!-- Pinout Card -->
  <div class="card">
    <h2>ESP32-C3 SuperMini Pinout</h2>
    <table class="pin-table">
      <thead>
        <tr><th>GPIO</th><th>Function</th><th>Notes</th></tr>
      </thead>
      <tbody>
        <tr><td><code>GPIO 3</code></td><td>Steering Servo Signal</td><td>50 Hz PWM, clamped 50&deg;&ndash;130&deg; (center 90&deg;)</td></tr>
        <tr><td><code>GPIO 4</code></td><td>H-Bridge IN1 (PWM)</td><td>Forward speed PWM (0 when reversing/stopped)</td></tr>
        <tr><td><code>GPIO 5</code></td><td>H-Bridge IN2 (PWM)</td><td>Reverse speed PWM (0 when forward/stopped)</td></tr>
        <tr><td><code>GPIO 8</code></td><td>Onboard Blue LED</td><td>Status indicator (active LOW)</td></tr>
      </tbody>
    </table>
  </div>
</div>

<script>
(() => {
  const $ = id => document.getElementById(id);
  let ws = null;
  let wsConnected = false;
  let throttle = 0;
  let steer = 0;
  let sliderThr = 0;
  let sliderSteer = 0;
  const keys = { w: false, a: false, s: false, d: false };

  function showToast(msg, isErr = false) {
    const t = $('toast');
    t.style.color = isErr ? 'var(--red)' : 'var(--green)';
    t.textContent = msg;
    setTimeout(() => { if (t.textContent === msg) t.textContent = ''; }, 3500);
  }

  function updateUIInputs() {
    $('thrSlider').value = throttle;
    $('thrSliderVal').textContent = throttle + '%';
    $('steerSlider').value = steer;
    const estDeg = Math.round(90 + (steer * 40) / 100);
    $('steerSliderVal').textContent = `${steer}% (${estDeg}\u00B0)`;
    $('btnW').classList.toggle('active', keys.w);
    $('btnA').classList.toggle('active', keys.a);
    $('btnS').classList.toggle('active', keys.s);
    $('btnD').classList.toggle('active', keys.d);
  }

  function recomputeFromKeys() {
    const pwr = parseInt($('wasdPower').value, 10) || 100;
    let kt = 0;
    if (keys.w && !keys.s) kt = pwr;
    else if (keys.s && !keys.w) kt = -pwr;

    let ks = 0;
    if (keys.a && !keys.d) ks = -100;
    else if (keys.d && !keys.a) ks = 100;

    // Keyboard takes priority while pressed, otherwise use slider values (0 when released)
    throttle = (keys.w || keys.s) ? kt : sliderThr;
    steer = (keys.a || keys.d) ? ks : sliderSteer;
    updateUIInputs();
    sendControl();
  }

  function emergencyStop() {
    keys.w = keys.a = keys.s = keys.d = false;
    sliderThr = 0;
    sliderSteer = 0;
    throttle = 0;
    steer = 0;
    updateUIInputs();
    sendControl();
  }

  function sendControl() {
    const payload = `C:${throttle},${steer}`;
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(payload);
    }
  }

  function connectWS() {
    const proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    ws = new WebSocket(`${proto}//${location.host}/ws`);
    ws.onopen = () => {
      wsConnected = true;
      $('connBadge').textContent = 'WS Connected';
      $('connBadge').className = 'badge ok';
      sendControl();
    };
    ws.onclose = () => {
      wsConnected = false;
      $('connBadge').textContent = 'Reconnecting...';
      $('connBadge').className = 'badge err';
      setTimeout(connectWS, 800);
    };
    ws.onerror = () => { ws.close(); };
    ws.onmessage = (ev) => {
      try {
        const d = JSON.parse(ev.data);
        if (d.type === 'tel') {
          $('telThr').textContent = `${d.t}%`;
          const dirTxt = d.dir > 0 ? 'FWD' : (d.dir < 0 ? 'REV' : 'STOP');
          const pct = Math.round((d.pwm / 255) * 100);
          $('telMotor').textContent = d.pwm === 0 ? 'STOP (0)' : `${dirTxt} ${pct}% (${d.pwm})`;
          $('telServo').textContent = `${d.ang}\u00B0`;
          const dotPct = ((d.ang - 50) / 80) * 100;
          $('steerDot').style.left = `${Math.max(0, Math.min(100, dotPct))}%`;
          $('cliBadge').textContent = `Clients: ${d.cli}`;
          $('fsBadge').textContent = d.fs ? 'Failsafe: ACTIVE' : 'Failsafe: OK';
          $('fsBadge').className = d.fs ? 'badge warn' : 'badge ok';
        }
      } catch (_) {}
    };
  }

  // Heartbeat keeps continuous holds alive against failsafe timeout
  setInterval(() => {
    if (wsConnected) sendControl();
  }, 100);

  // Keyboard bindings (ignore when typing in inputs)
  const keyMap = {
    KeyW: 'w', ArrowUp: 'w',
    KeyS: 's', ArrowDown: 's',
    KeyA: 'a', ArrowLeft: 'a',
    KeyD: 'd', ArrowRight: 'd'
  };
  window.addEventListener('keydown', (e) => {
    if (['INPUT', 'SELECT', 'TEXTAREA'].includes(document.activeElement.tagName)) return;
    if (e.code === 'Space') {
      e.preventDefault();
      $('btnStop').classList.add('active');
      emergencyStop();
      return;
    }
    const k = keyMap[e.code];
    if (k && !keys[k]) {
      e.preventDefault();
      keys[k] = true;
      recomputeFromKeys();
    }
  });
  window.addEventListener('keyup', (e) => {
    if (e.code === 'Space') {
      $('btnStop').classList.remove('active');
      return;
    }
    const k = keyMap[e.code];
    if (k && keys[k]) {
      keys[k] = false;
      recomputeFromKeys();
    }
  });
  window.addEventListener('blur', emergencyStop);

  // On-screen WASD touch/mouse buttons
  document.querySelectorAll('.key-btn').forEach(btn => {
    const k = btn.dataset.key;
    const down = (e) => { e.preventDefault(); keys[k] = true; recomputeFromKeys(); };
    const up = (e) => { e.preventDefault(); if (keys[k]) { keys[k] = false; recomputeFromKeys(); } };
    btn.addEventListener('pointerdown', down);
    btn.addEventListener('pointerup', up);
    btn.addEventListener('pointercancel', up);
    btn.addEventListener('pointerleave', up);
  });
  $('btnStop').addEventListener('click', emergencyStop);

  $('wasdPower').addEventListener('input', () => {
    $('wasdPowerVal').textContent = $('wasdPower').value + '%';
    if (keys.w || keys.s) recomputeFromKeys();
  });

  // Analog sliders
  $('thrSlider').addEventListener('input', () => {
    sliderThr = parseInt($('thrSlider').value, 10) || 0;
    throttle = sliderThr;
    updateUIInputs();
    sendControl();
  });
  const releaseThr = () => {
    if ($('thrSpring').checked) {
      sliderThr = 0;
      throttle = 0;
      updateUIInputs();
      sendControl();
    }
  };
  $('thrSlider').addEventListener('pointerup', releaseThr);
  $('thrSlider').addEventListener('pointercancel', releaseThr);
  $('thrSlider').addEventListener('touchend', releaseThr);
  $('thrSlider').addEventListener('touchcancel', releaseThr);
  $('thrSlider').addEventListener('change', () => {
    if ($('thrSpring').checked && !keys.w && !keys.s) releaseThr();
  });

  $('steerSlider').addEventListener('input', () => {
    sliderSteer = parseInt($('steerSlider').value, 10) || 0;
    steer = sliderSteer;
    updateUIInputs();
    sendControl();
  });
  const releaseSteer = () => {
    if ($('steerSpring').checked) {
      sliderSteer = 0;
      steer = 0;
      updateUIInputs();
      sendControl();
    }
  };
  $('steerSlider').addEventListener('pointerup', releaseSteer);
  $('steerSlider').addEventListener('pointercancel', releaseSteer);
  $('steerSlider').addEventListener('touchend', releaseSteer);
  $('steerSlider').addEventListener('touchcancel', releaseSteer);
  $('steerSlider').addEventListener('change', () => {
    if ($('steerSpring').checked && !keys.a && !keys.d) releaseSteer();
  });

  // Min/Max speed slider coupling
  $('minSpd').addEventListener('input', () => {
    let minV = parseInt($('minSpd').value, 10);
    let maxV = parseInt($('maxSpd').value, 10);
    if (minV > maxV) {
      maxV = minV;
      $('maxSpd').value = maxV;
      $('maxSpdVal').textContent = maxV + '%';
    }
    $('minSpdVal').textContent = minV + '%';
  });
  $('maxSpd').addEventListener('input', () => {
    let minV = parseInt($('minSpd').value, 10);
    let maxV = parseInt($('maxSpd').value, 10);
    if (maxV < minV) {
      minV = maxV;
      $('minSpd').value = minV;
      $('minSpdVal').textContent = minV + '%';
    }
    $('maxSpdVal').textContent = maxV + '%';
  });

  async function loadSettings() {
    try {
      const r = await fetch('/api/status');
      const d = await r.json();
      const s = d.settings;
      $('invSteer').checked = !!s.invertSteering;
      $('invMotor').checked = !!s.invertMotor;
      $('minSpd').value = s.motorMinSpeed;
      $('minSpdVal').textContent = s.motorMinSpeed + '%';
      $('maxSpd').value = s.motorMaxSpeed;
      $('maxSpdVal').textContent = s.motorMaxSpeed + '%';
      $('failsafeMs').value = s.failsafeMs;
      $('apSsid').value = s.apSsid;
      $('apPass').value = s.apPass;
    } catch (e) {
      showToast('Failed to load settings', true);
    }
  }

  async function saveSettings(rebootAfter = false) {
    const body = {
      invertSteering: $('invSteer').checked,
      invertMotor: $('invMotor').checked,
      motorMinSpeed: parseInt($('minSpd').value, 10),
      motorMaxSpeed: parseInt($('maxSpd').value, 10),
      failsafeMs: parseInt($('failsafeMs').value, 10) || 500,
      apSsid: $('apSsid').value.trim(),
      apPass: $('apPass').value
    };
    if (body.apPass.length > 0 && body.apPass.length < 8) {
      showToast('Wi-Fi password must be at least 8 chars (or empty for open)', true);
      return;
    }
    try {
      const r = await fetch('/api/settings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body)
      });
      if (!r.ok) throw new Error('HTTP ' + r.status);
      const res = await r.json();
      $('minSpd').value = res.settings.motorMinSpeed;
      $('minSpdVal').textContent = res.settings.motorMinSpeed + '%';
      $('maxSpd').value = res.settings.motorMaxSpeed;
      $('maxSpdVal').textContent = res.settings.motorMaxSpeed + '%';
      if (rebootAfter) {
        showToast('Saved! Rebooting ESP32-C3...');
        await fetch('/api/reboot', { method: 'POST' });
      } else {
        showToast('Settings saved to flash!');
      }
    } catch (e) {
      showToast('Error saving settings', true);
    }
  }

  // Auto-save invert toggles immediately so rebooting right after never loses them
  $('invSteer').addEventListener('change', () => saveSettings(false));
  $('invMotor').addEventListener('change', () => saveSettings(false));
  $('btnSave').addEventListener('click', () => saveSettings(false));
  $('btnSaveReboot').addEventListener('click', () => saveSettings(true));
  $('btnReset').addEventListener('click', async () => {
    try {
      await fetch('/api/reset', { method: 'POST' });
      await loadSettings();
      showToast('Defaults restored and saved to flash!');
    } catch (e) {
      showToast('Failed to reset defaults', true);
    }
  });

  loadSettings();
  connectWS();
})();
</script>
</body>
</html>
)rawliteral";
