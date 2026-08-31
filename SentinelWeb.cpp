#include "SentinelWeb.h"
#include "AppState.h"

SentinelWeb sentinelWeb;

// ============================================================
// HTML
// ============================================================

static const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta
    name="viewport"
    content="width=device-width,initial-scale=1"
>

<title>Network Sentinel</title>

<style>

* {
    box-sizing: border-box;
}

body {

    margin: 0;

    font-family:
        system-ui,
        -apple-system,
        BlinkMacSystemFont,
        "Segoe UI",
        sans-serif;

    background: #0b0f14;

    color: #e8edf2;
}

header {

    height: 64px;

    display: flex;

    align-items: center;

    justify-content: space-between;

    padding: 0 24px;

    background: #111820;

    border-bottom:
        1px solid #27313b;
}

.logo {

    font-size: 20px;

    font-weight: 700;

    letter-spacing: 1px;
}

.status {

    display: flex;

    align-items: center;

    gap: 8px;

    font-size: 14px;
}

.dot {

    width: 9px;

    height: 9px;

    border-radius: 50%;

    background: #35d07f;
}

.layout {

    display: flex;

    min-height:
        calc(100vh - 64px);
}

nav {

    width: 210px;

    padding: 20px 12px;

    background: #0f151b;

    border-right:
        1px solid #27313b;
}

nav button {

    width: 100%;

    padding: 12px;

    margin-bottom: 6px;

    border: 0;

    border-radius: 8px;

    background: transparent;

    color: #aeb8c2;

    text-align: left;

    cursor: pointer;

    font-size: 14px;
}

nav button:hover,
nav button.active {

    background: #1b2630;

    color: white;
}

main {

    flex: 1;

    padding: 28px;

    overflow: auto;
}

h1 {

    margin-top: 0;

    font-size: 26px;
}

.grid {

    display: grid;

    grid-template-columns:
        repeat(
            auto-fit,
            minmax(
                180px,
                1fr
            )
        );

    gap: 16px;

    margin-bottom: 24px;
}

.card {

    background: #111820;

    border:
        1px solid #27313b;

    border-radius: 12px;

    padding: 20px;
}

.card-title {

    color: #8e9aa6;

    font-size: 13px;

    margin-bottom: 8px;

    text-transform:
        uppercase;

    letter-spacing: .5px;
}

.value {

    font-size: 28px;

    font-weight: 700;
}

.ok {

    color: #35d07f;
}

.bad {

    color: #ff647c;
}

.info {

    display: grid;

    grid-template-columns:
        repeat(
            auto-fit,
            minmax(
                220px,
                1fr
            )
        );

    gap: 12px;
}

.row {

    display: flex;

    justify-content:
        space-between;

    padding: 12px 0;

    border-bottom:
        1px solid #202a33;
}

.label {

    color: #8e9aa6;
}

table {

    width: 100%;

    border-collapse:
        collapse;
}

th,
td {

    padding: 12px;

    text-align: left;

    border-bottom:
        1px solid #27313b;
}

th {

    color: #8e9aa6;

    font-size: 13px;
}

.badge {

    padding:
        4px 8px;

    border-radius: 5px;

    background: #1c2a23;

    color: #35d07f;

    font-size: 12px;
}

@media(max-width:700px) {

    nav {

        width: 60px;
    }

    nav button {

        font-size: 0;

        text-align: center;
    }

    nav button::first-letter {

        font-size: 18px;
    }

    main {

        padding: 16px;
    }

    header {

        padding: 0 14px;
    }
}

</style>

</head>

<body>

<header>

<div class="logo">
NETWORK SENTINEL
</div>

<div class="status">

<div
    id="statusDot"
    class="dot"
></div>

<span id="statusText">
CONNECTING
</span>

</div>

</header>

<div class="layout">

<nav>

<button
    class="active"
    onclick="showPage('dashboard',this)"
>
📊 Dashboard
</button>

<button
    onclick="showPage('wifi',this)"
