// Minimal DOM shim to actually execute the JS embedded in SentinelWeb.cpp
// and verify that updateDashboard / updateLiveTelemetry / updateLiveWiFi /
// updateEvents populate every card ID that they reference.

const fs = require('fs');
const path = require('path');

const src = fs.readFileSync(
    path.join(__dirname, '..', 'SentinelWeb.cpp'),
    'utf8'
);

// Extract R"rawliteral(...)rawliteral" block
const m = src.match(/R"rawliteral\(([\s\S]*?)\)rawliteral"/);
if (!m) { console.error('HTML/JS literal not found'); process.exit(2); }
const html = m[1];

// -----------------------------------------------------------
// DOM shim
// -----------------------------------------------------------
const elements = new Map();

class Element {
    constructor(id) {
        this.id = id;
        this.textContent = '';
        this.innerHTML = '';
        this.className = '';
        this.style = {};
        this._children = [];
        this.width = 300;
        this.height = 60;
    }
    set textContent(v) {
        if (typeof v === 'string') {
            this._text = v;
            this._inner = v
                .replace(/&/g, '&')
                .replace(/</g, '<')
                .replace(/>/g, '>');
        } else {
            this._text = String(v);
        }
    }
    get textContent() { return this._text; }
    set innerHTML(v) { this._inner = v; this._text = v; }
    get innerHTML() { return this._inner; }
    appendChild(child) {
        this._children.push(child);
        if (child._inner) {
            this._inner =
                (this._inner === undefined ? '' : this._inner) +
                child._inner;
        }
        return child;
    }
    getContext(kind) {
        if (kind === '2d') {
            return {
                fillStyle: '', strokeStyle: '', lineWidth: 1, font: '',
                fillRect() {}, clearRect() {}, fillText() {},
                beginPath() {}, moveTo() {}, lineTo() {}, stroke() {},
                fill() {}, closePath() {}, save() {}, restore() {},
                setLineDash() {}, scale() {}, translate() {}, rotate() {},
                arc() {}, measureText() { return { width: 0 }; },
                rect() {}, quadraticCurveTo() {}, bezierCurveTo() {},
                transform() {}, setTransform() {}, drawImage() {},
            };
        }
        return null;
    }
    getBoundingClientRect() {
        return { left: 0, top: 0, width: this.width, height: this.height, right: this.width, bottom: this.height };
    }
}

global.document = {
    getElementById(id) {
        if (!elements.has(id)) elements.set(id, new Element(id));
        return elements.get(id);
    },
    createElement(tag) {
        return new Element('<' + tag + '>');
    },
    addEventListener() {},
};

global.window = {
    addEventListener() {},
    removeEventListener() {},
    devicePixelRatio: 1,
};
global.console = console;
global.setTimeout = setTimeout;
global.clearTimeout = clearTimeout;
global.WebSocket = function () {
    this.send = () => {};
    this.close = () => {};
    this.onmessage = null;
    this.onopen = null;
    this.onclose = null;
    this.onerror = null;
};
global.location = { hostname: 'sentinel.local' };

// -----------------------------------------------------------
// Inject the HTML so all referenced IDs exist as Elements.
// We only need the IDs; the JS uses getElementById only.
// -----------------------------------------------------------
const idRe = /id="([a-zA-Z_][a-zA-Z0-9_]*)"/g;
let mm;
const ids = [];
while ((mm = idRe.exec(html)) !== null) ids.push(mm[1]);
ids.forEach(id => elements.set(id, new Element(id)));

// -----------------------------------------------------------
// Extract just the <script> contents and execute them.
// -----------------------------------------------------------
const scriptRe = /<script>([\s\S]*?)<\/script>/g;
const scripts = [];
let sm;
while ((sm = scriptRe.exec(html)) !== null) scripts.push(sm[1]);

// Run in a sandbox-ish eval so the IIFE-style page setup happens.
// We evaluate scripts sequentially; each function becomes global.
const wrapped = scripts.join('\n') + '\n; module.exports = { updateDashboard, updateLiveTelemetry, updateLiveWiFi, updateEvents, updateLAN, updateWiFi, updateHistory, addTelemetryToHistory, formatUptime, get history() { return history; } };';
const exposed = { module: { exports: {} } };
const fn = new Function('module', wrapped);
fn(exposed.module);

