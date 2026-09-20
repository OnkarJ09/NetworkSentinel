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

    float averageRSSI = 0;

    int busiestChannel = 0;

    int busiestChannelCount = 0;

    int congestion = 0;
};



#define TELEMETRY_HISTORY_SIZE 60

struct TelemetrySample {

    uint32_t timestamp;

    float ping;

    int rssi;

    uint8_t packetLoss;

    uint32_t dnsTime;

    uint32_t heap;
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
    // WIFI
    // --------------------------------------------------------

    int rssi = 0;

    int channel = 0;

    String ssid;

    String bssid;

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
    // ANALYZER
    // --------------------------------------------------------

    WiFiAnalyzerState analyzer;

    // --------------------------------------------------------
    // LAN SCANNER
    // --------------------------------------------------------

    LANScannerState lanScanner;
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

    uint32_t bootTime = 0;

    uint32_t lastNetworkUpdate = 0;

    bool networkTestRunning = false;

    SystemMetrics system;
};

extern AppState appState;