>
📡 WiFi
</button>

<button
    onclick="showPage('lan',this)"
>
🖥 LAN
</button>

<button
    onclick="showPage('internet',this)"
>
🌐 Internet
</button>

<button
    onclick="showPage('system',this)"
>
⚙ System
</button>

</nav>

<main>

<section id="dashboard">

<h1>
Network Dashboard
</h1>

<div class="grid">

<div class="card">

<div class="card-title">
Internet Ping
</div>

<div
    id="ping"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
WiFi Signal
</div>

<div
    id="rssi"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Packet Loss
</div>

<div
    id="loss"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Channel
</div>

<div
    id="channel"
    class="value"
>
--
</div>

</div>

</div>

<div class="card">

<h3>
Connection
</h3>

<div class="row">

<span class="label">
SSID
</span>

<span id="ssid">
--
</span>

</div>

<div class="row">

<span class="label">
IP Address
</span>

<span id="ip">
--
</span>

</div>

<div class="row">

<span class="label">
Gateway
</span>

<span id="gateway">
--
</span>

</div>

<div class="row">

<span class="label">
DNS
</span>

<span id="dns">
--
</span>

</div>

</div>

</section>

<section
    id="wifi"
    style="display:none"
>

<h1>
WiFi Analyzer
</h1>

<div class="grid">

<div class="card">

<div class="card-title">
Networks
</div>

<div
    id="wifiCount"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Average RSSI
</div>

<div
    id="avgRSSI"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Open Networks
</div>

<div
    id="openNetworks"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Busiest Channel
</div>

<div
    id="busyChannel"
    class="value"
>
--
</div>

</div>

</div>

<div class="card">

<table>

<thead>

<tr>

<th>
SSID
</th>

<th>
RSSI
</th>

<th>
Channel
</th>

<th>
Security
</th>

</tr>

</thead>

<tbody id="wifiTable">

</tbody>

</table>

</div>

</section>

<section
    id="lan"
    style="display:none"
>

<h1>
LAN Scanner
</h1>

<div class="card">

<p>
LAN scanner coming next.
</p>

</div>

</section>

<section
    id="internet"
    style="display:none"
>

<h1>
Internet Diagnostics
</h1>

<div class="grid">

<div class="card">

<div class="card-title">
WiFi
</div>

<div
    id="wifiStatus"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Gateway
</div>

<div
    id="gatewayStatus"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Internet
</div>

<div
    id="internetStatus"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
DNS
</div>

<div
    id="dnsStatus"
    class="value"
>
--
</div>

</div>

</div>

</section>

<section
    id="system"
    style="display:none"
>

<h1>
System
</h1>

<div class="grid">

<div class="card">

<div class="card-title">
Free Heap
</div>

<div
    id="heap"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
CPU
</div>

<div
    id="cpu"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Uptime
</div>

<div
    id="uptime"
    class="value"
>
--
</div>

</div>

</div>

</section>

</main>

</div>

<script>

function showPage(
    page,
    button
) {

    document
        .querySelectorAll(
            "main section"
        )
        .forEach(
            section =>
                section.style.display =
                    "none"
        );

    document
        .getElementById(page)
        .style.display =
            "block";

    document
        .querySelectorAll(
            "nav button"
        )
        .forEach(
            b =>
                b.classList.remove(
                    "active"
                )
        );

    button.classList.add(
        "active"
    );
}

function setStatus(
    element,
    value
) {

    element.textContent =
        value
            ? "ONLINE"
            : "OFFLINE";

    element.className =
        value
            ? "value ok"
            : "value bad";
}

