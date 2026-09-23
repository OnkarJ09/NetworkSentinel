#!/usr/bin/env bash
# Host-side checks for pure logic in Config.cpp and AppState.cpp. Compiles
# them against the Arduino/LittleFS stubs and runs assert-based assertions.
set -u

cd "$(dirname "$0")/.."

BIN=$(mktemp)

if ! g++ -std=gnu++17 -I_acceptance/stubs -I. \
        _acceptance/check_host.cpp Config.cpp AppState.cpp -o "$BIN" 2>&1; then
    echo "host logic: COMPILE FAILED"
    rm -f "$BIN"
    exit 1
fi

if "$BIN"; then
    rm -f "$BIN"
    exit 0
fi

rm -f "$BIN"
exit 1
