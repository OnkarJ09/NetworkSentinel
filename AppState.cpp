#include "AppState.h"

AppState appState;

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

    Serial.print("[EVENT] ");
    if (severity == EVENT_WARNING) Serial.print("WARN ");
    else if (severity == EVENT_CRITICAL) Serial.print("CRIT ");
    else Serial.print("INFO ");
    Serial.println(message);
}