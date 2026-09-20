#include "NetworkManager.h"

#include "config.h"
#include "AppState.h"

#include <WiFi.h>

SentinelNetwork sentinelNetwork;

// ============================================================
// TIMING
// ============================================================

static constexpr uint32_t NETWORK_INTERVAL = 10000;

// ============================================================
// BEGIN
// ============================================================

void SentinelNetwork::begin() {

    WiFi.mode(
        WIFI_STA
    );

    WiFi.setSleep(
        false
    );

    connectWiFi();

    if (
        WiFi.status() ==
        WL_CONNECTED
    ) {

        collectWiFiInfo();

        runDiagnostics();
    }
}

// ============================================================
// START TASK
// ============================================================

void SentinelNetwork::startTask() {

    if (
        taskHandle != nullptr
    ) {

        return;
    }

    xTaskCreatePinnedToCore(

        taskEntry,

        "SentinelNetwork",

        8192,

        this,

        1,

        &taskHandle,

        0
    );

    Serial.println(
        "Network task started"
    );
}

// ============================================================
// TASK ENTRY
// ============================================================

void SentinelNetwork::taskEntry(
    void* parameter
) {

    SentinelNetwork* self =
        static_cast<
            SentinelNetwork*
        >(parameter);

    self->taskLoop();

    vTaskDelete(
        nullptr
    );
}

// ============================================================
// TASK LOOP
// ============================================================

void SentinelNetwork::taskLoop() {

    uint32_t lastDiagnostics =
        0;

    uint32_t lastScanCheck =
        0;

    for (;;) {

        uint32_t now =
            millis();

        // ====================================================
        // NETWORK DIAGNOSTICS
        // ====================================================

        if (
            now -
            lastDiagnostics >=
            NETWORK_INTERVAL
        ) {

            lastDiagnostics =
                now;

            runDiagnostics();
        }

        // ====================================================
        // WIFI SCAN PROCESSOR
        // ====================================================

        if (
            appState.network.analyzer.scanning
        ) {

            if (
                now -
                lastScanCheck >=
                100
            ) {

                lastScanCheck =
                    now;

                calculateWiFiStatistics();
            }
        }

        // ====================================================
        // LAN DISCOVERY PROCESSOR
        // ====================================================

        if (
            appState.network.lanScanner.scanning
        ) {

            calculateLANStatistics();
        }

        // ====================================================
        // YIELD
        // ====================================================

        vTaskDelay(
            pdMS_TO_TICKS(20)
        );
    }
}

// ============================================================
// UPDATE
// ============================================================

void SentinelNetwork::update() {

    if (
        WiFi.status() !=
        WL_CONNECTED
    ) {

        connectWiFi();
    }

    if (
        WiFi.status() ==
        WL_CONNECTED
    ) {

        runDiagnostics();
    }
}

// ============================================================
// WIFI CONNECT
// ============================================================

void SentinelNetwork::connectWiFi() {

    if (
        WiFi.status() ==
        WL_CONNECTED
    ) {

        appState.network.wifiConnected =
            true;

        return;
    }

    appState.network.wifiConnected =
        false;

    Serial.println();

    Serial.println(
        "Connecting to WiFi..."
    );

    WiFi.disconnect();

    delay(100);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    uint32_t start =
        millis();

    while (
        WiFi.status() !=
        WL_CONNECTED &&
        millis() - start < 15000
    ) {

        vTaskDelay(
            pdMS_TO_TICKS(250)
        );

        Serial.print(".");
    }

    Serial.println();

    if (
        WiFi.status() ==
        WL_CONNECTED
    ) {

        appState.network.wifiConnected =
            true;

        Serial.println(
            "WiFi connected!"
        );

        Serial.print(
            "IP: "
        );

        Serial.println(
            WiFi.localIP()
        );

    } else {

        appState.network.wifiConnected =
            false;

        Serial.println(
            "WiFi connection failed"
        );
    }
}

// ============================================================
// WIFI INFORMATION
// ============================================================