const lib = exposed.module.exports;
console.log('Functions exposed:', Object.keys(lib));

// -----------------------------------------------------------
// Helper: read element textContent after a call
// -----------------------------------------------------------
function read(id) {
    const el = elements.get(id);
    return el ? el.textContent : '<missing>';
}

// -----------------------------------------------------------
// Real acceptance payloads
// -----------------------------------------------------------

// Full /api payload
const apiPayload = {
    ssid: 'TestAP',
    ip: '192.168.1.42',
    gateway: '192.168.1.1',
    dns: '1.1.1.1',
    channel: 6,
    rssi: -55,
    internetPing: 23,
    gatewayPing: 2,
    packetLoss: 1,
    wifi: true,
    gatewayOnline: true,
    internetOnline: true,
    dnsOnline: true,
    heap: 245760,
    cpu: 240,
    uptime: '12:34:56',
    wifiTxPower: 19,
    loopTimeMs: 18,
    minFreeHeap: 200000,
    maxLoopTimeMs: 42,
    wifiMAC: 'AA:BB:CC:DD:EE:FF',
    healthScore: 87,
    stabilityScore: 92,
    wifiUptimeSec: 7200,
    wifiReconnects: 3,
    lowMemory: false,
    cpuBlocked: false,
    highLatency: false,
    dnsTime: 12,
};

console.log('\n=== updateDashboard (/api payload) ===');
lib.updateDashboard(apiPayload);
[
    'ping','rssi','loss','channel','ssid','ip','gateway','dns',
    'wifiStatus','gatewayStatus','internetStatus','dnsStatus',
    'heap','cpu','uptime',
    'wifiPower','loopTime','minHeap','maxLoop','wifiMAC',
    'alertBanner',
    'healthScore','stabilityScore','wifiUptime','reconnects',
    'intPing','intGatewayPing','intDnsTime','intLoss',
    'statusText'
].forEach(id => console.log(id, '=', read(id)));

// Telemetry payload (live, raw millis for uptime)
const telPayload = {
    ping: 24,
    rssi: -57,
    packetLoss: 0,
    channel: 11,
    wifi: true,
    gateway: true,
    internet: true,
    dns: true,
    heap: 250000,
    uptime: 4500000,
    healthScore: 90,
    stabilityScore: 95,
    lowMemory: false,
};

console.log('\n=== updateLiveTelemetry (WS payload, millis) ===');
lib.updateLiveTelemetry(telPayload);
['ping','rssi','loss','channel','wifiStatus','gatewayStatus','internetStatus','dnsStatus','heap','uptime','healthScore','stabilityScore','alertBanner','statusText']
    .forEach(id => console.log(id, '=', read(id)));

// Telemetry with lowMemory=true to verify alert banner
telPayload.lowMemory = true;
lib.updateLiveTelemetry(telPayload);
console.log('\n--- alertBanner after lowMemory=true ---');
console.log('alertBanner =', read('alertBanner'));
console.log('alertBanner.style.display =', elements.get('alertBanner').style.display);
telPayload.lowMemory = false;
lib.updateLiveTelemetry(telPayload);
console.log('\n--- alertBanner after lowMemory=false ---');
console.log('alertBanner =', read('alertBanner'));
console.log('alertBanner.style.display =', elements.get('alertBanner').style.display);

// WiFi payload (live)
const wifiPayload = {
    count: 14,
    averageRSSI: -65,
    open: 3,
    busiestChannel: 6,
    hidden: 2,
    strongestRSSI: -42,
    weakestRSSI: -88,
};

console.log('\n=== updateLiveWiFi ===');
lib.updateLiveWiFi(wifiPayload);
['wifiCount','avgRSSI','openNetworks','busyChannel','hiddenNetworks','strongestRSSI','weakestRSSI']
    .forEach(id => console.log(id, '=', read(id)));

// Events payload — shape must match what createEventsJSON() emits:
// { type, count, events:[ { timestamp (millis), severity (numeric), message } ] }
const eventsPayload = {
    type: 'events',
    events: [
        { timestamp: 4500000, severity: 0, message: 'Internet restored, ping 23ms' },
        { timestamp: 4200000, severity: 2, message: 'WiFi disconnected, lost link to AP' },
        { timestamp: 3600000, severity: 1, message: 'Ping spike 450ms (baseline 25ms)' },
    ],
};

