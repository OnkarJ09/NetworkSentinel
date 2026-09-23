// Host-side test for the Config key=value parser (uses the stub String).
// Smallest thing that fails if the parse logic breaks.
#include "Config.h"
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
    return 0;
}
