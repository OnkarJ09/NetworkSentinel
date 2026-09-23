#include "config.h"

#include "AppState.h"

#include "Buttons.h"

#include "OLED.h"
#include "Menu.h"

#include "NetworkManager.h"
#include "SentinelWeb.h"

// ============================================================
// OBJECTS
// ============================================================

Buttons buttons;

int selectedAP = 0;

int selectedLANDevice = 0;

// ============================================================
// SCREEN DRAWING
// ============================================================

void drawDashboard() {

    oled.clear();

    oled.header(
        "DASHBOARD"
    );

    auto& n =
        appState.network;

    // --------------------------------------------------------
    // CONNECTION
    // --------------------------------------------------------

    oled.text(
        0,
        13,
        n.internetOnline
            ? "NET: ONLINE"
            : "NET: OFFLINE"
    );

    // --------------------------------------------------------
    // PING
    // --------------------------------------------------------

    oled.text(
        0,
        25,
        "PING:"
    );

    if (
        n.internetPing >= 0
    ) {

        oled.text(
            42,
            25,
            String(
                (int)n.internetPing
            ) + "ms"
        );

    } else {

        oled.text(
            42,
            25,
            "---"
        );
    }

    // --------------------------------------------------------
    // LOSS
    // --------------------------------------------------------

    oled.text(
        0,
        37,
        "LOSS:"
    );

    oled.text(
        42,
        37,
        String(
            n.packetLoss
        ) + "%"
    );

    // --------------------------------------------------------
    // RSSI
    // --------------------------------------------------------

    oled.text(
        0,
        49,
        "RSSI:"
    );

    oled.text(
        42,
        49,
        String(
            n.rssi
        ) + "dBm"
    );

    // RSSI Sparkline
    if (appState.telemetryHistoryCount > 0) {
        float rssiData[TELEMETRY_HISTORY_SIZE];
        for (int i = 0; i < appState.telemetryHistoryCount; i++) {
            int idx = (appState.telemetryHistoryIndex - 1 - i + TELEMETRY_HISTORY_SIZE) % TELEMETRY_HISTORY_SIZE;
            rssiData[i] = (float)appState.telemetryHistory[idx].rssi;
        }
        oled.sparkline(70, 45, 55, 15, rssiData, appState.telemetryHistoryCount, -100, -20);
    }

    oled.update();
}

// ============================================================
// WIFI SCREEN
// ============================================================

void drawWiFi() {

    oled.clear();

    oled.header(
        "WIFI ANALYZER"
    );

    auto& analyzer =
        appState.network.analyzer;

    // ========================================================
    // SCANNING
    // ========================================================

    if (
        analyzer.scanning
    ) {

        oled.text(
            0,
            18,
            "SCANNING..."
        );

        oled.text(
            0,
            32,
            "Please wait"
        );

        oled.text(
            0,
            48,
            "OK = SCAN"
        );

        oled.update();

        return;
    }

    // ========================================================
    // NO DATA
    // ========================================================

    if (
        analyzer.networkCount == 0
    ) {

        oled.text(
            0,
            18,
            "No scan data"
        );

        oled.text(
            0,
            32,
            "OK = SCAN"
        );

        oled.text(
            0,
            46,
            "BACK = RETURN"
        );

        oled.update();

        return;
    }

    // ========================================================
    // SELECTED AP
    // ========================================================

    if (
        selectedAP >=
        analyzer.networkCount
    ) {

        selectedAP =
            analyzer.networkCount - 1;
    }

    WiFiNetworkInfo& ap =
        analyzer.networks[
            selectedAP
        ];

    // --------------------------------------------------------
    // INDEX
    // --------------------------------------------------------

    oled.text(
        0,
        12,
        String(
            selectedAP + 1
        ) +
        "/" +
        String(
            analyzer.networkCount
        )
    );

    // --------------------------------------------------------
    // SSID
    // --------------------------------------------------------

    String ssid =
        ap.ssid;

    if (
        ssid.length() == 0
    ) {

        ssid =
            "<hidden>";
    }

    if (
        ssid.length() > 20
    ) {

        ssid =
            ssid.substring(
                0,
                20
            );
    }

    oled.text(
        0,
        22,
        ssid
    );

    // --------------------------------------------------------
    // RSSI
    // --------------------------------------------------------

    oled.text(
        0,
        34,
        String(
            ap.rssi
        ) +
        " dBm"
    );

    // --------------------------------------------------------
    // CHANNEL
    // --------------------------------------------------------

    oled.text(
        60,
        34,
        "CH " +
        String(
            ap.channel
        )
    );

    // --------------------------------------------------------
    // SECURITY
    // --------------------------------------------------------

    oled.text(
        0,
        46,
        ap.security
    );

    // --------------------------------------------------------
    // SIGNAL BAR
    // --------------------------------------------------------

    int bars = 0;

    if (ap.rssi >= -50)
        bars = 4;

    else if (ap.rssi >= -60)
        bars = 3;

    else if (ap.rssi >= -70)
        bars = 2;

    else if (ap.rssi >= -80)
        bars = 1;

    else
        bars = 0;

    oled.raw().setCursor(
        60,
        46
    );

    oled.raw().print(
        "SIG "
    );

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        oled.raw().print(
            i < bars
                ? "|"
                : "."
        );
    }

    oled.update();
}