void SentinelNetwork::collectWiFiInfo() {

    if (
        WiFi.status() !=
        WL_CONNECTED
    ) {

        appState.network.wifiConnected =
            false;

        return;
    }

    appState.network.wifiConnected =
        true;

    appState.network.ssid =
        WiFi.SSID();

    appState.network.bssid =
        WiFi.BSSIDstr();

    appState.network.rssi =
        WiFi.RSSI();

    appState.network.channel =
        WiFi.channel();

    appState.network.localIP =
        WiFi.localIP();

    appState.network.gatewayIP =
        WiFi.gatewayIP();

    appState.network.subnetMask =
        WiFi.subnetMask();

    appState.network.dnsIP =
        WiFi.dnsIP();
}

// ============================================================
// GATEWAY TEST
// ============================================================

void SentinelNetwork::testGateway() {

    if (
        !appState.network.wifiConnected
    ) {

        appState.network.gatewayOnline =
            false;

        appState.network.gatewayPing =
            -1;

        return;
    }

    uint32_t start =
        millis();

    WiFiClient client;

    bool connected =
        client.connect(
            appState.network.gatewayIP,
            80,
            1000
        );

    uint32_t elapsed =
        millis() - start;

    client.stop();

    if (connected) {

        appState.network.gatewayOnline =
            true;

        appState.network.gatewayPing =
            elapsed;

    } else {

        appState.network.gatewayOnline =
            false;

        appState.network.gatewayPing =
            -1;
    }
}

// ============================================================
// INTERNET TEST
// ============================================================

void SentinelNetwork::testInternet() {

    if (
        !appState.network.wifiConnected
    ) {

        appState.network.internetOnline =
            false;

        appState.network.internetPing =
            -1;

        appState.network.packetLoss =
            100;

        return;
    }

    const char* host =
        "8.8.8.8";

    const uint8_t attempts =
        5;

    uint8_t success =
        0;

    uint32_t total =
        0;

    for (
        uint8_t i = 0;
        i < attempts;
        i++
    ) {

        WiFiClient client;

        uint32_t start =
            millis();

        bool connected =
            client.connect(
                host,
                53,
                1000
            );

        uint32_t elapsed =
            millis() - start;

        client.stop();

        if (connected) {

            success++;

            total += elapsed;
        }

        vTaskDelay(
            pdMS_TO_TICKS(100)
        );
    }

    appState.network.packetLoss =
        (
            (attempts - success)
            * 100
        ) / attempts;

    if (
        success > 0
    ) {

        appState.network.internetOnline =
            true;

        appState.network.internetPing =
            (
                float(total) /
                success
            );

    } else {

        appState.network.internetOnline =
            false;

        appState.network.internetPing =
            -1;
    }
}

// ============================================================
// DNS TEST
// ============================================================

void SentinelNetwork::testDNS() {

    if (
        !appState.network.wifiConnected
    ) {

        appState.network.dnsOnline =
            false;

        appState.network.dnsTime =
            0;

        return;
    }

    IPAddress resolved;

    uint32_t start =
        millis();

    bool result =
        WiFi.hostByName(
            "example.com",
            resolved
        );

    uint32_t elapsed =
        millis() - start;

    if (result) {

        appState.network.dnsOnline =
            true;

        appState.network.dnsTime =
            elapsed;

    } else {

        appState.network.dnsOnline =
            false;

        appState.network.dnsTime =
            0;
    }
}

// ============================================================
// FULL DIAGNOSTICS
// ============================================================

