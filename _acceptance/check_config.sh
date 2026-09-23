#!/usr/bin/env bash
# Host-side check for the Config key=value parser. Compiles Config.cpp
# against the Arduino/LittleFS stubs and runs the assertions.
set -u

cd "$(dirname "$0")/.."

BIN=$(mktemp)

if ! g++ -std=gnu++17 -I_acceptance/stubs -I. \
        _acceptance/check_config.cpp Config.cpp -o "$BIN" 2>&1; then
    echo "config parser: COMPILE FAILED"
    rm -f "$BIN"
    exit 1
fi

if "$BIN"; then
    rm -f "$BIN"
    exit 0
fi

rm -f "$BIN"
exit 1
