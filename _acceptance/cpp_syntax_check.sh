#!/usr/bin/env bash
# C++ syntax-only check for every source file in NetworkSentinel.
# Uses minimal Arduino stubs to make g++ happy without the ESP32 toolchain.
set -e

cd "$(dirname "$0")/.."

CPP_FLAGS="-std=gnu++17 -fsyntax-only -Wno-narrowing -I_acceptance/stubs -I."

PASS=0
FAIL=0
FAIL_FILES=()

check() {
    local file="$1"
    local out
    out=$(g++ $CPP_FLAGS "$file" 2>&1) || true
    if [ -z "$out" ]; then
        echo "PASS  $file"
        PASS=$((PASS + 1))
    else
        echo "FAIL  $file"
        echo "$out" | head -8 | sed 's/^/    /'
        FAIL=$((FAIL + 1))
        FAIL_FILES+=("$file")
    fi
}

for f in AppState.cpp NetworkManager.cpp SentinelWeb.cpp OLED.cpp Joystick.cpp Menu.cpp; do
    check "$f"
done

# NetworkSentinel.ino is special — Arduino IDE compiles it as .cpp
cp NetworkSentinel.ino NetworkSentinel.tmp.cpp
check NetworkSentinel.tmp.cpp
rm -f NetworkSentinel.tmp.cpp

echo
echo "Summary: $PASS PASS, $FAIL FAIL"
if [ $FAIL -gt 0 ]; then
    echo "Failing files:"
    for f in "${FAIL_FILES[@]}"; do echo "  - $f"; done
    exit 1
fi
