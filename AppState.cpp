#include "AppState.h"

#include <LittleFS.h>

AppState appState;

static const char* EVENTS_PATH = "/events.txt";

// ============================================================
// EVENT LOG
// ============================================================
//
// Events are appended to /events.txt on flash. The file is trimmed to the
// last MAX_EVENTS lines on boot, so it cannot grow without bound.

void logEvent(
    uint8_t severity,
    const String& message
) {

    EventEntry& entry =
        appState.events[
            appState.eventHead
        ];

    entry.timestamp = millis();
    entry.severity = severity;
    entry.message = message;

    appState.eventHead =
        (appState.eventHead + 1) % MAX_EVENTS;

    if (appState.eventCount < MAX_EVENTS) {
        appState.eventCount++;
    }

    File file =
        LittleFS.open(EVENTS_PATH, "a");

    if (file) {

        file.print(entry.timestamp);
        file.print('|');
        file.print(entry.severity);
        file.print('|');
        file.println(entry.message);

        file.close();
    }

    Serial.print("[EVENT] ");
    if (severity == EVENT_WARNING) Serial.print("WARN ");
    else if (severity == EVENT_CRITICAL) Serial.print("CRIT ");
    else Serial.print("INFO ");
    Serial.println(message);
}

// ============================================================
// PARSE ONE LINE
// ============================================================

bool parseEventLine(
    const String& raw,
    EventEntry& out
) {

    String line = raw;
    line.trim();

    if (line.length() == 0) {
        return false;
    }

    int p1 = line.indexOf('|');
    if (p1 <= 0) {
        return false;
    }

    int next = line.substring(p1 + 1).indexOf('|');
    if (next < 0) {
        return false;
    }

    int p2 = p1 + 1 + next;

    out.timestamp =
        (uint32_t)line.substring(0, p1).toInt();
    out.severity =
        (uint8_t)line.substring(p1 + 1, p2).toInt();
    out.message = line.substring(p2 + 1);

    return true;
}

// ============================================================
// PARSE DEVICE LINE
// ============================================================

static IPAddress parseIP(const String& s) {

    int parts[4] = {0, 0, 0, 0};
    int idx = 0;
    int start = 0;
    int len = s.length();

    for (int i = 0; i <= len && idx < 4; i++) {

        if (i == len || s.charAt(i) == '.') {

            parts[idx++] =
                s.substring(start, i).toInt();

            start = i + 1;
        }
    }

    return IPAddress(
        parts[0], parts[1], parts[2], parts[3]
    );
}

bool parseDeviceLine(
    const String& raw,
    LANDevice& out
) {

    String line = raw;
    line.trim();

    int p1 = line.indexOf('|');
    if (p1 < 0) return false;

    int n2 = line.substring(p1 + 1).indexOf('|');
    if (n2 < 0) return false;
    int p2 = p1 + 1 + n2;

    int n3 = line.substring(p2 + 1).indexOf('|');
    if (n3 < 0) return false;
    int p3 = p2 + 1 + n3;

    out.ip = parseIP(line.substring(0, p1));
    out.hostname = line.substring(p1 + 1, p2);
    out.firstSeen =
        (uint32_t)line.substring(p2 + 1, p3).toInt();
    out.lastSeen =
        (uint32_t)line.substring(p3 + 1).toInt();

    return true;
}

// ============================================================
// BEGIN
// ============================================================

void eventsBegin() {

    if (!LittleFS.exists(EVENTS_PATH)) {
        return;
    }

    File file =
        LittleFS.open(EVENTS_PATH, "r");

    if (!file) {
        return;
    }

    // Sliding window of the last MAX_EVENTS entries
    static EventEntry tmp[MAX_EVENTS];
    int count = 0;

    while (file.available()) {

        EventEntry entry;

        if (!parseEventLine(
                file.readStringUntil('\n'),
                entry
            )) {
            continue;
        }

        if (count < MAX_EVENTS) {

            tmp[count++] = entry;

        } else {

            for (int i = 1; i < MAX_EVENTS; i++) {
                tmp[i - 1] = tmp[i];
            }

            tmp[MAX_EVENTS - 1] = entry;
        }
    }

    file.close();

    // Install oldest..newest into the ring
    appState.eventCount = count;
    appState.eventHead = 0;

    for (int i = 0; i < count; i++) {

        appState.events[
            appState.eventHead
        ] = tmp[i];

        appState.eventHead =
            (appState.eventHead + 1) % MAX_EVENTS;
    }

    // Trim the file to what we kept
    File out =
        LittleFS.open(EVENTS_PATH, "w");

    if (out) {

        for (int i = 0; i < count; i++) {

            EventEntry& e = appState.events[i];

            out.print(e.timestamp);
            out.print('|');
            out.print(e.severity);
            out.print('|');
            out.println(e.message);
        }

        out.close();
    }
}

// ============================================================
// RESET
// ============================================================

void eventsReset() {

    LittleFS.remove(EVENTS_PATH);

    appState.eventCount = 0;
    appState.eventHead = 0;
}
