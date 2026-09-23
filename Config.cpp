#include "Config.h"

#include <LittleFS.h>

SentinelConfig sentinelConfig;

static const char* CONFIG_PATH = "/config.txt";

// ============================================================
// BEGIN
// ============================================================

void configBegin() {

    LittleFS.begin(true);

    if (LittleFS.exists(CONFIG_PATH)) {

        File file =
            LittleFS.open(CONFIG_PATH, "r");

        if (file) {

            while (file.available()) {

                String line =
                    file.readStringUntil('\n');

                configApplyLine(line);
            }

            file.close();
        }
    }

    // Count this boot, then persist
    sentinelConfig.bootCount++;
    configSave();
}

// ============================================================
// SAVE
// ============================================================

void configSave() {

    File file =
        LittleFS.open(CONFIG_PATH, "w");

    if (!file) {
        return;
    }

    file.print("lanScanIntervalMs=");
    file.println(sentinelConfig.lanScanIntervalMs);

    file.print("highLatencyMs=");
    file.println(sentinelConfig.highLatencyMs);

    file.print("outageWindowMs=");
    file.println(sentinelConfig.outageWindowMs);

    file.print("outageLimit=");
    file.println(sentinelConfig.outageLimit);

    file.print("lanAutoScan=");
    file.println(
        sentinelConfig.lanAutoScan ? 1 : 0
    );

    file.print("bootCount=");
    file.println(sentinelConfig.bootCount);

    file.close();
}

// ============================================================
// RESET
// ============================================================

void configReset() {

    LittleFS.remove(CONFIG_PATH);

    // Restore defaults (bootCount intentionally reset too)
    sentinelConfig = SentinelConfig();
}

// ============================================================
// APPLY ONE LINE
// ============================================================

void configApplyLine(const String& raw) {

    String line = raw;
    line.trim();

    if (
        line.length() == 0 ||
        line.startsWith("#")
    ) {
        return;
    }

    int eq = line.indexOf('=');

    if (eq <= 0) {
        return;
    }

    String key = line.substring(0, eq);
    String value = line.substring(eq + 1);
    key.trim();
    value.trim();

    int n = value.toInt();

    if (key == "lanScanIntervalMs") {
        sentinelConfig.lanScanIntervalMs = (uint32_t)n;
    } else if (key == "highLatencyMs") {
        sentinelConfig.highLatencyMs = (uint32_t)n;
    } else if (key == "outageWindowMs") {
        sentinelConfig.outageWindowMs = (uint32_t)n;
    } else if (key == "outageLimit") {
        sentinelConfig.outageLimit = (uint8_t)n;
    } else if (key == "lanAutoScan") {
        sentinelConfig.lanAutoScan = (n != 0);
    } else if (key == "bootCount") {
        sentinelConfig.bootCount = (uint32_t)n;
    }
}