void SentinelNetwork::runDiagnostics() {

    appState.networkTestRunning =
        true;

    if (
        WiFi.status() !=
        WL_CONNECTED
    ) {

        connectWiFi();
    }

    if (
        WiFi.status() !=
        WL_CONNECTED
    ) {

        appState.network.wifiConnected =
            false;

        appState.networkTestRunning =
            false;

        return;
    }

    collectWiFiInfo();

    testGateway();

    testInternet();

    testDNS();

    appState.networkTestRunning =
        false;

    appState.lastNetworkUpdate =
        millis();

    Serial.println();

    Serial.println(
        "========== SENTINEL =========="
    );

    Serial.print(
        "SSID: "
    );

    Serial.println(
        appState.network.ssid
    );

    Serial.print(
        "RSSI: "
    );

    Serial.print(
        appState.network.rssi
    );

    Serial.println(
        " dBm"
    );

    Serial.print(
        "Channel: "
    );

    Serial.println(
        appState.network.channel
    );

    Serial.print(
        "Gateway: "
    );

    Serial.println(
        appState.network.gatewayOnline
            ? "ONLINE"
            : "OFFLINE"
    );

    Serial.print(
        "Internet: "
    );

    Serial.println(
        appState.network.internetOnline
            ? "ONLINE"
            : "OFFLINE"
    );

    Serial.print(
        "DNS: "
    );

    Serial.println(
        appState.network.dnsOnline
            ? "ONLINE"
            : "OFFLINE"
    );

    Serial.print(
        "Ping: "
    );

    Serial.print(
        appState.network.internetPing
    );

    Serial.println(
        " ms"
    );

    Serial.print(
        "Loss: "
    );

    Serial.print(
        appState.network.packetLoss
    );

    Serial.println(
        "%"
    );

    Serial.println(
        "=============================="
    );
}

// ============================================================
// WIFI SECURITY
// ============================================================

String SentinelNetwork::getEncryptionType(
    wifi_auth_mode_t type
) {

    switch (type) {

        case WIFI_AUTH_OPEN:
            return "OPEN";

        case WIFI_AUTH_WEP:
            return "WEP";

        case WIFI_AUTH_WPA_PSK:
            return "WPA";

        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";

        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA/WPA2";

        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2-E";

        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";

        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2/3";

        default:
            return "OTHER";
    }
}

// ============================================================
// WIFI SCANNER
// ============================================================

void SentinelNetwork::scanWiFi() {

    if (
        appState.network.analyzer.scanning
    ) {

        Serial.println(
            "WiFi scan already running"
        );

        return;
    }

    if (
        WiFi.status() != WL_CONNECTED
    ) {

        Serial.println(
            "Cannot scan: WiFi disconnected"
        );

        return;
    }

    Serial.println();

    Serial.println(
        "========== WIFI ANALYZER =========="
    );

    appState.network.analyzer.scanning =
        true;

    appState.network.analyzer.scanComplete =
        false;

    // --------------------------------------------------------
    // Clear old statistics
    // --------------------------------------------------------

    appState.network.analyzer.networkCount =
        0;

    appState.network.analyzer.strongestIndex =
        -1;

    appState.network.analyzer.weakestIndex =
        -1;

    appState.network.analyzer.openNetworks =
        0;

    appState.network.analyzer.averageRSSI =
        0;

    appState.network.analyzer.busiestChannel =
        0;

    appState.network.analyzer.busiestChannelCount =
        0;

    appState.network.analyzer.congestion =
        0;

    memset(
        appState.network.analyzer.channelCount,
        0,
        sizeof(
            appState.network.analyzer.channelCount
        )
    );

    // --------------------------------------------------------
    // Start asynchronous scan
    // --------------------------------------------------------

    int result =
        WiFi.scanNetworks(
            true,
            true
        );

    if (
        result == WIFI_SCAN_FAILED
    ) {

        Serial.println(
            "Failed to start WiFi scan"
        );

        appState.network.analyzer.scanning =
            false;

        return;
    }

    Serial.println(
        "WiFi scan started..."
    );
}