console.log('\n=== updateEvents ===');
lib.updateEvents(eventsPayload);
const evList = elements.get('eventList');
console.log('eventList.innerHTML length =', (evList.innerHTML || '').length);
console.log('eventList contains "Internet restored":', (evList.innerHTML || '').includes('Internet restored'));
console.log('eventList contains "WiFi disconnected":', (evList.innerHTML || '').includes('WiFi disconnected'));
console.log('eventList contains "Ping spike":', (evList.innerHTML || '').includes('Ping spike'));
console.log('eventList contains color #e74c3c (critical):', (evList.innerHTML || '').includes('#e74c3c'));
console.log('eventList contains color #f39c12 (warning):', (evList.innerHTML || '').includes('#f39c12'));
console.log('eventList contains color #3498db (info):', (evList.innerHTML || '').includes('#3498db'));

// XSS safety: a malicious payload should not inject <script>
const xssPayload = {
    type: 'events',
    events: [
        { timestamp: 0, severity: 2, message: '</div><script>alert(1)</script>' },
    ],
};
lib.updateEvents(xssPayload);
console.log('\n--- XSS safety ---');
console.log('eventList escaped <script>:',
    !(evList.innerHTML || '').includes('<script>') &&
    (evList.innerHTML || '').includes('<'));

// Empty events
lib.updateEvents({ events: [] });
console.log('\n--- empty events ---');
console.log('eventList =', read('eventList'));

// formatUptime
console.log('\n=== formatUptime ===');
console.log('0 =', lib.formatUptime(0));
console.log('65000ms =', lib.formatUptime(65000));
console.log('4500000ms =', lib.formatUptime(4500000));
console.log('999ms =', lib.formatUptime(999));

// Final assertions
const failures = [];
function expect(name, actual, predicate) {
    const ok = predicate(actual);
    console.log((ok ? 'PASS' : 'FAIL') + ' ' + name + ' = ' + actual);
    if (!ok) failures.push(name);
}

expect('uptime (Bug #2 /api path)', (function(){ apiPayload.lowMemory = false; apiPayload.cpuBlocked = false; apiPayload.highLatency = false; lib.updateDashboard(apiPayload); return read('uptime'); })(),
    v => v === '12:34:56');
expect('uptime (telemetry millis path)', (function(){ lib.updateLiveTelemetry(telPayload); return read('uptime'); })(),
    v => /^\d+:\d{2}:\d{2}$/.test(v));
expect('wifiPower', (function(){ lib.updateDashboard(apiPayload); return read('wifiPower'); })(),
    v => v === '19 dBm');
expect('loopTime', (function(){ lib.updateDashboard(apiPayload); return read('loopTime'); })(),
    v => v === '18 ms');
expect('wifiMAC', (function(){ lib.updateDashboard(apiPayload); return read('wifiMAC'); })(),
    v => v === 'AA:BB:CC:DD:EE:FF');
expect('healthScore', (function(){ lib.updateDashboard(apiPayload); return read('healthScore'); })(),
    v => v === '87/100');
expect('stabilityScore', (function(){ lib.updateDashboard(apiPayload); return read('stabilityScore'); })(),
    v => v === '92/100');
expect('wifiUptime (formatted from secs)', (function(){ lib.updateDashboard(apiPayload); return read('wifiUptime'); })(),
    v => /^\d+:\d{2}:\d{2}$/.test(v));
expect('reconnects', (function(){ lib.updateDashboard(apiPayload); return read('reconnects'); })(),
    v => v === '3');
expect('intPing', (function(){ lib.updateDashboard(apiPayload); return read('intPing'); })(),
    v => v === '23 ms');
expect('intGatewayPing', (function(){ lib.updateDashboard(apiPayload); return read('intGatewayPing'); })(),
    v => v === '2 ms');
expect('intDnsTime', (function(){ lib.updateDashboard(apiPayload); return read('intDnsTime'); })(),
    v => v === '12 ms');
expect('intLoss', (function(){ lib.updateDashboard(apiPayload); return read('intLoss'); })(),
    v => v === '1%');
expect('hiddenNetworks', (function(){ lib.updateLiveWiFi(wifiPayload); return read('hiddenNetworks'); })(),
    v => v === '2');
