#pragma once

#include <Arduino.h>
#include <WiFi.h>

class SentinelNetwork {

public:

    void begin();

    void startTask();

    void update();

    bool isConnected();

    // ========================================================
    // WIFI ANALYZER
    // ========================================================

    void scanWiFi();

    bool isScanning();

    void calculateWiFiStatistics();

    // ========================================================
    // LAN DISCOVERY
    // ========================================================

    void scanLAN();

    bool isLANScanning();

    void calculateLANStatistics();

    void loadLANDevices();

    void saveLANDevices();

private:

    void connectWiFi();

    void collectWiFiInfo();

    void testGateway();

    void testInternet();

    void testDNS();

    void runDiagnostics();

    String getEncryptionType(
        wifi_auth_mode_t type
    );

    static void taskEntry(
        void* parameter
    );

    void taskLoop();

    TaskHandle_t taskHandle =
        nullptr;
};

extern SentinelNetwork sentinelNetwork;