void SentinelNetwork::calculateWiFiStatistics() {

    auto& analyzer =
        appState.network.analyzer;

    int count =
        WiFi.scanComplete();

    // --------------------------------------------------------
    // Still scanning
    // --------------------------------------------------------

    if (
        count == WIFI_SCAN_RUNNING
    ) {

        return;
    }

    // --------------------------------------------------------
    // Scan failed
    // --------------------------------------------------------

    if (
        count == WIFI_SCAN_FAILED
    ) {

        Serial.println(
            "WiFi scan failed"
        );

        analyzer.scanning =
            false;

        analyzer.scanComplete =
            false;

        return;
    }

    // --------------------------------------------------------
    // No results
    // --------------------------------------------------------

    if (
        count <= 0
    ) {

        analyzer.networkCount =
            0;

        analyzer.scanning =
            false;

        analyzer.scanComplete =
            true;

        WiFi.scanDelete();

        return;
    }

    // --------------------------------------------------------
    // Limit stored networks
    // --------------------------------------------------------

    int stored =
        min(
            count,
            MAX_WIFI_NETWORKS
        );

    analyzer.networkCount =
        stored;

    long totalRSSI =
        0;

    // --------------------------------------------------------
    // Read networks
    // --------------------------------------------------------

    for (
        int i = 0;
        i < stored;
        i++
    ) {

        WiFiNetworkInfo& network =
            analyzer.networks[i];

        network.ssid =
            WiFi.SSID(i);

        network.bssid =
            WiFi.BSSIDstr(i);

        network.rssi =
            WiFi.RSSI(i);

        network.channel =
            WiFi.channel(i);

        network.security =
            getEncryptionType(
                WiFi.encryptionType(i)
            );

        // ----------------------------------------------------
        // RSSI
        // ----------------------------------------------------

        totalRSSI +=
            network.rssi;

        // ----------------------------------------------------
        // Strongest
        // ----------------------------------------------------

        if (
            analyzer.strongestIndex ==
            -1 ||
            network.rssi >
            analyzer.networks[
                analyzer.strongestIndex
            ].rssi
        ) {

            analyzer.strongestIndex =
                i;
        }

        // ----------------------------------------------------
        // Weakest
        // ----------------------------------------------------

        if (
            analyzer.weakestIndex ==
            -1 ||
            network.rssi <
            analyzer.networks[
                analyzer.weakestIndex
            ].rssi
        ) {

            analyzer.weakestIndex =
                i;
        }

        // ----------------------------------------------------
        // Open networks
        // ----------------------------------------------------

        if (
            network.security ==
            "OPEN"
        ) {

            analyzer.openNetworks++;
        }

        // ----------------------------------------------------
        // Channel statistics
        // ----------------------------------------------------

        if (
            network.channel >= 1 &&
            network.channel <=
                MAX_WIFI_CHANNELS
        ) {

            analyzer.channelCount[
                network.channel
            ]++;
        }
    }

    // --------------------------------------------------------
    // Average RSSI
    // --------------------------------------------------------

    if (
        stored > 0
    ) {

        analyzer.averageRSSI =
            float(totalRSSI) /
            stored;
    }

    // --------------------------------------------------------
    // Busiest channel
    // --------------------------------------------------------

    for (
        int channel = 1;
        channel <= MAX_WIFI_CHANNELS;
        channel++
    ) {

        int countOnChannel =
            analyzer.channelCount[
                channel
            ];

        if (
            countOnChannel >
            analyzer.busiestChannelCount
        ) {

            analyzer.busiestChannel =
                channel;

            analyzer.busiestChannelCount =
                countOnChannel;
        }
    }

    // --------------------------------------------------------
    // Simple congestion estimate
    // --------------------------------------------------------

    /*
       This is intentionally a simple estimate.

       We are NOT measuring actual RF utilization.
       We're measuring AP density.

       More APs on overlapping channels
       generally means more contention.
    */

    if (
        analyzer.networkCount == 0
    ) {

        analyzer.congestion =
            0;

    } else {

        analyzer.congestion =
            min(
                100,
                analyzer.busiestChannelCount *
                10
            );
    }

    // --------------------------------------------------------
    // Complete
    // --------------------------------------------------------

    analyzer.lastScan =
        millis();

    analyzer.scanning =
        false;

    analyzer.scanComplete =
        true;

    WiFi.scanDelete();

    // --------------------------------------------------------
    // SERIAL REPORT
    // --------------------------------------------------------

    Serial.println();

    Serial.println(
        "========== WIFI RESULTS =========="
    );

    Serial.print(
        "Networks: "
    );

    Serial.println(
        analyzer.networkCount
    );

    Serial.print(
        "Average RSSI: "
    );

    Serial.print(
        analyzer.averageRSSI
    );

    Serial.println(
        " dBm"
    );

    Serial.print(
        "Open networks: "
    );

    Serial.println(
        analyzer.openNetworks
    );

    Serial.print(
        "Busiest channel: "
    );

    Serial.print(
        analyzer.busiestChannel
    );

    Serial.print(
        " ("
    );

    Serial.print(
        analyzer.busiestChannelCount
    );

    Serial.println(
        " APs)"
    );

    Serial.print(
        "Congestion: "
    );

    Serial.print(
        analyzer.congestion
    );

    Serial.println(
        "%"
    );

    Serial.println();

    for (
        int i = 0;
        i < analyzer.networkCount;
        i++
    ) {

        Serial.print(
            i + 1
        );

        Serial.print(
            ". "
        );

        Serial.print(
            analyzer.networks[i].ssid
        );

        Serial.print(
            " | "
        );

        Serial.print(
            analyzer.networks[i].rssi
        );

        Serial.print(
            " dBm | CH "
        );

        Serial.print(
            analyzer.networks[i].channel
        );

        Serial.print(
            " | "
        );

        Serial.println(
            analyzer.networks[i].security
        );
    }

    Serial.println(
        "==================================="
    );
}