// ============================================================
// INTERNET SCREEN
// ============================================================

void drawInternet() {

    oled.clear();

    oled.header(
        "INTERNET TEST"
    );

    auto& n =
        appState.network;

    oled.text(
        0,
        13,
        n.wifiConnected
            ? "WIFI     OK"
            : "WIFI     FAIL"
    );

    oled.text(
        0,
        25,
        n.gatewayOnline
            ? "GATEWAY  OK"
            : "GATEWAY  FAIL"
    );

    oled.text(
        0,
        37,
        n.internetOnline
            ? "INTERNET OK"
            : "INTERNET FAIL"
    );

    oled.text(
        0,
        49,
        n.dnsOnline
            ? "DNS      OK"
            : "DNS      FAIL"
    );

    oled.update();
}

// ============================================================
// NETWORK INFO
// ============================================================

void drawNetworkInfo() {

    oled.clear();

    oled.header(
        "NETWORK INFO"
    );

    auto& n =
        appState.network;

    oled.text(
        0,
        13,
        "IP:"
    );

    oled.text(
        20,
        13,
        n.localIP.toString()
    );

    oled.text(
        0,
        26,
        "GW:"
    );

    oled.text(
        20,
        26,
        n.gatewayIP.toString()
    );

    oled.text(
        0,
        39,
        "DNS:"
    );

    oled.text(
        25,
        39,
        n.dnsIP.toString()
    );

    oled.text(
        0,
        52,
        "CH:"
    );

    oled.text(
        25,
        52,
        String(
            n.channel
        )
    );

    oled.update();
}

// ============================================================
// SYSTEM SCREEN
// ============================================================

void drawSystem() {

    oled.clear();

    oled.header(
        "SYSTEM"
    );

    uint32_t uptime =
        millis() -
        appState.bootTime;

    uint32_t seconds =
        uptime / 1000;

    uint32_t minutes =
        seconds / 60;

    uint32_t hours =
        minutes / 60;

    seconds %= 60;

    minutes %= 60;

    oled.text(
        0,
        13,
        "UPTIME:"
    );

    oled.text(
        45,
        13,
        String(hours) +
        ":" +
        String(minutes) +
        ":" +
        String(seconds)
    );

    oled.text(
        0,
        27,
        "HEAP:"
    );

    oled.text(
        40,
        27,
        String(
            ESP.getFreeHeap() /
            1024
        ) + "KB"
    );

    oled.text(
        0,
        41,
        "CPU:"
    );

    oled.text(
        40,
        41,
        String(
            getCpuFrequencyMhz()
        ) + "MHz"
    );

    oled.text(
        0,
        55,
        "CORE:"
    );

    oled.text(
        40,
        55,
        "0/1"
    );

    oled.update();
}

// ============================================================
// DRAW CURRENT SCREEN
// ============================================================