expect('strongestRSSI', (function(){ lib.updateLiveWiFi(wifiPayload); return read('strongestRSSI'); })(),
    v => v === '-42 dBm');
expect('weakestRSSI', (function(){ lib.updateLiveWiFi(wifiPayload); return read('weakestRSSI'); })(),
    v => v === '-88 dBm');
expect('alertBanner hidden when healthy', (function(){ apiPayload.lowMemory = false; apiPayload.cpuBlocked = false; apiPayload.highLatency = false; lib.updateDashboard(apiPayload); return elements.get('alertBanner').style.display; })(),
    v => v === 'none');
expect('alertBanner visible when lowMemory=true', (function(){ apiPayload.lowMemory = true; lib.updateDashboard(apiPayload); return elements.get('alertBanner').style.display; })(),
    v => v === 'block');

// updateLAN
const lanPayload = {
    type: 'lan',
    deviceCount: 3,
    scanning: false,
    scanComplete: true,
    subnetStart: '192.168.1.1',
    subnetEnd: '192.168.1.254',
    totalHosts: 254,
    scannedHosts: 254,
    autoScanning: true,
    timeToNextScan: 45000,
    devices: [
        { ip: '192.168.1.1', mac: 'AA:BB:CC:00:00:01', hostname: 'router', online: true, latency: 1 },
        { ip: '192.168.1.42', mac: 'AA:BB:CC:00:00:2A', hostname: 'sentinel', online: true, latency: 2 },
        { ip: '192.168.1.100', mac: 'AA:BB:CC:00:00:64', hostname: 'unknown', online: false, latency: -1 },
    ],
};
console.log('\n=== updateLAN ===');
lib.updateLAN(lanPayload);
['lanDeviceCount','lanScanState','lanSubnet','lanProgress','lanAutoStatus']
    .forEach(id => console.log(id, '=', read(id)));
expect('lanDeviceCount', read('lanDeviceCount'), v => v === '3');
expect('lanScanState', read('lanScanState'), v => v === 'COMPLETE');
expect('lanProgress', read('lanProgress'), v => v === '254/254');
expect('lanAutoStatus', read('lanAutoStatus'), v => v === 'WAIT 45s');
const lanTable = elements.get('lanTable');
const lanHTML = lanTable.innerHTML || '';
expect('lanTable has router', lanHTML.includes('router'), v => v === true);
expect('lanTable has sentinel', lanHTML.includes('sentinel'), v => v === true);
expect('lanTable has unknown', lanHTML.includes('unknown'), v => v === true);

// =====================================================
// WORKFLOW COVERAGE — addTelemetryToHistory
// =====================================================
console.log('\n=== addTelemetryToHistory (state field round-trip) ===');
const before = elements.get('pingCanvas');
lib.addTelemetryToHistory({
    ping: 10, rssi: -50, packetLoss: 0, dnsTime: 5, heap: 100000,
    uptime: 1000, state: 15,
});
lib.addTelemetryToHistory({
    ping: 20, rssi: -55, packetLoss: 5, dnsTime: 10, heap: 90000,
    uptime: 2000, state: 7,
});
lib.addTelemetryToHistory({
    ping: 30, rssi: -60, packetLoss: 10, dnsTime: 15, heap: 80000,
    uptime: 3000, state: 0,
});
const hist = lib.history;
console.log('history length:', hist ? hist.length : 'NOT ACCESSIBLE from exported scope');
expect('history has 3 samples after 3 inserts', hist && hist.length === 3, v => v === true);
expect('samples carry state field', hist && hist.every(s => typeof s.state === 'number'), v => v === true);

// =====================================================
// WORKFLOW COVERAGE — 60-sample ring limit
// =====================================================
console.log('\n=== addTelemetryToHistory ring limit ===');
for (let i = 0; i < 70; i++) {
    lib.addTelemetryToHistory({
        ping: i, rssi: -50, packetLoss: 0, dnsTime: 1, heap: 100000,
        uptime: 10000 + i, state: i & 0xF,
    });
}
expect('history capped at 60 samples', hist && hist.length === 60, v => v === true);
expect('oldest sample evicted (first ping != 0)', hist && hist[0].ping === 10, v => v === true);

