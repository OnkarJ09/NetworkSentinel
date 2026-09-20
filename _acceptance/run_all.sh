#!/usr/bin/env bash
# Top-level acceptance runner. Exits 0 only if every check passes.
set -u

cd "$(dirname "$0")"

echo "=========================================="
echo " NetworkSentinel acceptance suite"
echo "=========================================="
echo

FAIL=0

echo "[1/2] C++ syntax check (g++ -fsyntax-only)"
echo "------------------------------------------"
if bash ./cpp_syntax_check.sh; then
    echo ">>> C++ syntax: PASS"
else
    echo ">>> C++ syntax: FAIL"
    FAIL=1
fi
echo

echo "[2/4] C++ symbol-resolution check (declared vs defined methods)"
echo "------------------------------------------"
if bash ./cpp_symbol_check.sh; then
    echo ">>> C++ symbols: PASS"
else
    echo ">>> C++ symbols: FAIL"
    FAIL=1
fi
echo

echo "[3/4] JavaScript harness (Node)"
echo "------------------------------------------"
if node run_js.js; then
    echo ">>> JS harness: PASS"
else
    echo ">>> JS harness: FAIL"
    FAIL=1
fi
echo

echo "[4/4] WS JSON shape cross-check (Node)"
echo "------------------------------------------"
if node check_ws_shape.js; then
    echo ">>> WS shape: PASS"
else
    echo ">>> WS shape: FAIL"
    FAIL=1
fi
echo

echo "=========================================="
if [ $FAIL -eq 0 ]; then
    echo " RESULT: ALL ACCEPTANCE CHECKS PASSED"
else
    echo " RESULT: FAILURES PRESENT"
fi
echo "=========================================="
exit $FAIL
