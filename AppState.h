#pragma once

#include <Arduino.h>
#include <WiFi.h>

#define MAX_WIFI_NETWORKS 30
#define MAX_WIFI_CHANNELS 14
#define MAX_LAN_DEVICES 32

// ============================================================
// WIFI NETWORK
// ============================================================

struct WiFiNetworkInfo {

    String ssid;

    String bssid;

    int32_t rssi = 0;

    int32_t channel = 0;

    String security;
};

// ============================================================
// LAN DEVICE
// ============================================================

struct LANDevice {

    IPAddress ip;

    String hostname;

    String mac;

    bool online = false;

    float latency = -1;

    uint32_t lastSeen = 0;

    uint32_t firstSeen = 0;

    // Set true when the device responds during the current scan
    bool seenThisScan = false;
};

// ============================================================
// LAN SCANNER
// ============================================================

struct LANScannerState {

    LANDevice devices[MAX_LAN_DEVICES];

    int deviceCount = 0;

    bool scanning = false;

    // Indicates whether an automatic periodic scan is scheduled/running
    bool autoScanning = false;

    bool scanComplete = false;

    uint32_t lastScan = 0;

    IPAddress subnetStart;

    IPAddress subnetEnd;

    int totalHosts = 0;

    int scannedHosts = 0;

    // Percentage of known devices currently online (0-100)
    uint8_t healthScore = 0;
};

// ============================================================
// WIFI ANALYZER
// ============================================================

struct WiFiAnalyzerState {

    WiFiNetworkInfo networks[
        MAX_WIFI_NETWORKS
    ];

    int networkCount = 0;

    bool scanning = false;

    bool scanComplete = false;

    uint32_t lastScan = 0;

    // --------------------------------------------------------
    // Channel statistics
    // --------------------------------------------------------

    uint8_t channelCount[
        MAX_WIFI_CHANNELS + 1
    ] = {};

    // --------------------------------------------------------
    // Statistics
    // --------------------------------------------------------

    int strongestIndex = -1;

    int weakestIndex = -1;

    int openNetworks = 0;

    int hiddenNetworks = 0;

    float averageRSSI = 0;

    int busiestChannel = 0;

    int busiestChannelCount = 0;

    int congestion = 0;

    // Least congested of channels 1/6/11
    int recommendedChannel = 0;
};



#define TELEMETRY_HISTORY_SIZE 60

struct TelemetrySample {

    uint32_t timestamp;

    float ping;

    int rssi;

    uint8_t packetLoss;

    uint32_t dnsTime;

    uint32_t heap;

    uint8_t stateFlags = 0;
};

// ============================================================
// SECURITY BASELINE (Phase 10)
// ============================================================

#define MAX_KNOWN_APS 32

struct KnownAP {

    String bssid;

    String ssid;

    String security;

    bool seen = false;
};

struct SecurityState {

    KnownAP known[MAX_KNOWN_APS];

    int knownCount = 0;

    int unknownAPs = 0;

    int disappeared = 0;

    int rogueAPs = 0;

    int openCount = 0;

    // Event edge-trigger memory (avoid spamming every scan)
    int prevOpen = 0;

    int prevRogue = 0;

    String posture = "GOOD";

    uint32_t lastAnalysis = 0;

    // Persisted to /known_aps.txt so the baseline survives reboots.
};

// ============================================================
// NETWORK STATE
// ============================================================

struct NetworkState {

    // --------------------------------------------------------
    // CONNECTION
    // --------------------------------------------------------

    bool wifiConnected = false;

    bool gatewayOnline = false;

    bool internetOnline = false;

    bool dnsOnline = false;

    // --------------------------------------------------------
    // CONNECTION UPTIME
    // --------------------------------------------------------

    uint32_t wifiConnectedSince = 0;

    uint32_t wifiReconnectCount = 0;

    // --------------------------------------------------------
    // WIFI
    // --------------------------------------------------------

    int rssi = 0;

    int channel = 0;

    String ssid;