void drawWiFiStatistics() {

    oled.clear();

    oled.header(
        "WIFI STATS"
    );

    auto& a =
        appState.network.analyzer;

    // --------------------------------------------------------
    // NETWORK COUNT
    // --------------------------------------------------------

    oled.text(
        0,
        13,
        "APs:"
    );

    oled.text(
        40,
        13,
        String(
            a.networkCount
        )
    );

    // --------------------------------------------------------
    // AVERAGE RSSI
    // --------------------------------------------------------

    oled.text(
        0,
        25,
        "AVG:"
    );

    oled.text(
        40,
        25,
        String(
            (int)a.averageRSSI
        ) +
        "dBm"
    );

    // --------------------------------------------------------
    // OPEN
    // --------------------------------------------------------

    oled.text(
        0,
        37,
        "OPEN:"
    );

    oled.text(
        40,
        37,
        String(
            a.openNetworks
        )
    );

    // --------------------------------------------------------
    // BUSIEST
    // --------------------------------------------------------

    oled.text(
        0,
        49,
        "BUSY:"
    );

    oled.text(
        40,
        49,
        "CH " +
        String(
            a.busiestChannel
        )
    );

    oled.text(
        80,
        49,
        String(
            a.busiestChannelCount
        ) +
        " AP"
    );

    oled.update();
}


void drawLAN() {

    oled.clear();

    oled.header(
        "LAN DISCOVERY"
    );

    auto& scanner =
        appState.network.lanScanner;

    if (
        scanner.scanning
    ) {

        oled.text(
            0,
            16,
            "SCANNING LAN"
        );

        oled.text(
            0,
            32,
            String(scanner.scannedHosts) +
            "/" +
            String(scanner.totalHosts) +
            " hosts"
        );

        oled.text(
            0,
            48,
            String(scanner.deviceCount) +
            " devices"
        );

        oled.update();

        return;
    }

    if (
        scanner.deviceCount == 0
    ) {

        oled.text(
            0,
            18,
            "No LAN data"
        );

        oled.text(
            0,
            34,
            "OK = SCAN"
        );

        oled.text(
            0,
            50,
            appState.network.localIP.toString()
        );

        oled.update();

        return;
    }

    if (
        selectedLANDevice >=
        scanner.deviceCount
    ) {

        selectedLANDevice =
            scanner.deviceCount - 1;
    }

    LANDevice& device =
        scanner.devices[
            selectedLANDevice
        ];

    oled.text(
        0,
        12,
        String(selectedLANDevice + 1) +
        "/" +
        String(scanner.deviceCount)
    );

    oled.text(
        0,
        24,
        device.ip.toString()
    );

    String name =
        device.hostname.length() > 0
            ? device.hostname
            : "unknown";

    if (
        name.length() > 20
    ) {

        name =
            name.substring(
                0,
                20
            );
    }

    oled.text(
        0,
        38,
        name
    );

    oled.text(
        0,
        52,
        device.latency >= 0
            ? String((int)device.latency) + "ms ONLINE"
            : "OFFLINE"
    );

    oled.update();
}


void drawCurrentScreen() {

    if (
        !menu.isInside()
    ) {

        menu.draw();

        return;
    }

    switch (
        menu.selected()
    ) {

        case 0:

            drawDashboard();

            break;

        case 1:

            drawWiFi();

            break;

        case 2:

            drawLAN();

            break;

        case 3:

            drawInternet();

            break;

        case 4:

            drawWiFiStatistics();

            break;

        case 5:

            oled.clear();

            oled.header(
                "EVENTS"
            );

            oled.text(
                0,
                20,
                "COMING NEXT"
            );

            oled.update();

            break;

        case 6:

            drawNetworkInfo();

            break;

        case 7:

            drawSystem();

            break;
    }
}

// ============================================================
// SETUP
// ============================================================

void setup() {

    Serial.begin(
        115200
    );

    delay(500);

    Serial.println();
    Serial.println(
        "================================"
    );

    Serial.println(
        "NETWORK SENTINEL V6"
    );

    Serial.println(
        "NETWORK ENGINE"
    );

    Serial.println(
        "================================"
    );

    // --------------------------------------------------------
    // STATE
    // --------------------------------------------------------

    appState.bootTime =
        millis();

    // --------------------------------------------------------
    // INPUT
    // --------------------------------------------------------

    buttons.begin();

    // --------------------------------------------------------
    // OLED
    // --------------------------------------------------------

    if (
        !oled.begin()
    ) {

        Serial.println(
            "OLED FAILED"
        );

        while (true) {

            delay(1000);
        }
    }

    oled.clear();

    oled.header(
        "NETWORK SENTINEL"
    );

    oled.text(
        0,
        20,
        "Starting..."
    );

    oled.update();

    delay(1000);

    // --------------------------------------------------------
    // MENU
    // --------------------------------------------------------

    menu.begin();

    // --------------------------------------------------------
    // NETWORK
    // --------------------------------------------------------

    sentinelNetwork.begin();

    // --------------------------------------------------------
    // START BACKGROUND TASK
    // --------------------------------------------------------

    sentinelNetwork.startTask();

    sentinelWeb.begin();

    // --------------------------------------------------------
    // DRAW
    // --------------------------------------------------------

    drawCurrentScreen();

    Serial.println();
    Serial.println(
        "SYSTEM READY"
    );
}