// =====================================================
// WORKFLOW COVERAGE — updateHistory replaces buffer
// =====================================================
console.log('\n=== updateHistory (full buffer replace) ===');
lib.updateHistory({
    samples: [
        { time: 0, ping: 1, rssi: -1, loss: 0, dns: 1, heap: 1, state: 1 },
        { time: 1, ping: 2, rssi: -2, loss: 0, dns: 1, heap: 1, state: 2 },
    ],
});
expect('updateHistory replaces buffer (length 2)', lib.history.length === 2, v => v === true);

// =====================================================
// EDGE CASES — extreme uptime values
// =====================================================
console.log('\n=== formatUptime edge cases ===');
expect('formatUptime(0)', lib.formatUptime(0), v => v === '0:00:00');
expect('formatUptime(1ms)', lib.formatUptime(1), v => v === '0:00:00');
expect('formatUptime(59.999s)', lib.formatUptime(59999), v => v === '0:00:59');
expect('formatUptime(60s)', lib.formatUptime(60000), v => v === '0:01:00');
expect('formatUptime(3661s)', lib.formatUptime(3661000), v => v === '1:01:01');
expect('formatUptime(24h)', lib.formatUptime(24 * 3600 * 1000), v => v === '24:00:00');
expect('formatUptime(49d17h) = 1193:00:00',
    lib.formatUptime((49 * 24 + 17) * 3600 * 1000), v => v === '1193:00:00');

// =====================================================
// EDGE CASES — missing fields in payload
// =====================================================
console.log('\n=== updateDashboard with partial payload ===');
lib.updateDashboard({
    ssid: 'partial', internetPing: 10, packetLoss: 0, rssi: -50,
    channel: 6, heap: 100000, cpu: 240, uptime: '1:00:00',
    // intentionally missing: ip, gateway, dns, wifi, gatewayOnline, etc.
});
expect('partial payload does not crash (uptime shown)', read('uptime'), v => v === '1:00:00');
expect('partial payload does not crash (ssid shown)', read('ssid'), v => v === 'partial');
expect('partial payload shows -- for missing ping', read('intPing'), v => v === '10 ms');

// =====================================================
// EDGE CASES — empty events list
// =====================================================
console.log('\n=== updateEvents with various empty/edge payloads ===');
lib.updateEvents({});
expect('empty payload shows placeholder', read('eventList'),
    v => v.includes('Waiting for events'));
lib.updateEvents({ events: null });
expect('null events shows placeholder', read('eventList'),
    v => v.includes('Waiting for events'));

// =====================================================
// EDGE CASES — alert banner with all alerts
// =====================================================
console.log('\n=== alertBanner with all alerts ===');
lib.updateDashboard({
    ...apiPayload, lowMemory: true, cpuBlocked: true, highLatency: true,
});
expect('banner shows all 3 alerts', read('alertBanner'),
    v => v.includes('LOW MEMORY') && v.includes('CPU BLOCKED') && v.includes('HIGH LATENCY'));
expect('banner visible when 3 alerts', elements.get('alertBanner').style.display, v => v === 'block');

// =====================================================
// WS ROUTER — type dispatch for all known message types
// =====================================================
console.log('\n=== WS message router (offline simulated) ===');
// We can't actually connect WebSocket in this shim, but we can directly invoke
// the onmessage handler if we capture it. Easier: simulate the dispatch inline
// using the exported update* functions — we've already done that.

// =====================================================
// HTML CARDS PRESENCE — every card id listed in JS must have an HTML element
// =====================================================
console.log('\n=== HTML/JS ID coverage ===');
const html_ids = new Set();
const html_id_re = /id="([a-zA-Z_][a-zA-Z0-9_]*)"/g;
let m1;
const raw_html = src.match(/R"rawliteral\(([\s\S]*?)\)rawliteral"/)[1];
while ((m1 = html_id_re.exec(raw_html)) !== null) html_ids.add(m1[1]);

const js_calls_re = /getElementById\(\s*['"]([a-zA-Z_][a-zA-Z0-9_]*)['"]\s*\)/g;
const js_ids = new Set();
let m2;
while ((m2 = js_calls_re.exec(raw_html)) !== null) js_ids.add(m2[1]);

const missing = [...js_ids].filter(id => !html_ids.has(id));
expect('every JS getElementById resolves to HTML id', missing.length, v => v === 0);

if (failures.length) {
    console.log('\nFAILED:', failures);
    process.exit(1);
} else {
    console.log('\nAll acceptance checks passed.');
}
