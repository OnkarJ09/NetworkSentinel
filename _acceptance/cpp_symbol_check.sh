#!/usr/bin/env bash
# Symbol-resolution check: every method declared inside a project class
# header must have a matching `ClassName::method(...)` definition in some
# .cpp or .ino file. Catches linker errors caused by declared-but-not-
# defined methods, which `-fsyntax-only` and stub-based parsing miss.
#
# The project style declares methods inside `class Name { ... }` without
# repeating the class prefix on each line. We extract every method name
# from the header, then look for `Name::name(` definitions in the .cpp.
#
# Usage: bash cpp_symbol_check.sh
# Exits 0 if every declaration has a definition; non-zero otherwise.
set -u

cd "$(dirname "$0")/.."

PASS=0
FAIL=0
FAIL_LINES=()

# Returns 0 if `ClassName::method` followed by `(` appears as a definition
# in any .cpp or .ino file under the project root.
has_definition() {
    local class="$1"
    local method="$2"
    local cpp_files
    cpp_files=$(find . -maxdepth 2 \( -name '*.cpp' -o -name '*.ino' \) ! -path './_acceptance/*')
    for f in $cpp_files; do
        if grep -qE "^[[:space:]]*[^/]*${class}::${method}[[:space:]]*\\(" "$f"; then
            return 0
        fi
    done
    return 1
}

# Extracts every method name declared inside `class Name { ... }` in the
# given header. Project style wraps arguments across lines, so we join
# continuation lines first. We then look for tokens of the form
# `name(` that look like method declarations (return type before,
# parenthesis after). We exclude constructors, destructors, access
# keywords, and `operator*` overloads.
declared_methods() {
    local header="$1"
    awk '
        {
            line = $0
            # Continuation: line ends with `,` (possibly with whitespace)
            if (match(line, /,[ \t]*$/)) {
                sub(/,[ \t]*$/, "", line)
                buf = buf line ", "
                next
            }
            # Continuation: line ends with `(` (method opens)
            if (match(line, /[(][ \t]*$/)) {
                sub(/[(][ \t]*$/, "( ", line)
                buf = buf line
                next
            }
            print buf line
            buf = ""
        }
        END { if (buf != "") print buf }
    ' "$header" | grep -oE '[A-Za-z_][A-Za-z0-9_]*[ \t]*\(' | \
        grep -oE '[A-Za-z_][A-Za-z0-9_]*' | sort -u
}

check_class() {
    local class="$1"
    local header="$2"

    if [ ! -f "$header" ]; then
        echo "FAIL  $header (missing)"
        FAIL=$((FAIL + 1))
        return
    fi

    local decls
    decls=$(declared_methods "$header")
    if [ -z "$decls" ]; then
        echo "PASS  $class (no methods declared)"
        PASS=$((PASS + 1))
        return
    fi

    local missing=0
    local total=0
    while IFS= read -r method; do
        [ -z "$method" ] && continue
        # Skip keywords / operators / false positives
        case "$method" in
            if|else|for|while|return|class|public|private|protected|const|volatile|static|extern|inline|virtual|friend|using|namespace|template|typename|operator|switch|case|default|break|continue|goto|do|sizeof|new|delete|this|nullptr|true|false) continue ;;
        esac
        total=$((total + 1))
        if has_definition "$class" "$method"; then
            :
        else
            echo "FAIL  $class::$method declared in $header but no definition in any .cpp/.ino"
            FAIL_LINES+=("$class::$method declared in $header")
            missing=$((missing + 1))
        fi
    done <<< "$decls"

    if [ "$missing" -eq 0 ]; then
        echo "PASS  $class ($total methods all defined)"
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
    fi
}

check_class OLED          OLED.h
check_class Buttons       Buttons.h
check_class Menu          Menu.h
check_class AppState      AppState.h
check_class SentinelNetwork NetworkManager.h
check_class SentinelWeb   SentinelWeb.h

echo
echo "Summary: $PASS PASS, $FAIL FAIL"
if [ $FAIL -gt 0 ]; then
    echo "Missing definitions:"
    for line in "${FAIL_LINES[@]}"; do echo "  - $line"; done
    exit 1
fi