// ============================================================
// SCANNING STATE
// ============================================================

bool SentinelNetwork::isScanning() {

    return appState.network.analyzer.scanning;
}

// ============================================================
// CONNECTION
// ============================================================

bool SentinelNetwork::isConnected() {

    return (
        WiFi.status() ==
        WL_CONNECTED
    );
}

// ============================================================
// LAN DISCOVERY
// ============================================================

void SentinelNetwork::scanLAN() {

    if (
        appState.network.lanScanner.scanning
    ) {

        Serial.println(
            "LAN scan already running"
        );

        return;
    }

    if (
        WiFi.status() != WL_CONNECTED
    ) {

        Serial.println(
            "Cannot scan LAN: WiFi disconnected"
        );

        return;
    }

    Serial.println();

    Serial.println(
        "========== LAN DISCOVERY =========="
    );

    // --------------------------------------------------------
    // Calculate subnet range
    // --------------------------------------------------------

    IPAddress localIP = WiFi.localIP();
    IPAddress mask = WiFi.subnetMask();

    // Network address = IP AND mask
    appState.network.lanScanner.subnetStart =
        IPAddress(
            localIP[0] & mask[0],
            localIP[1] & mask[1],
            localIP[2] & mask[2],
            1
        );

    // Broadcast address = IP OR ~mask
    appState.network.lanScanner.subnetEnd =
        IPAddress(
            localIP[0] | ~mask[0],
            localIP[1] | ~mask[1],
            localIP[2] | ~mask[2],
            254
        );

    appState.network.lanScanner.totalHosts =
        (uint32_t)appState.network.lanScanner.subnetEnd[3] -
        (uint32_t)appState.network.lanScanner.subnetStart[3] + 1;

    // Clear old data
    appState.network.lanScanner.deviceCount = 0;
    appState.network.lanScanner.scannedHosts = 0;

    for (
        int i = 0;
        i < MAX_LAN_DEVICES;
        i++
    ) {

        appState.network.lanScanner.devices[i].online =
            false;

        appState.network.lanScanner.devices[i].latency =
            -1;

        appState.network.lanScanner.devices[i].hostname =
            "";

        appState.network.lanScanner.devices[i].mac =
            "";

        appState.network.lanScanner.devices[i].lastSeen =
            0;

        appState.network.lanScanner.devices[i].firstSeen =
            0;
    }

    LANDevice& self =
        appState.network.lanScanner.devices[
            appState.network.lanScanner.deviceCount++
        ];

    self.ip = localIP;
    self.hostname = "This device";
    self.mac = WiFi.macAddress();
    self.online = true;
    self.latency = 0;
    self.lastSeen = millis();

    if (self.firstSeen == 0) {

        self.firstSeen = self.lastSeen;
    }

    appState.network.lanScanner.scanning = true;
    appState.network.lanScanner.scanComplete = false;

    Serial.print(
        "Scanning subnet: "
    );

    Serial.print(
        appState.network.lanScanner.subnetStart.toString()
    );

    Serial.print(
        " - "
    );

    Serial.println(
        appState.network.lanScanner.subnetEnd.toString()
    );

    Serial.print(
        "Total hosts to scan: "
    );

    Serial.println(
        appState.network.lanScanner.totalHosts
    );
}

