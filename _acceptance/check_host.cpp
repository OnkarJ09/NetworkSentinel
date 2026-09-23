// Host-side test for the Config key=value parser (uses the stub String).
// Smallest thing that fails if the parse logic breaks.
#include "Config.h"
#include "AppState.h"
#include <cassert>
#include <cstdio>

int main() {

    sentinelConfig.highLatencyMs = 1;
    sentinelConfig.lanScanIntervalMs = 0;
    sentinelConfig.outageLimit = 0;
    sentinelConfig.lanAutoScan = true;

    configApplyLine("highLatencyMs=250");
    assert(sentinelConfig.highLatencyMs == 250);

    configApplyLine("lanScanIntervalMs=120000");
    assert(sentinelConfig.lanScanIntervalMs == 120000);

    configApplyLine("outageLimit=5");
    assert(sentinelConfig.outageLimit == 5);

    configApplyLine("lanAutoScan=0");
    assert(sentinelConfig.lanAutoScan == false);

    configApplyLine("lanAutoScan=1");
    assert(sentinelConfig.lanAutoScan == true);

    configApplyLine("bootCount=7");
    assert(sentinelConfig.bootCount == 7);

    // Whitespace tolerance
    configApplyLine("  highLatencyMs = 99  ");
    assert(sentinelConfig.highLatencyMs == 99);

    // Malformed lines must not change state
    uint32_t before = sentinelConfig.highLatencyMs;
    configApplyLine("");
    configApplyLine("   ");
    configApplyLine("# comment");
    configApplyLine("garbage-with-no-equals");
    configApplyLine("=novalue");
    configApplyLine("unknownKey=9");
    assert(sentinelConfig.highLatencyMs == before);

    // Factory reset restores defaults
    configReset();
    assert(sentinelConfig.highLatencyMs == 150);
    assert(sentinelConfig.lanScanIntervalMs == 60000);
    assert(sentinelConfig.lanAutoScan == true);
    assert(sentinelConfig.bootCount == 0);

    printf("config parser OK\n");

    // ------------------------------------------------------
    // Event log line parser
    // ------------------------------------------------------
    EventEntry e;

    assert(parseEventLine("1234|2|Internet unavailable", e));
    assert(e.timestamp == 1234);
    assert(e.severity == 2);
    assert(e.message == "Internet unavailable");

    // Message may itself contain the separator
    assert(parseEventLine("9|0|Rogue AP: a|b", e));
    assert(e.timestamp == 9);
    assert(e.severity == 0);
    assert(e.message == "Rogue AP: a|b");

    // Malformed lines rejected
    assert(!parseEventLine("", e));
    assert(!parseEventLine("   ", e));
    assert(!parseEventLine("no-separator", e));
    assert(!parseEventLine("1|only-two", e));

    printf("event parser OK\n");
    return 0;
}