function updateDashboard(
    data
) {

    document.getElementById(
        "ping"
    ).textContent =
        data.internetPing >= 0
            ? Math.round(
                data.internetPing
            ) + " ms"
            : "--";

    document.getElementById(
        "rssi"
    ).textContent =
        data.rssi + " dBm";

    document.getElementById(
        "loss"
    ).textContent =
        data.packetLoss + "%";

    document.getElementById(
        "channel"
    ).textContent =
        data.channel;

    document.getElementById(
        "ssid"
    ).textContent =
        data.ssid;

    document.getElementById(
        "ip"
    ).textContent =
        data.ip;

    document.getElementById(
        "gateway"
    ).textContent =
        data.gateway;

    document.getElementById(
        "dns"
    ).textContent =
        data.dns;

    setStatus(
        document.getElementById(
            "wifiStatus"
        ),
        data.wifi
    );

    setStatus(
        document.getElementById(
            "gatewayStatus"
        ),
        data.gatewayOnline
    );

    setStatus(
        document.getElementById(
            "internetStatus"
        ),
        data.internetOnline
    );

    setStatus(
        document.getElementById(
            "dnsStatus"
        ),
        data.dnsOnline
    );

    document.getElementById(
        "heap"
    ).textContent =
        data.heap + " KB";

    document.getElementById(
        "cpu"
    ).textContent =
        data.cpu + " MHz";

    document.getElementById(
        "uptime"
    ).textContent =
        data.uptime;

    document.getElementById(
        "statusText"
    ).textContent =
        data.internetOnline
            ? "ONLINE"
            : "OFFLINE";
}

function updateWiFi(
    data
) {

    document.getElementById(
        "wifiCount"
    ).textContent =
        data.count;

    document.getElementById(
        "avgRSSI"
    ).textContent =
        Math.round(
            data.averageRSSI
        ) + " dBm";

    document.getElementById(
        "openNetworks"
    ).textContent =
        data.open;

    document.getElementById(
        "busyChannel"
    ).textContent =
        "CH " +
        data.busiestChannel;

    let table =
        document.getElementById(
            "wifiTable"
        );

    table.innerHTML = "";

    data.networks.forEach(
        network => {

            let row =
                document.createElement(
                    "tr"
                );

            row.innerHTML = `

                <td>
                    ${
                        network.ssid ||
                        "&lt;hidden&gt;"
                    }
                </td>

                <td>
                    ${network.rssi} dBm
                </td>

                <td>
                    ${network.channel}
                </td>

                <td>
                    <span class="badge">
                        ${network.security}
                    </span>
                </td>

            `;

            table.appendChild(
                row
            );
        }
    );
}

async function refresh() {

    try {

        let response =
            await fetch(
                "/api"
            );

        let data =
            await response.json();

        updateDashboard(
            data
        );

        let wifiResponse =
            await fetch(
                "/api/wifi"
            );

        let wifiData =
            await wifiResponse.json();

        updateWiFi(
            wifiData
        );

    }

    catch (error) {

        document.getElementById(
            "statusText"
        ).textContent =
            "DISCONNECTED";
    }
}

refresh();

setInterval(
    refresh,
    2000
);

</script>

</body>

</html>

)rawliteral";

// ============================================================
// BEGIN
// ============================================================

void SentinelWeb::begin() {

    setupRoutes();

    server.begin();

    Serial.println(
        "Web server started"
    );

    Serial.print(
        "Open: http://"
    );

    Serial.println(
        WiFi.localIP()
    );
}

// ============================================================
// UPDATE
// ============================================================

void SentinelWeb::update() {

    server.handleClient();
}

// ============================================================
// ROUTES
// ============================================================

void SentinelWeb::setupRoutes() {

    server.on(
        "/",
        HTTP_GET,
        [this]() {
            handleRoot();
        }
    );

    server.on(
        "/api",
        HTTP_GET,
        [this]() {
            handleAPI();
        }
    );

    server.on(
        "/api/wifi",
        HTTP_GET,
        [this]() {
            handleWiFi();
        }
    );

    server.onNotFound(
        [this]() {
            handleNotFound();
        }
    );
}

// ============================================================
// ROOT
// ============================================================

void SentinelWeb::handleRoot() {

    server.send_P(
        200,
        "text/html",
        INDEX_HTML
    );
}

// ============================================================
// API
// ============================================================