// ============================================================
// LOOP
// ============================================================

void loop() {

    uint32_t loopStart = millis();

    // -------------------------------------------------------
    // Web Update
    // -------------------------------------------------------

    sentinelWeb.update();
    // --------------------------------------------------------
    // BUTTONS
    // --------------------------------------------------------

    buttons.update();

    // --------------------------------------------------------
    // MENU
    // --------------------------------------------------------

    if (
        !menu.isInside()
    ) {

        if (buttons.wasPressed(BUTTON_UP)) {
            menu.up();
        }

        if (buttons.wasPressed(BUTTON_DOWN)) {
            menu.down();
        }

        if (buttons.wasPressed(BUTTON_OK)) {
            menu.select();
        }

        if (buttons.wasPressed(BUTTON_BACK)) {
            menu.select(); // BACK acts as select in main menu (same as OK press)
        }
    }

    // --------------------------------------------------------
    // SCREEN
    // --------------------------------------------------------

    else {

    // ========================================================
    // WIFI ANALYZER
    // ========================================================

    if (
        menu.selected() == 1
    ) {

        if (buttons.wasPressed(BUTTON_UP)) {
            if (selectedAP > 0) {
                selectedAP--;
            }
        }

        if (buttons.wasPressed(BUTTON_DOWN)) {
            if (selectedAP <
                appState.network
                    .analyzer
                    .networkCount - 1) {
                selectedAP++;
            }
        }

        if (buttons.wasPressed(BUTTON_OK)) {
            sentinelNetwork.scanWiFi();
        }

        if (buttons.wasPressed(BUTTON_BACK)) {
            menu.back();
        }
    }

    // ========================================================
    // LAN DISCOVERY
    // ========================================================

    else if (
        menu.selected() == 2
    ) {

        if (buttons.wasPressed(BUTTON_UP)) {
            if (selectedLANDevice > 0) {
                selectedLANDevice--;
            }
        }

        if (buttons.wasPressed(BUTTON_DOWN)) {
            if (selectedLANDevice <
                appState.network
                    .lanScanner
                    .deviceCount - 1) {
                selectedLANDevice++;
            }
        }

        if (buttons.wasPressed(BUTTON_OK)) {
            sentinelNetwork.scanLAN();
        }

        if (buttons.wasPressed(BUTTON_BACK)) {
            menu.back();
        }
    }

    // ========================================================
    // OTHER SCREENS
    // ========================================================

    else {

        if (buttons.wasPressed(BUTTON_BACK)) {
            menu.back();
        }

        if (buttons.wasPressed(BUTTON_OK)) {
            sentinelNetwork.update();
        }
    }

    // Handle BACK long press to go to Home/Dashboard
    if (buttons.wasLongPressed(BUTTON_BACK)) {
        // Go to root menu by repeatedly calling menu.back() until at root
        while (!menu.isInside()) {
            menu.back();
        }
    }
}

// --------------------------------------------------------
// DISPLAY
// --------------------------------------------------------

    static uint32_t lastDisplay = 0;

    if (
        millis() -
        lastDisplay >=
        DISPLAY_UPDATE_MS
    ) {

        lastDisplay =
            millis();

        drawCurrentScreen();
    }

    delay(5);

    uint32_t loopTime = millis() - loopStart;
    appState.system.loopTimeMs = loopTime;

    if (loopTime > appState.system.maxLoopTimeMs) {
        appState.system.maxLoopTimeMs = loopTime;
    }

    appState.system.minFreeHeap = ESP.getMinFreeHeap();
}