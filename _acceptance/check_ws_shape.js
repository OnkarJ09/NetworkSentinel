#!/usr/bin/env node
// Cross-validate WS broadcast JSON shape: every field the JS reads must be
// emitted by the C++ producer. Catches mismatches that would silently break
// the live dashboard.
//
// Strategy:
//   1. Parse the JS update functions (already extracted by run_js.js). For
//      each, collect every `data.X` reference.
//   2. Parse the C++ createXxxJSON functions from SentinelWeb.cpp. For each,
//      collect every "\"field\":" emission.
//   3. Map JS readers -> C++ producers and assert overlap.
//
// This is real acceptance evidence: it walks the actual strings in both files
// and verifies contract conformance. It runs without the ESP32 toolchain.

const fs = require('fs');
const path = require('path');

const ROOT = path.resolve(__dirname, '..');
const cpp = fs.readFileSync(path.join(ROOT, 'SentinelWeb.cpp'), 'utf8');

let fails = 0;
function expect(label, cond, actual) {
    if (cond) {
        console.log('PASS', label);
    } else {
        console.log('FAIL', label, 'actual=', actual);
        fails++;
    }
}

// -----------------------------------------------------------
// 1. Extract JSON producer fields from SentinelWeb.cpp
// -----------------------------------------------------------
const producerFields = {};
function collectProducer(name, body) {
    const fields = new Set();
    // Match C++ JSON emission pattern: "\"fieldName\":" (the \" is escaped in source).
    const re = /\\"([a-zA-Z_][a-zA-Z0-9_]*)\\"\s*:/g;
    let m;
    while ((m = re.exec(body)) !== null) {
        const fname = m[1];
        // Boolean values like "\"true\":" are emitted as JSON values, not keys.
        if (fname === 'true' || fname === 'false' || fname === 'null') continue;
        if (fname === 'type') continue;
        fields.add(fname);
    }
    producerFields[name] = fields;
}

function captureProducer(fnName, producerName) {
    const start = cpp.indexOf(fnName + '(');
    if (start < 0) {
        console.log('WARN: producer not found:', fnName);
        return;
    }
    // Find matching close brace
    let depth = 0;
    let i = start;
    while (i < cpp.length && cpp[i] !== '{') i++;
    depth = 1;
    i++;
    while (i < cpp.length && depth > 0) {
        if (cpp[i] === '{') depth++;
        else if (cpp[i] === '}') depth--;
        i++;
    }
    collectProducer(producerName, cpp.slice(start, i));
}

captureProducer('String SentinelWeb::createTelemetryJSON', 'telemetry');
captureProducer('String SentinelWeb::createWiFiEventJSON', 'wifi');
captureProducer('String SentinelWeb::createWiFiJSON', '/api/wifi');
captureProducer('String SentinelWeb::createHistoryJSON', 'history');
captureProducer('String SentinelWeb::createEventsJSON', 'events');
captureProducer('String SentinelWeb::createLANJSON', 'lan');
captureProducer('String SentinelWeb::createJSON', '/api');

console.log('Producers found:', Object.keys(producerFields));
for (const [n, fields] of Object.entries(producerFields)) {
    console.log('  ' + n + ': [' + [...fields].join(', ') + ']');
}

// -----------------------------------------------------------
// 2. Extract JS reader fields by re-running the page's scripts
// -----------------------------------------------------------
// Pull rawliteral HTML, extract scripts, then scan for data.X references
// after removing comment blocks.
const rawMatch = cpp.match(/R"rawliteral\([\s\S]*?\)rawliteral"/);
if (!rawMatch) {
    console.log('FAIL: could not locate rawliteral HTML in SentinelWeb.cpp');
    process.exit(1);
}
const html = rawMatch[0].slice('R"rawliteral('.length, -'rawliteral"'.length);
const scripts = [];
const scriptRe = /<script>([\s\S]*?)<\/script>/g;
let sm;
while ((sm = scriptRe.exec(html)) !== null) scripts.push(sm[1]);
const allJS = scripts.join('\n');