void SentinelWeb::handleAPI() {

    server.send(
        200,
        "application/json",
        createJSON()
    );
}

// ============================================================
// WIFI API
// ============================================================

void SentinelWeb::handleWiFi() {

    server.send(
        200,
        "application/json",
        createWiFiJSON()
    );
}

// ============================================================
// NOT FOUND
// ============================================================

void SentinelWeb::handleNotFound() {

    server.send(
        404,
        "text/plain",
        "Not Found"
    );
}

// ============================================================
// JSON
// ============================================================

String SentinelWeb::createJSON() {

    auto& n =
        appState.network;

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

    String json = "{";

    json +=
        "\"wifi\":" +
        String(
            n.wifiConnected
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"gatewayOnline\":" +
        String(
            n.gatewayOnline
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"internetOnline\":" +
        String(
            n.internetOnline
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"dnsOnline\":" +
        String(
            n.dnsOnline
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"rssi\":" +
        String(
            n.rssi
        ) +
        ",";

    json +=
        "\"channel\":" +
        String(
            n.channel
        ) +
        ",";

    json +=
        "\"internetPing\":" +
        String(
            n.internetPing
        ) +
        ",";

    json +=
        "\"packetLoss\":" +
        String(
            n.packetLoss
        ) +
        ",";

    json +=
        "\"dnsTime\":" +
        String(
            n.dnsTime
        ) +
        ",";

    json +=
        "\"ssid\":\"" +
        n.ssid +
        "\",";

    json +=
        "\"ip\":\"" +
        n.localIP.toString() +
        "\",";

    json +=
        "\"gateway\":\"" +
        n.gatewayIP.toString() +
        "\",";

    json +=
        "\"dns\":\"" +
        n.dnsIP.toString() +
        "\",";

    json +=
        "\"heap\":" +
        String(
            ESP.getFreeHeap() /
            1024
        ) +
        ",";

    json +=
        "\"cpu\":" +
        String(
            getCpuFrequencyMhz()
        ) +
        ",";

    char uptimeString[20];

    snprintf(
        uptimeString,
        sizeof(uptimeString),
        "%lu:%02lu:%02lu",
        (unsigned long)hours,
        (unsigned long)minutes,
        (unsigned long)seconds
    );

    json +=
        "\"uptime\":\"" +
        String(
            uptimeString
        ) +
        "\"";

    json += "}";

    return json;
}

// ============================================================
// WIFI JSON
// ============================================================

String SentinelWeb::createWiFiJSON() {

    auto& analyzer =
        appState.network.analyzer;

    String json =
        "{";

    json +=
        "\"count\":" +
        String(
            analyzer.networkCount
        ) +
        ",";

    json +=
        "\"averageRSSI\":" +
        String(
            analyzer.averageRSSI
        ) +
        ",";

    json +=
        "\"open\":" +
        String(
            analyzer.openNetworks
        ) +
        ",";

    json +=
        "\"busiestChannel\":" +
        String(
            analyzer.busiestChannel
        ) +
        ",";

    json +=
        "\"congestion\":" +
        String(
            analyzer.congestion
        ) +
        ",";

    json +=
        "\"scanning\":" +
        String(
            analyzer.scanning
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"networks\":[";

    for (
        int i = 0;
        i < analyzer.networkCount;
        i++
    ) {

        if (
            i > 0
        ) {

            json += ",";
        }

        auto& network =
            analyzer.networks[i];

        json += "{";

        json +=
            "\"ssid\":\"" +
            network.ssid +
            "\",";

        json +=
            "\"bssid\":\"" +
            network.bssid +
            "\",";

        json +=
            "\"rssi\":" +
            String(
                network.rssi
            ) +
            ",";

        json +=
            "\"channel\":" +
            String(
                network.channel
            ) +
            ",";

        json +=
            "\"security\":\"" +
            network.security +
            "\"";

        json += "}";
    }

    json += "]";

    json += "}";

    return json;
}