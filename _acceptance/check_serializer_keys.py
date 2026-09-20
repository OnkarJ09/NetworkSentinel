#!/usr/bin/env python3
"""Structural lint: verify that every JSON key documented as 'in serializer X'
actually appears in that serializer's source code."""
import re, sys

src = open('../SentinelWeb.cpp').read()

# Find function boundaries
def find_func(name, src):
    pat = re.compile(r'String SentinelWeb::' + re.escape(name) + r'\(\)\s*\{')
    m = pat.search(src)
    if not m: return None
    # walk braces to find matching close
    i = src.find('{', m.start())
    depth = 0
    while i < len(src):
        if src[i] == '{': depth += 1
        elif src[i] == '}':
            depth -= 1
            if depth == 0:
                return src[m.start():i+1]
        i += 1
    return None

def keys_in(body):
    # Match C++ escaped-quote + word + C++ escaped-quote, then colon.
    # In C++ source, JSON keys appear as \"key\": — i.e. backslash, quote,
    # word, backslash, quote, colon.
    return set(re.findall(r'\\"([a-zA-Z_][a-zA-Z0-9_]*)\\"\s*:', body))

def check(name, expected, src):
    body = find_func(name, src)
    if body is None:
        print(f'FAIL  {name}: function not found')
        return False
    actual = keys_in(body)
    missing = expected - actual
    if missing:
        print(f'FAIL  {name}: missing keys {sorted(missing)}')
        return False
    print(f'PASS  {name}: {len(actual)} keys, all {len(expected)} expected present')
    return True

# Documented keys per the plan
ok = True
ok &= check('createJSON', {
    'ssid','ip','gateway','dns','channel','rssi','internetPing','gatewayPing',
    'packetLoss','wifi','gatewayOnline','internetOnline','dnsOnline',
    'heap','cpu','uptime',
    # Phase 2-6 additions
    'wifiMAC','wifiUptimeSec','wifiReconnects','stabilityScore','healthScore',
    'pingSpikes','lossSpikes','rssiDrops',
}, src)

ok &= check('createTelemetryJSON', {
    'wifi','gateway','internet','dns','ping','packetLoss','rssi','channel',
    'dnsTime','heap','cpu','uptime','lowMemory','cpuBlocked','highLatency',
    # additions
    'healthScore','stabilityScore',
}, src)

ok &= check('createWiFiEventJSON', {
    'ssid','rssi','channel','encryption','hidden','strongestRSSI','weakestRSSI',
}, src)

ok &= check('createHistoryJSON', {
    'samples',
    'state',
}, src)

ok &= check('createEventsJSON', {
    'type','events','title','message','source','severity','timestamp',
}, src)

ok &= check('createLANJSON', {
    'type','devices','ip','mac','hostname','lastSeen','firstSeen',
}, src)

if not ok:
    sys.exit(1)
print('\nAll serializer key contracts satisfied.')