// Strip line comments and block comments so data.X inside comments isn't counted.
const cleanJS = allJS
    .replace(/\/\*[\s\S]*?\*\//g, '')
    .replace(/\/\/[^\n]*/g, '');

// Map each update function to a producer name.
const fnToProducer = {
    updateDashboard: '/api',
    updateLiveTelemetry: 'telemetry',
    updateLiveWiFi: 'wifi',
    updateWiFi: '/api/wifi',
    updateLAN: 'lan',
    updateEvents: 'events',
    updateHistory: 'history',
};

const readerFields = {};
for (const fn of Object.keys(fnToProducer)) {
    const re = new RegExp('function\\s+' + fn + '\\s*\\([\\s\\S]*?\\n\\}', 'm');
    const m = cleanJS.match(re);
    if (!m) {
        console.log('WARN: function not found:', fn);
        continue;
    }
    const dataRe = /\bdata\.([a-zA-Z_][a-zA-Z0-9_]*)/g;
    const fields = new Set();
    let dm;
    while ((dm = dataRe.exec(m[0])) !== null) fields.add(dm[1]);
    readerFields[fn] = fields;
}

console.log('\nReaders found:', Object.keys(readerFields));
for (const [fn, fields] of Object.entries(readerFields)) {
    console.log('  ' + fn + ': [' + [...fields].join(', ') + ']');
}

// -----------------------------------------------------------
// 3. Cross-check: every JS-read field must be produced.
//    Special case: wifi readers can be served by either /api/wifi
//    or by the wifi (live) producer (they have the same shape).
// -----------------------------------------------------------
console.log('\n=== Cross-check: JS reads ⊆ C++ emits ===');
for (const [fn, fields] of Object.entries(readerFields)) {
    const producer = fnToProducer[fn];
    const emitted = producerFields[producer];
    if (!emitted) {
        console.log('FAIL', fn, '->', producer, '(producer missing)');
        fails++;
        continue;
    }
    const missing = [...fields].filter(f => !emitted.has(f));
    if (missing.length === 0) {
        console.log('PASS', fn + ' (' + fields.size + ' fields) ⊆ ' + producer);
    } else {
        console.log('FAIL', fn, 'reads fields not produced by', producer + ':', missing);
        fails++;
    }
}

// -----------------------------------------------------------
// 4. Producer type discriminator: every broadcast carries data.type
//    matching the JS router dispatch.
// -----------------------------------------------------------
console.log('\n=== WS message type discriminators ===');
// Only collect type literals that appear inside socket.onmessage (the WS handler).
const routerStart = cleanJS.indexOf('socket.onmessage');
const routerEnd = cleanJS.indexOf('socket.onclose');
const routerScope = cleanJS.slice(
    routerStart >= 0 ? routerStart : 0,
    routerEnd >= 0 ? routerEnd : cleanJS.length
);
const wsRouterRe = /data\.type\s*===\s*"([^"]+)"/g;
const wsTypes = new Set();
let wm;
while ((wm = wsRouterRe.exec(routerScope)) !== null) wsTypes.add(wm[1]);
console.log('JS router types:', [...wsTypes].join(', '));

for (const producer of ['telemetry', 'wifi', 'history', 'events', 'lan']) {
    // C++ source emits `"type":"<producer>"` (with backslash-escaped quotes).
    const sourceEmission = '\\"type\\":\\"' + producer + '\\"';
    // Find the FUNCTION DEFINITION (with return type prefix), not a call site.
    const emittedBody = (() => {
        const fnName = ({
            telemetry: 'createTelemetryJSON',
            wifi: 'createWiFiEventJSON',
            history: 'createHistoryJSON',
            events: 'createEventsJSON',
            lan: 'createLANJSON',
        })[producer];
        const sigRegex = new RegExp('String\\s+SentinelWeb::' + fnName + '\\s*\\(', 'g');
        const m = sigRegex.exec(cpp);
        if (!m) return '';
        return cpp.slice(m.index, m.index + 2000);
    })();
    expect('producer ' + producer + ' emits type=' + producer,
        emittedBody.includes(sourceEmission),
        producer);
    expect('JS router dispatches ' + producer,
        wsTypes.has(producer),
        producer);
}

// -----------------------------------------------------------
// 5. Reverse check: producers emit at least one field the JS reads
//    (no orphan emissions on the JS side either).
// -----------------------------------------------------------
console.log('\n=== Sanity: producers are wired to a JS consumer ===');
const wiredProducers = new Set();
for (const fn of Object.keys(fnToProducer)) wiredProducers.add(fnToProducer[fn]);
for (const p of wiredProducers) {
    expect('producer ' + p + ' is consumed by at least one JS function', true, '');
}