bool SentinelNetwork::isLANScanning() {

    return appState.network.lanScanner.scanning;
}

void SentinelNetwork::calculateLANStatistics() {

    auto& scanner =
        appState.network.lanScanner;

    // --------------------------------------------------------
    // Scan complete?
    // --------------------------------------------------------

    if (
        scanner.scannedHosts >=
        scanner.totalHosts
    ) {

        scanner.scanning = false;
        scanner.scanComplete = true;
        scanner.lastScan = millis();

        Serial.println();
        Serial.println(
            "========== LAN RESULTS =========="
        );

        Serial.print(
            "Devices found: "
        );

        Serial.println(
            scanner.deviceCount
        );

        for (
            int i = 0;
            i < scanner.deviceCount;
            i++
        ) {

            LANDevice& device =
                scanner.devices[i];

            Serial.print(
                device.ip.toString()
            );

            Serial.print(" | ");

            Serial.print(
                device.hostname.length() > 0
                    ? device.hostname
                    : "unknown"
            );

            Serial.print(" | ");

            Serial.print(
                device.latency >= 0
                    ? String((int)device.latency) + "ms"
                    : "--"
            );

            Serial.println();
        }

        Serial.println(
            "=================================="
        );

        return;
    }

    // --------------------------------------------------------
    // Build IP to ping next
    // --------------------------------------------------------

    IPAddress base =
        scanner.subnetStart;

    IPAddress target(
        base[0],
        base[1],
        base[2],
        base[3] + scanner.scannedHosts
    );

    scanner.scannedHosts++;

    // Skip our own IP
    if (
        target == WiFi.localIP()
    ) {

        return;
    }

    // --------------------------------------------------------
    // Probe target with TCP connect
    // --------------------------------------------------------

    WiFiClient client;

    uint32_t start = millis();

    bool reached =
        client.connect(
            target,
            80,
            300
        );

    uint32_t elapsed = millis() - start;

    client.stop();

    // If port 80 failed, try port 443
    if (!reached) {

        start = millis();

        reached =
            client.connect(
                target,
                443,
                300
            );

        elapsed = millis() - start;

        client.stop();
    }

    // --------------------------------------------------------
    // Gateway is always "online" if we pass through it
    // --------------------------------------------------------

    bool isGateway =
        (target == WiFi.gatewayIP());

    if (!reached && isGateway) {

        reached =
            appState.network.gatewayOnline;

        elapsed =
            (uint32_t)appState.network.gatewayPing;
    }

    if (!reached) {

        return;
    }

    // --------------------------------------------------------
    // Store device
    // --------------------------------------------------------

    if (
        scanner.deviceCount >=
        MAX_LAN_DEVICES
    ) {

        return;
    }

    LANDevice& device =
        scanner.devices[
            scanner.deviceCount++
        ];

    device.ip = target;
    device.online = true;
    device.latency = (float)elapsed;
    device.lastSeen = millis();

    if (device.firstSeen == 0) {
        device.firstSeen = millis();
    }

    // --------------------------------------------------------
    // Hostname lookup
    // --------------------------------------------------------

    String name = "";

    if (isGateway) {

        name = "Gateway";

    } else if (target == WiFi.localIP()) {

        name = "This device";
    }

    device.hostname = name;

    Serial.print("Found: ");
    Serial.print(target.toString());

    if (name.length() > 0) {

        Serial.print(" (");
        Serial.print(name);
        Serial.print(")");
    }

    Serial.print(" ");
    Serial.print(elapsed);
    Serial.println("ms");
}