    String bssid;

    String wifiMAC;

    // --------------------------------------------------------
    // IP
    // --------------------------------------------------------

    IPAddress localIP;

    IPAddress gatewayIP;

    IPAddress subnetMask;

    IPAddress dnsIP;

    // --------------------------------------------------------
    // DIAGNOSTICS
    // --------------------------------------------------------

    float gatewayPing = -1;

    float internetPing = -1;

    uint8_t packetLoss = 100;

    uint32_t dnsTime = 0;

    // --------------------------------------------------------
    // OUTAGE TRACKING
    // --------------------------------------------------------

    uint32_t lastInternetDown = 0;

    uint32_t lastGatewayDown = 0;

    uint32_t lastDnsDown = 0;

    uint32_t lastInternetUp = 0;

    uint32_t lastGatewayUp = 0;

    uint32_t lastDnsUp = 0;

    bool internetWasOnline = false;

    bool gatewayWasOnline = false;

    bool dnsWasOnline = false;

    bool wifiWasConnected = false;

    // --------------------------------------------------------
    // SPIKE / STABILITY
    // --------------------------------------------------------

    uint16_t pingSpikes = 0;

    uint16_t lossSpikes = 0;

    uint16_t rssiDrops = 0;

    uint8_t stabilityScore = 100;

    uint8_t healthScore = 100;

    bool highLatencyActive = false;

    // Intermittent-connectivity window (Phase 9)
    uint8_t outageCount = 0;

    uint32_t outageWindowStart = 0;

    bool intermittentReported = false;

    // --------------------------------------------------------
    // ANALYZER
    // --------------------------------------------------------

    WiFiAnalyzerState analyzer;

    // --------------------------------------------------------
    // SECURITY
    // --------------------------------------------------------

    SecurityState security;

    // --------------------------------------------------------
    // LAN SCANNER
    // --------------------------------------------------------

    LANScannerState lanScanner;
};

// ============================================================
// EVENT LOG
// ============================================================

#define MAX_EVENTS 32

enum EventSeverity : uint8_t {
    EVENT_INFO = 0,
    EVENT_WARNING = 1,
    EVENT_CRITICAL = 2
};

struct EventEntry {
    uint32_t timestamp = 0;
    uint8_t severity = EVENT_INFO;
    String message;
};

// ============================================================
// SYSTEM METRICS & ALERTS
// ============================================================

struct SystemAlerts {
    bool lowMemory = false;
    bool cpuBlocked = false;
    bool highLatency = false;
};

struct SystemMetrics {
    uint32_t loopTimeMs = 0;
    uint32_t maxLoopTimeMs = 0;
    uint32_t minFreeHeap = 0;
    int8_t wifiTxPower = 0;
    SystemAlerts alerts;
};

// ============================================================
// APPLICATION STATE
// ============================================================

struct AppState {

    NetworkState network;

    TelemetrySample telemetryHistory[
    TELEMETRY_HISTORY_SIZE
    ];

    uint8_t telemetryHistoryIndex = 0;

    uint8_t telemetryHistoryCount = 0;

    // Last sample snapshot for spike detection
    float lastPing = -1;
    int lastRssi = 0;
    uint8_t lastLoss = 0;

    // Event log
    EventEntry events[MAX_EVENTS];
    uint8_t eventCount = 0;
    uint8_t eventHead = 0;

    uint32_t bootTime = 0;

    uint32_t lastNetworkUpdate = 0;

    bool networkTestRunning = false;

    SystemMetrics system;
};

// Shared event-log writer, callable from any module (NetworkManager, SentinelWeb)
void logEvent(uint8_t severity, const String& message);

// Load persisted events from /events.txt (last MAX_EVENTS) and trim the file.
void eventsBegin();

// Clear the event log and delete /events.txt (factory reset).
void eventsReset();

// Parse one "timestamp|severity|message" line. Used by the loader and host tests.
bool parseEventLine(const String& line, EventEntry& out);

extern AppState appState;