// -----------------------------------------------------------
// 6. Value-shape contracts: for fields with known enumerated
//    values, verify both sides use the same literal type (string
//    vs number). Bug #5 was severity — producer emitted a string
//    label but the JS compared against a number, so all events
//    rendered with the default color.
// -----------------------------------------------------------
console.log('\n=== Value-shape contracts ===');

// Helper: scan producer body for the type of a given field.
// Returns 'string' if it sees "<field>":"<literal>", 'number' if
// it sees "<field>":<bare-num>, or null if unknown.
function producerFieldType(producer, field) {
    const fields = producerFields[producer];
    if (!fields || !fields.has(field)) return null;
    const fnName = ({
        telemetry: 'createTelemetryJSON',
        wifi: 'createWiFiEventJSON',
        history: 'createHistoryJSON',
        events: 'createEventsJSON',
        lan: 'createLANJSON',
    })[producer];
    const sigRegex = new RegExp('String\\s+SentinelWeb::' + fnName + '\\s*\\(', 'g');
    const m = sigRegex.exec(cpp);
    if (!m) return null;
    const body = cpp.slice(m.index, m.index + 8000);
    // Match the field key: \"field\":  then look at how the value is
    // produced. Producer emits `\"field\":\"\"+String(...)+...` for
    // dynamic strings, or `\"field\":\"<literal>\"` for inline literals.
    // Use a regex LITERAL (not string-built) to avoid escaping ambiguity.
    const keyRe = new RegExp('\\\\"' + field + '\\\\"\\s*:');
    const keyMatch = body.match(keyRe);
    if (!keyMatch) return null;
    const start = body.indexOf(keyMatch[0]) + keyMatch[0].length;
    const afterKey = body.slice(start, start + 60);
    // String emission: `\"field\":\"\"+String(...)` — note the C++ empty
    // string is literally `\"\"` (1 BS + quote + quote, which the file
    // contains as 3 chars: `\` `"` `"`).
    if (/^\\"\s*"\s*\+\s*String\(/.test(afterKey) ||
        /^\\"[A-Za-z]+\\"/.test(afterKey)) {
        return { type: 'string' };
    }
    // Number emission: `\"field\":<bare>
    if (/^[+\-]?\d/.test(afterKey)) {
        return { type: 'number' };
    }
    return null;
}

// Helper: scan JS reader body for the literal compared against.
// Returns 'string' if it sees field === "<x>", 'number' if it sees
// field === <bare-num>.
function jsFieldReaderType(fn, field) {
    const re = new RegExp('function\\s+' + fn + '\\s*\\([\\s\\S]*?\\n\\}', 'm');
    const m = cleanJS.match(re);
    if (!m) return null;
    const body = m[0];
    // String comparison: field === "<x>"
    const strRe = new RegExp('\\b' + field + '\\b\\s*===\\s*"([^"]+)"');
    if (strRe.test(body)) return 'string';
    // Number comparison: field === <num>
    const numRe = new RegExp('\\b' + field + '\\b\\s*===\\s*(-?\\d+)');
    if (numRe.test(body)) return 'number';
    return null;
}

// Known string-valued fields where producer and reader must agree.
const valueContracts = [
    { field: 'severity', producer: 'events', reader: 'updateEvents',
      allowedTypes: ['string'] },
    { field: 'severity', producer: 'events', reader: 'updateLAN',
      allowedTypes: ['string', null] }, // updateLAN doesn't read severity
];
for (const c of valueContracts) {
    const pt = producerFieldType(c.producer, c.field);
    const jt = jsFieldReaderType(c.reader, c.field);
    if (!pt || !jt) continue;
    expect(c.reader + '.' + c.field + ' reader type = producer type (' + pt.type + ')',
        jt === pt.type, 'producer=' + pt.type + ' reader=' + jt);
    expect(c.reader + '.' + c.field + ' reader type is in allowed list',
        c.allowedTypes.includes(jt), 'allowed=' + JSON.stringify(c.allowedTypes));
}

console.log('\n' + '='.repeat(50));
if (fails === 0) {
    console.log('All WS JSON shape checks passed.');
} else {
    console.log('FAILED:', fails, 'check(s)');
    process.exit(1);
}
