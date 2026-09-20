#include "config.h"
#include "SentinelWeb.h"
#include "AppState.h"
#include "NetworkManager.h"

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
    onclick="showPage('events',this)"
>
📋 Events
</button>

<button
    onclick="showPage('system',this)"
>
⚙ System
</button>

</nav>

<main>

<div id="alertBanner" style="display:none; background: #e74c3c; color: white; padding: 10px; margin-bottom: 20px; border-radius: 8px; font-weight: bold; text-align: center;"></div>


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

<div class="card">

<div class="card-title">
Health Score
</div>

<div
    id="healthScore"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Stability
</div>

<div
    id="stabilityScore"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
WiFi Uptime
</div>

<div
    id="wifiUptime"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Reconnects
</div>

<div
    id="reconnects"
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

<h2 style="margin-top: 32px;">
Telemetry Graphs
</h2>

<div class="grid" style="grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));">

<div class="card">

<canvas
    id="pingCanvas"
    style="width: 100%; height: 200px;"
></canvas>

</div>

<div class="card">

<canvas
    id="rssiCanvas"
    style="width: 100%; height: 200px;"
></canvas>

</div>

<div class="card">

<canvas
    id="lossCanvas"
    style="width: 100%; height: 200px;"
></canvas>

</div>

<div class="card">

<canvas
    id="dnsCanvas"
    style="width: 100%; height: 200px;"
></canvas>

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

<div class="card">

<div class="card-title">
Hidden Networks
</div>

<div
    id="hiddenNetworks"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Strongest Signal
</div>

<div
    id="strongestRSSI"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Weakest Signal
</div>

<div
    id="weakestRSSI"
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
LAN Discovery
</h1>

<div class="grid">

<div class="card">

<div class="card-title">
Devices
</div>

<div
    id="lanDeviceCount"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Scan State
</div>

<div
    id="lanScanState"
    class="value"
>
IDLE
</div>

</div>

<div class="card">

<div class="card-title">
Subnet
</div>

<div
    id="lanSubnet"
    class="value"
    style="font-size:18px"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Progress
</div>

<div
    id="lanProgress"
    class="value"
>
--
</div>

</div>

</div>

<div class="card">
    <div class="card-title">Auto-Scan Status</div>
    <div id="lanAutoStatus" class="value">--</div>
</div>

<div class="card" style="margin-bottom: 16px;">

<button
    onclick="startLANScan()"
    style="padding: 10px 14px; border: 0; border-radius: 8px; background: #35d07f; color: #07110b; font-weight: 700; cursor: pointer;"
>
Start LAN Scan
</button>

<span
    id="lanHint"
    class="label"
    style="margin-left: 12px;"
>
Scans the local subnet without leaving this page.
</span>

</div>

<div class="card">

<table>

<thead>

<tr>

<th>
IP Address
</th>

<th>
Hostname
</th>

<th>
MAC
</th>

<th>
Latency
</th>

<th>
Status
</th>

</tr>

</thead>

<tbody id="lanTable">

</tbody>

</table>

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

<div class="card">

<div class="card-title">
Internet Ping
</div>

<div
    id="intPing"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
Gateway Ping
</div>

<div
    id="intGatewayPing"
    class="value"
>
--
</div>

</div>

<div class="card">

<div class="card-title">
DNS Latency
</div>

<div
    id="intDnsTime"
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
    id="intLoss"
    class="value"
>
--
</div>

</div>

</div>

</section>

<section
    id="events"
    style="display:none"
>

<h1>
Event Log
</h1>

<div class="card">

<div
    id="eventList"
    class="info"
    style="max-height:600px;overflow:auto"
>
<div class="label">
Waiting for events...
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
<div class="card">
<div class="card-title">
System Load
</div>
<div id="loopTime" class="value">
--
</div>
</div>
<div class="card">
<div class="card-title">
WiFi Power
</div>
<div id="wifiPower" class="value">
--
</div>
</div>
<div class="card">
<div class="card-title">
WiFi MAC
</div>
<div id="wifiMAC" class="value" style="font-size:14px">
--
</div>
</div>
<div class="card">
<div class="card-title">
Firmware
</div>
<div id="firmware" class="value" style="font-size:18px">
V6.0
</div>
</div>
<div class="card">
<div class="card-title">
Min Heap
</div>
<div id="minHeap" class="value">
--
</div>
</div>
<div class="card">
<div class="card-title">
Max Loop
</div>
<div id="maxLoop" class="value">
--
</div>
</div>
</div>

<h2 style="margin-top: 32px;">
System History
</h2>
<div class="grid" style="grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));">
<div class="card">
<canvas
    id="memoryCanvas"
    style="width: 100%; height: 200px;"
></canvas>
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

    // Bug-fix #2: data.uptime is "H:M:S" string from /api;
    // formatUptime() expects milliseconds. Display the raw
    // string as-is.
    document.getElementById(
        "uptime"
    ).textContent =
        data.uptime;

    // --------------------------------------------------------
    // Bug-fix #4: dashboard WiFiPower + loopTime cards
    // --------------------------------------------------------

    if (
        data.wifiTxPower !== undefined
    ) {
        document.getElementById(
            "wifiPower"
        ).textContent =
            data.wifiTxPower +
            " dBm";
    }

    if (
        data.loopTimeMs !== undefined
    ) {
        document.getElementById(
            "loopTime"
        ).textContent =
            data.loopTimeMs + " ms";
    }

    if (
        data.minFreeHeap !== undefined
    ) {
        document.getElementById(
            "minHeap"
        ).textContent =
            Math.round(
                data.minFreeHeap / 1024
            ) + " KB";
    }

    if (
        data.maxLoopTimeMs !== undefined
    ) {
        document.getElementById(
            "maxLoop"
        ).textContent =
            data.maxLoopTimeMs + " ms";
    }

    if (
        data.wifiMAC !== undefined
    ) {
        document.getElementById(
            "wifiMAC"
        ).textContent =
            data.wifiMAC;
    }

    // --------------------------------------------------------
    // Bug-fix #3: alert banner
    // --------------------------------------------------------

    const banner =
        document.getElementById(
            "alertBanner"
        );

    const alerts = [];

    if (data.lowMemory)   alerts.push("LOW MEMORY");
    if (data.cpuBlocked)  alerts.push("CPU BLOCKED");
    if (data.highLatency) alerts.push("HIGH LATENCY");
    if (
        data.wifi === false
    ) alerts.push("WIFI OFFLINE");

    if (alerts.length > 0) {
        banner.textContent =
            "ALERT: " + alerts.join(", ");
        banner.style.display = "block";
    } else {
        banner.style.display = "none";
    }

    // --------------------------------------------------------
    // Health / Stability / WiFi uptime / Reconnects
    // --------------------------------------------------------

    if (
        data.healthScore !== undefined
    ) {
        const hs =
            document.getElementById(
                "healthScore"
            );
        hs.textContent =
            data.healthScore + "/100";
        hs.className =
            data.healthScore >= 80
                ? "value ok"
                : data.healthScore >= 50
                    ? "value"
                    : "value bad";
    }

    if (
        data.stabilityScore !== undefined
    ) {
        const ss =
            document.getElementById(
                "stabilityScore"
            );
        ss.textContent =
            data.stabilityScore + "/100";
        ss.className =
            data.stabilityScore >= 80
                ? "value ok"
                : data.stabilityScore >= 50
                    ? "value"
                    : "value bad";
    }

    if (
        data.wifiUptimeSec !== undefined
    ) {
        document.getElementById(
            "wifiUptime"
        ).textContent =
            formatUptime(
                data.wifiUptimeSec * 1000
            );
    }

    if (
        data.wifiReconnects !== undefined
    ) {
        document.getElementById(
            "reconnects"
        ).textContent =
            data.wifiReconnects;
    }

    // --------------------------------------------------------
    // Internet page cards
    // --------------------------------------------------------

    if (
        data.internetPing !== undefined
    ) {
        const el =
            document.getElementById("intPing");
        if (el) {
            el.textContent =
                data.internetPing >= 0
                    ? Math.round(
                        data.internetPing
                    ) + " ms"
                    : "--";
        }
    }

    if (
        data.gatewayPing !== undefined
    ) {
        const el =
            document.getElementById("intGatewayPing");
        if (el) {
            el.textContent =
                data.gatewayPing >= 0
                    ? Math.round(
                        data.gatewayPing
                    ) + " ms"
                    : "--";
        }
    }

    if (
        data.dnsTime !== undefined
    ) {
        const el =
            document.getElementById("intDnsTime");
        if (el) {
            el.textContent =
                data.dnsTime + " ms";
        }
    }

    if (
        data.packetLoss !== undefined
    ) {
        const el =
            document.getElementById("intLoss");
        if (el) {
            el.textContent =
                data.packetLoss + "%";
        }
    }

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

function updateLAN(
    data
) {

    document.getElementById(
        "lanDeviceCount"
    ).textContent =
        data.deviceCount;

    document.getElementById(
        "lanScanState"
    ).textContent =
        data.scanning
            ? "SCANNING"
            : (data.scanComplete ? "COMPLETE" : "IDLE");

    document.getElementById(
        "lanScanState"
    ).className =
        data.scanning
            ? "value"
            : (data.scanComplete ? "value ok" : "value");

    document.getElementById(
        "lanSubnet"
    ).textContent =
        (data.subnetStart || "--") +
        " - " +
        (data.subnetEnd || "--");

    document.getElementById(
        "lanProgress"
    ).textContent =
        data.totalHosts > 0
            ? data.scannedHosts + "/" + data.totalHosts
            : "--";

    // Update auto-scan status
    const autoStatusEl = document.getElementById("lanAutoStatus");
    if (data.autoScanning) {
        if (data.scanning) {
            autoStatusEl.textContent = "SCANNING";
            autoStatusEl.className = "value";
        } else {
            const seconds = Math.max(0, Math.round(data.timeToNextScan / 1000));
            autoStatusEl.textContent = seconds > 0 ? `WAIT ${seconds}s` : "IDLE";
            autoStatusEl.className = seconds > 0 ? "value" : "value ok";
        }
    } else {
        autoStatusEl.textContent = "MANUAL";
        autoStatusEl.className = "value";
    }

    const table =
        document.getElementById(
            "lanTable"
        );

    table.innerHTML = "";

    if (!data.devices || data.devices.length === 0) {

        const row =
            document.createElement(
                "tr"
            );

        row.innerHTML =
            "<td colspan='5' class='label'>No LAN devices discovered yet.</td>";

        table.appendChild(row);

        return;
    }

    data.devices.forEach(
        device => {

            const row =
                document.createElement(
                    "tr"
                );

            row.innerHTML = `
                <td>${device.ip}</td>
                <td>${device.hostname || "unknown"}</td>
                <td>${device.mac || "--"}</td>
                <td>${device.latency >= 0 ? device.latency + " ms" : "--"}</td>
                <td><span class="badge">${device.online ? "ONLINE" : "OFFLINE"}</span></td>
            `;

            table.appendChild(row);
        }
    );
}

async function refreshLAN() {

    try {

        const response =
            await fetch(
                "/api/lan"
            );

        const data =
            await response.json();

        updateLAN(data);

    } catch (error) {

        console.error(
            "LAN refresh failed:",
            error
        );
    }
}

async function startLANScan() {

    try {

        await fetch(
            "/api/lan/scan"
        );

        refreshLAN();

    } catch (error) {

        console.error(
            "LAN scan request failed:",
            error
        );
    }
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

        await refreshLAN();

    }

    catch (error) {

        document.getElementById(
            "statusText"
        ).textContent =
            "DISCONNECTED";
    }
}



// ============================================================
// WEBSOCKET LIVE TELEMETRY
// ============================================================

let socket = null;

function connectWebSocket() {

    const protocol =
        location.protocol === "https:"
            ? "wss:"
            : "ws:";

    socket =
        new WebSocket(
            protocol +
            "//" +
            location.hostname +
            ":81/"
        );

    socket.onopen = function() {

        console.log(
            "WebSocket connected"
        );

        document.getElementById(
            "statusText"
        ).textContent =
            "LIVE";

        document.getElementById(
            "statusDot"
        ).style.background =
            "#35d07f";
    };

    socket.onmessage =
        function(event) {

            try {

                const data =
                    JSON.parse(
                        event.data
                    );

                // --------------------------------------------
                // TELEMETRY
                // --------------------------------------------

                if (
                    data.type ===
                    "telemetry"
                ) {

                    updateLiveTelemetry(
                        data
                    );

                    addTelemetryToHistory(
                        data
                    );
                }

                // --------------------------------------------
                // WIFI
                // --------------------------------------------

                else if (
                    data.type ===
                    "wifi"
                ) {

                    updateLiveWiFi(
                        data
                    );
                }

                // --------------------------------------------
                // HISTORY
                // --------------------------------------------

                else if (
                    data.type ===
                    "history"
                ) {

                    updateHistory(
                        data
                    );
                }

                // --------------------------------------------
                // LAN
                // --------------------------------------------

                else if (
                    data.type ===
                    "lan"
                ) {

                    updateLAN(
                        data
                    );
                }

                // --------------------------------------------
                // EVENTS
                // --------------------------------------------

                else if (
                    data.type ===
                    "events"
                ) {

                    updateEvents(
                        data
                    );
                }

            }

            catch(error) {

                console.error(
                    "WebSocket JSON error:",
                    error
                );
            }
        };

    socket.onclose = function() {

        console.log(
            "WebSocket disconnected"
        );

        document.getElementById(
            "statusText"
        ).textContent =
            "RECONNECTING";

        setTimeout(
            connectWebSocket,
            2000
        );
    };

    socket.onerror = function(error) {

        console.error(
            "WebSocket error:",
            error
        );

        socket.close();
    };
}

function updateLiveTelemetry(
    data
) {

    document.getElementById(
        "ping"
    ).textContent =
        data.ping >= 0
            ? Math.round(
                data.ping
            ) + " ms"
            : "--";

    document.getElementById(
        "rssi"
    ).textContent =
        data.rssi +
        " dBm";

    document.getElementById(
        "loss"
    ).textContent =
        data.packetLoss +
        "%";

    document.getElementById(
        "channel"
    ).textContent =
        data.channel;

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
        data.gateway
    );

    setStatus(
        document.getElementById(
            "internetStatus"
        ),
        data.internet
    );

    setStatus(
        document.getElementById(
            "dnsStatus"
        ),
        data.dns
    );

    document.getElementById(
        "heap"
    ).textContent =
        Math.round(
            data.heap / 1024
        ) +
        " KB";

    document.getElementById(
        "uptime"
    ).textContent =
        formatUptime(
            data.uptime
        );

    // Health / stability / lowMemory alert
    if (
        data.healthScore !== undefined
    ) {
        const hs =
            document.getElementById(
                "healthScore"
            );
        hs.textContent =
            data.healthScore + "/100";
        hs.className =
            data.healthScore >= 80
                ? "value ok"
                : data.healthScore >= 50
                    ? "value"
                    : "value bad";
    }

    if (
        data.stabilityScore !== undefined
    ) {
        const ss =
            document.getElementById(
                "stabilityScore"
            );
        ss.textContent =
            data.stabilityScore + "/100";
        ss.className =
            data.stabilityScore >= 80
                ? "value ok"
                : data.stabilityScore >= 50
                    ? "value"
                    : "value bad";
    }

    if (
        data.lowMemory !== undefined
    ) {
        const banner =
            document.getElementById(
                "alertBanner"
            );
        if (data.lowMemory) {
            banner.textContent =
                "ALERT: LOW MEMORY";
            banner.style.display = "block";
        } else {
            banner.style.display = "none";
        }
    }

    document.getElementById(
        "statusText"
    ).textContent =
        data.internet
            ? "LIVE"
            : "OFFLINE";
}

function updateLiveWiFi(
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
        ) +
        " dBm";

    document.getElementById(
        "openNetworks"
    ).textContent =
        data.open;

    document.getElementById(
        "busyChannel"
    ).textContent =
        "CH " +
        data.busiestChannel;

    // Hidden / strongest / weakest
    if (
        data.hidden !== undefined
    ) {
        const el =
            document.getElementById(
                "hiddenNetworks"
            );
        if (el) {
            el.textContent =
                data.hidden;
        }
    }

    if (
        data.strongestRSSI !== undefined
    ) {
        const el =
            document.getElementById(
                "strongestRSSI"
            );
        if (el) {
            el.textContent =
                data.strongestRSSI +
                " dBm";
        }
    }

    if (
        data.weakestRSSI !== undefined
    ) {
        const el =
            document.getElementById(
                "weakestRSSI"
            );
        if (el) {
            el.textContent =
                data.weakestRSSI +
                " dBm";
        }
    }
}

function formatUptime(
    milliseconds
) {

    let seconds =
        Math.floor(
            milliseconds / 1000
        );

    let hours =
        Math.floor(
            seconds / 3600
        );

    seconds %= 3600;

    let minutes =
        Math.floor(
            seconds / 60
        );

    seconds %= 60;

    return (
        hours +
        ":" +
        String(minutes)
            .padStart(2, "0") +
        ":" +
        String(seconds)
            .padStart(2, "0")
    );
}

// ============================================================
// HISTORY & GRAPHS
// ============================================================

let history = [];

function updateHistory(data) {

    // Load full history buffer
    history = data.samples || [];

    // Redraw all graphs
    drawAllGraphs();
}

function addTelemetryToHistory(data) {

    // Add new sample
    history.push({
        time: data.uptime,
        ping: data.ping,
        rssi: data.rssi,
        loss: data.packetLoss,
        dns: data.dnsTime,
        heap: data.heap,
        state: data.state
    });

    // Maintain 60-sample limit
    if (history.length > 60) {
        history.shift();
    }

    // Redraw graphs
    drawAllGraphs();
}

function updateEvents(
    data
) {

    const list =
        document.getElementById(
            "eventList"
        );

    if (
        !data.events ||
        data.events.length === 0
    ) {

        list.innerHTML =
            '<div class="label">' +
            "Waiting for events..." +
            "</div>";

        return;
    }

    let html = "";

    for (let i = 0;
         i < data.events.length;
         i++) {

        const e = data.events[i];

        const color =
            e.severity === 2
                ? "#e74c3c"
                : e.severity === 1
                    ? "#f39c12"
                    : "#3498db";

        html +=
            '<div style="border-left:3px solid ' +
            color +
            ';padding:6px;margin-bottom:4px">';

        html +=
            '<div style="font-weight:600">' +
            escapeHtml(
                e.title
            ) +
            "</div>";

        html +=
            '<div class="label">' +
            escapeHtml(
                e.message
            ) +
            "</div>";

        html +=
            '<div class="label">' +
            escapeHtml(
                e.source
            ) +
            " · " +
            escapeHtml(
                e.timestamp
            ) +
            "</div>";

        html += "</div>";
    }

    list.innerHTML = html;
}

function escapeHtml(
    str
) {

    if (!str) return "";

    return str
        .toString()
        .replace(/&/g, "&")
        .replace(/</g, "<")
        .replace(/>/g, ">")
        .replace(/"/g, """);
}

function drawAllGraphs() {

    if (history.length === 0) {
        return;
    }

    drawGraph(
        "pingCanvas",
        history.map(s => s.ping),
        0,
        200,
        "ms",
        "Ping"
    );

    drawGraph(
        "rssiCanvas",
        history.map(s => s.rssi),
        -100,
        -20,
        "dBm",
        "RSSI"
    );

    drawGraph(
        "lossCanvas",
        history.map(s => s.loss),
        0,
        100,
        "%",
        "Loss"
    );

    drawGraph(
        "dnsCanvas",
        history.map(s => s.dns),
        0,
        500,
        "ms",
        "DNS"
    );

    drawGraph(
        "memoryCanvas",
        history.map(s => s.heap ? s.heap : 0),
        0,
        300,
        "KB",
        "Free Heap"
    );
}

function drawGraph(
    canvasId,
    values,
    minValue,
    maxValue,
    unit,
    title
) {

    const canvas =
        document.getElementById(canvasId);

    if (!canvas) {
        return;
    }

    const ctx =
        canvas.getContext("2d");

    const dpr =
        window.devicePixelRatio || 1;

    const rect =
        canvas.getBoundingClientRect();

    canvas.width =
        rect.width * dpr;

    canvas.height =
        rect.height * dpr;

    ctx.scale(dpr, dpr);

    const width = rect.width;
    const height = rect.height;

    // Clear
    ctx.fillStyle = "#0b0f14";
    ctx.fillRect(0, 0, width, height);

    if (values.length < 2) {
        ctx.fillStyle = "#8e9aa6";
        ctx.font = "13px sans-serif";
        ctx.textAlign = "center";
        ctx.fillText(
            "Collecting data...",
            width / 2,
            height / 2
        );
        return;
    }

    // Calculate statistics
    // For RSSI (negative values), we need all values
    // For other metrics, filter out invalid (-1) values
    const validValues = values.filter(v =>
        v !== undefined &&
        v !== null &&
        (minValue < 0 ? true : v >= 0)
    );
    const currentValue = values[values.length - 1];
    const min = validValues.length > 0
        ? Math.min(...validValues)
        : 0;
    const max = validValues.length > 0
        ? Math.max(...validValues)
        : 0;
    const avg = validValues.length > 0
        ? validValues.reduce((a, b) => a + b, 0) / validValues.length
        : 0;

    // Auto-scale to actual data
    let dataMin = Math.min(...validValues);
    let dataMax = Math.max(...validValues);

    // Use provided min/max if data fits
    if (dataMin >= minValue && dataMax <= maxValue) {
        // Data fits in range
    } else {
        // Expand range for data
        minValue = Math.min(minValue, dataMin - 10);
        maxValue = Math.max(maxValue, dataMax + 10);
    }

    const padding = 40;
    const graphWidth = width - padding * 2;
    const graphHeight = height - padding * 2;

    // Grid
    ctx.strokeStyle = "#1b2630";
    ctx.lineWidth = 1;

    for (let i = 0; i <= 4; i++) {
        const y = padding + (graphHeight / 4) * i;
        ctx.beginPath();
        ctx.moveTo(padding, y);
        ctx.lineTo(width - padding, y);
        ctx.stroke();
    }

    // Axis labels
    ctx.fillStyle = "#8e9aa6";
    ctx.font = "11px sans-serif";
    ctx.textAlign = "right";

    for (let i = 0; i <= 4; i++) {
        const y = padding + (graphHeight / 4) * i;
        const value =
            maxValue - ((maxValue - minValue) / 4) * i;
        ctx.fillText(
            Math.round(value) + unit,
            padding - 8,
            y + 4
        );
    }

    // Line
    ctx.strokeStyle = "#35d07f";
    ctx.lineWidth = 2;
    ctx.beginPath();

    for (let i = 0; i < values.length; i++) {

        const x =
            padding +
            (graphWidth / (values.length - 1)) * i;

        const normalized =
            (values[i] - minValue) /
            (maxValue - minValue);

        const y =
            padding +
            graphHeight -
            normalized * graphHeight;

        if (i === 0) {
            ctx.moveTo(x, y);
        } else {
            ctx.lineTo(x, y);
        }
    }

    ctx.stroke();

    // Current value
    ctx.fillStyle = "#e8edf2";
    ctx.font = "bold 16px sans-serif";
    ctx.textAlign = "left";
    ctx.fillText(
        title + ": " +
        (currentValue >= 0
            ? Math.round(currentValue) + unit
            : "--"),
        padding,
        20
    );

    // Statistics (min/max/avg) at bottom
    ctx.font = "11px sans-serif";
    ctx.fillStyle = "#8e9aa6";

    const statsText = "Min: " + Math.round(min) + unit +
        "  Max: " + Math.round(max) + unit +
        "  Avg: " + Math.round(avg) + unit;

    ctx.textAlign = "right";
    ctx.fillText(
        statsText,
        width - padding,
        height - 8
    );
}

window.addEventListener("resize", drawAllGraphs);

// Initial REST load
refresh();

// Live WebSocket connection
connectWebSocket();

</script>

</body>

</html>

)rawliteral";

// ============================================================
// BEGIN
// ============================================================

void SentinelWeb::begin() {

    setupRoutes();

    setupWebSocket();

    server.begin();

    webSocket.begin();

    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        "NETWORK SENTINEL WEB"
    );

    Serial.println(
        "================================"
    );

    Serial.print(
        "HTTP:      http://"
    );

    Serial.println(
        WiFi.localIP()
    );

    Serial.print(
        "WebSocket: ws://"
    );

    Serial.print(
        WiFi.localIP()
    );

    Serial.println(
        ":81"
    );

    Serial.println(
        "================================"
    );
}

// ============================================================
// UPDATE
// ============================================================

void SentinelWeb::update() {

    server.handleClient();

    webSocket.loop();

    uint32_t now =
        millis();

    // ========================================================
    // TELEMETRY
    // ========================================================

    if (
    now -
    lastTelemetryBroadcast >=
    1000
    ) {

    lastTelemetryBroadcast =
        now;

    recordTelemetry();

    broadcastTelemetry();
    }

    // ========================================================
    // WIFI DATA
    // ========================================================

    if (
        now -
        lastWiFiBroadcast >=
        2000
    ) {

        lastWiFiBroadcast =
            now;

        broadcastWiFi();
    }

    // ========================================================
    // LAN DATA
    // ========================================================

    if (
        now -
        lastLANBroadcast >=
        3000
    ) {

        lastLANBroadcast =
            now;

        broadcastLAN();
    }

    // ========================================================
    // EVENTS
    // ========================================================

    static uint32_t lastEventsBroadcast = 0;

    if (
        now -
        lastEventsBroadcast >=
        5000
    ) {

        lastEventsBroadcast =
            now;

        broadcastEvents();
    }
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

    server.on(
        "/api/lan",
        HTTP_GET,
        [this]() {
            handleLAN();
        }
    );

    server.on(
        "/api/lan/scan",
        HTTP_GET,
        [this]() {
            handleLANScan();
        }
    );

    server.on(
        "/api/events",
        HTTP_GET,
        [this]() {
            handleEvents();
        }
    );

    server.onNotFound(
        [this]() {
            handleNotFound();
        }
    );
}


void SentinelWeb::setupWebSocket() {

    webSocket.onEvent(
        [this](
            uint8_t clientNum,
            WStype_t type,
            uint8_t* payload,
            size_t length
        ) {

            webSocketEvent(
                clientNum,
                type,
                payload,
                length
            );
        }
    );
}


void SentinelWeb::webSocketEvent(
    uint8_t clientNum,
    WStype_t type,
    uint8_t* payload,
    size_t length
) {

    switch (type) {

        case WStype_CONNECTED: {

            Serial.print(
                "WebSocket client connected: "
            );

            Serial.println(
                clientNum
            );

            // Create actual String variables.
            String telemetry =
                createTelemetryJSON();

            String wifi =
                createWiFiEventJSON();

            String history =
                createHistoryJSON();

            String lan =
                createLANJSON();

            String events =
                createEventsJSON();

            webSocket.sendTXT(
                clientNum,
                telemetry
            );

            webSocket.sendTXT(
                clientNum,
                wifi
            );

            webSocket.sendTXT(
                clientNum,
                history
            );

            webSocket.sendTXT(
                clientNum,
                lan
            );

            webSocket.sendTXT(
                clientNum,
                events
            );

            break;
        }

        case WStype_DISCONNECTED:

            Serial.print(
                "WebSocket client disconnected: "
            );

            Serial.println(
                clientNum
            );

            break;

        case WStype_TEXT:

            Serial.print(
                "WebSocket message: "
            );

            Serial.println(
                (char*)payload
            );

            break;

        default:

            break;
    }
}


void SentinelWeb::broadcastTelemetry() {

    if (
        webSocket.connectedClients() == 0
    ) {

        return;
    }

    String telemetry =
        createTelemetryJSON();

    webSocket.broadcastTXT(
        telemetry
    );
}


void SentinelWeb::broadcastHistory() {

    if (
        webSocket.connectedClients() == 0
    ) {

        return;
    }

    String history =
        createHistoryJSON();

    webSocket.broadcastTXT(
        history
    );
}


void SentinelWeb::broadcastWiFi() {

    if (
        webSocket.connectedClients() == 0
    ) {

        return;
    }

    String wifi =
        createWiFiEventJSON();

    webSocket.broadcastTXT(
        wifi
    );
}

void SentinelWeb::broadcastLAN() {

    if (
        webSocket.connectedClients() == 0
    ) {

        return;
    }

    String lan =
        createLANJSON();

    webSocket.broadcastTXT(
        lan
    );
}



String SentinelWeb::createTelemetryJSON() {

    auto& n =
        appState.network;

    String json =
        "{";

    json +=
        "\"type\":\"telemetry\",";

    json +=
        "\"wifi\":" +
        String(
            n.wifiConnected
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"gateway\":" +
        String(
            n.gatewayOnline
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"internet\":" +
        String(
            n.internetOnline
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"dns\":" +
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
        "\"ping\":" +
        String(
            n.internetPing
        ) +
        ",";

    json +=
        "\"gatewayPing\":" +
        String(
            n.gatewayPing
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
        "\"heap\":" +
        String(
            ESP.getFreeHeap()
        ) +
        ",";

    json +=
        "\"healthScore\":" +
        String(
            n.healthScore
        ) +
        ",";

    json +=
        "\"stabilityScore\":" +
        String(
            n.stabilityScore
        ) +
        ",";

    json +=
        "\"lowMemory\":" +
        String(
            appState.system.alerts.lowMemory
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"uptime\":" +
        String(
            millis()
        );

    json +=
        "}";

    return json;
}


String SentinelWeb::createWiFiEventJSON() {

    auto& a =
        appState.network.analyzer;

    String json =
        "{";

    json +=
        "\"type\":\"wifi\",";

    json +=
        "\"count\":" +
        String(
            a.networkCount
        ) +
        ",";

    json +=
        "\"averageRSSI\":" +
        String(
            a.averageRSSI
        ) +
        ",";

    json +=
        "\"open\":" +
        String(
            a.openNetworks
        ) +
        ",";

    json +=
        "\"hidden\":" +
        String(
            a.hiddenNetworks
        ) +
        ",";

    json +=
        "\"busiestChannel\":" +
        String(
            a.busiestChannel
        ) +
        ",";

    json +=
        "\"congestion\":" +
        String(
            a.congestion
        ) +
        ",";

    json +=
        "\"strongestRSSI\":" +
        String(
            a.strongestIndex >= 0
                ? a.networks[
                    a.strongestIndex
                ].rssi
                : 0
        ) +
        ",";

    json +=
        "\"weakestRSSI\":" +
        String(
            a.weakestIndex >= 0
                ? a.networks[
                    a.weakestIndex
                ].rssi
                : 0
        ) +
        ",";

    json +=
        "\"scanning\":" +
        String(
            a.scanning
                ? "true"
                : "false"
        );

    json +=
        "}";

    return json;
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
// LAN API
// ============================================================

void SentinelWeb::handleLAN() {

    server.send(
        200,
        "application/json",
        createLANJSON()
    );
}


void SentinelWeb::handleLANScan() {

    sentinelNetwork.scanLAN();

    server.send(
        202,
        "application/json",
        createLANJSON()
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

    json +=
        "\"loopTimeMs\":" +
        String(appState.system.loopTimeMs) +
        ",";

    json +=
        "\"maxLoopTimeMs\":" +
        String(appState.system.maxLoopTimeMs) +
        ",";

    json +=
        "\"minFreeHeap\":" +
        String(appState.system.minFreeHeap) +
        ",";

    json +=
        "\"wifiTxPower\":" +
        String(appState.system.wifiTxPower) +
        ",";

    json +=
        "\"lowMemory\":" +
        String(appState.system.alerts.lowMemory ? "true" : "false") +
        ",";

    json +=
        "\"cpuBlocked\":" +
        String(appState.system.alerts.cpuBlocked ? "true" : "false") +
        ",";

    json +=
        "\"highLatency\":" +
        String(appState.system.alerts.highLatency ? "true" : "false") +
        ",";

    // WiFi MAC
    json +=
        "\"wifiMAC\":\"" +
        n.wifiMAC +
        "\",";

    // WiFi connection uptime in seconds (or 0 if not connected)
    uint32_t wifiUpSec = 0;
    if (
        n.wifiConnectedSince > 0 &&
        n.wifiConnected
    ) {
        wifiUpSec = (millis() - n.wifiConnectedSince) / 1000;
    }

    json +=
        "\"wifiUptimeSec\":" +
        String(wifiUpSec) +
        ",";

    json +=
        "\"wifiReconnects\":" +
        String(n.wifiReconnectCount) +
        ",";

    json +=
        "\"stabilityScore\":" +
        String(n.stabilityScore) +
        ",";

    json +=
        "\"healthScore\":" +
        String(n.healthScore) +
        ",";

    json +=
        "\"pingSpikes\":" +
        String(n.pingSpikes) +
        ",";

    json +=
        "\"lossSpikes\":" +
        String(n.lossSpikes) +
        ",";

    json +=
        "\"rssiDrops\":" +
        String(n.rssiDrops) +
        ",";

    json +=
        "\"gatewayPing\":" +
        String(n.gatewayPing) +
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


void SentinelWeb::recordTelemetry() {

    TelemetrySample& sample =
        appState.telemetryHistory[
            appState.telemetryHistoryIndex
        ];

    sample.timestamp =
        millis();

    sample.ping =
        appState.network.internetPing;

    sample.rssi =
        appState.network.rssi;

    sample.packetLoss =
        appState.network.packetLoss;

    sample.dnsTime =
        appState.network.dnsTime;

    sample.heap =
        ESP.getFreeHeap() / 1024;

    // Bit 0 = wifi, Bit 1 = gateway, Bit 2 = internet, Bit 3 = dns
    sample.stateFlags =
        (appState.network.wifiConnected   ? 1 : 0) |
        (appState.network.gatewayOnline   ? 2 : 0) |
        (appState.network.internetOnline  ? 4 : 0) |
        (appState.network.dnsOnline       ? 8 : 0);

    // Detect outages and spikes once per sample
    detectOutages();
    detectSpikes();
    computeHealthScore();

    appState.telemetryHistoryIndex++;

    if (
        appState.telemetryHistoryIndex >=
        TELEMETRY_HISTORY_SIZE
    ) {

        appState.telemetryHistoryIndex = 0;
    }

    if (
        appState.telemetryHistoryCount <
        TELEMETRY_HISTORY_SIZE
    ) {

        appState.telemetryHistoryCount++;
    }
}


String SentinelWeb::createHistoryJSON() {

    String json = "{";

    json += "\"type\":\"history\",";

    json += "\"samples\":[";

    uint8_t count =
        appState.telemetryHistoryCount;

    uint8_t start = 0;

    if (
        count ==
        TELEMETRY_HISTORY_SIZE
    ) {

        start =
            appState.telemetryHistoryIndex;
    }

    for (
        uint8_t i = 0;
        i < count;
        i++
    ) {

        if (i > 0) {

            json += ",";
        }

        uint8_t index =
            (
                start + i
            ) %
            TELEMETRY_HISTORY_SIZE;

        TelemetrySample& sample =
            appState.telemetryHistory[
                index
            ];

        json += "{";

        json +=
            "\"time\":" +
            String(
                sample.timestamp
            ) +
            ",";

        json +=
            "\"ping\":" +
            String(
                sample.ping
            ) +
            ",";

        json +=
            "\"rssi\":" +
            String(
                sample.rssi
            ) +
            ",";

        json +=
            "\"loss\":" +
            String(
                sample.packetLoss
            ) +
            ",";

        json +=
            "\"dns\":" +
            String(
                sample.dnsTime
            ) +
            ",";

        json +=
            "\"heap\":" +
            String(
                sample.heap
            ) +
            ",";

        json +=
            "\"state\":" +
            String(
                (unsigned)sample.stateFlags
            );

        json += "}";
    }

    json += "]}";

    return json;
}

// ============================================================
// EVENT LOG
// ============================================================

void SentinelWeb::recordEvent(
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

    // Print the event to Serial so the user can debug
    Serial.print("[EVENT] ");
    if (severity == EVENT_WARNING) Serial.print("WARN ");
    else if (severity == EVENT_CRITICAL) Serial.print("CRIT ");
    else Serial.print("INFO ");
    Serial.println(message);
}

// ============================================================
// OUTAGE DETECTION
// ============================================================

void SentinelWeb::detectOutages() {

    uint32_t now = millis();

    // -------------------------------------------------------
    // INTERNET
    // -------------------------------------------------------

    if (
        appState.network.internetWasOnline &&
        !appState.network.internetOnline
    ) {

        appState.network.lastInternetDown = now;

        recordEvent(
            EVENT_WARNING,
            "Internet unavailable"
        );
    }

    if (
        !appState.network.internetWasOnline &&
        appState.network.internetOnline
    ) {

        appState.network.lastInternetUp = now;

        recordEvent(
            EVENT_INFO,
            "Internet restored"
        );
    }

    appState.network.internetWasOnline =
        appState.network.internetOnline;

    // -------------------------------------------------------
    // GATEWAY
    // -------------------------------------------------------

    if (
        appState.network.gatewayWasOnline &&
        !appState.network.gatewayOnline
    ) {

        appState.network.lastGatewayDown = now;

        recordEvent(
            EVENT_WARNING,
            "Gateway unreachable"
        );
    }

    if (
        !appState.network.gatewayWasOnline &&
        appState.network.gatewayOnline
    ) {

        appState.network.lastGatewayUp = now;

        recordEvent(
            EVENT_INFO,
            "Gateway reachable"
        );
    }

    appState.network.gatewayWasOnline =
        appState.network.gatewayOnline;

    // -------------------------------------------------------
    // DNS
    // -------------------------------------------------------

    if (
        appState.network.dnsWasOnline &&
        !appState.network.dnsOnline
    ) {

        appState.network.lastDnsDown = now;

        recordEvent(
            EVENT_WARNING,
            "DNS failure"
        );
    }

    if (
        !appState.network.dnsWasOnline &&
        appState.network.dnsOnline
    ) {

        appState.network.lastDnsUp = now;

        recordEvent(
            EVENT_INFO,
            "DNS restored"
        );
    }

    appState.network.dnsWasOnline =
        appState.network.dnsOnline;
}

// ============================================================
// SPIKE DETECTION
// ============================================================

void SentinelWeb::detectSpikes() {

    // Skip the first sample to establish a baseline
    if (appState.telemetryHistoryCount == 0) {

        appState.lastPing = appState.network.internetPing;
        appState.lastRssi = appState.network.rssi;
        appState.lastLoss = appState.network.packetLoss;
        return;
    }

    // -------------------------------------------------------
    // Ping spike: jumped by > 100ms in one sample
    // -------------------------------------------------------

    if (
        appState.lastPing >= 0 &&
        appState.network.internetPing >= 0 &&
        appState.network.internetPing -
            appState.lastPing > 100.0f
    ) {

        appState.network.pingSpikes++;

        if (
            appState.network.pingSpikes < 5
        ) {
            recordEvent(
                EVENT_WARNING,
                "Latency spike: " +
                    String(
                        (int)appState.network.internetPing
                    ) + " ms"
            );
        }
    }

    // -------------------------------------------------------
    // Loss spike: jumped by > 20%
    // -------------------------------------------------------

    if (
        appState.network.packetLoss >
            appState.lastLoss + 20
    ) {

        appState.network.lossSpikes++;

        if (
            appState.network.lossSpikes < 5
        ) {
            recordEvent(
                EVENT_WARNING,
                "Packet loss spike: " +
                    String(
                        appState.network.packetLoss
                    ) + "%"
            );
        }
    }

    // -------------------------------------------------------
    // RSSI drop: dropped by > 10 dBm
    // -------------------------------------------------------

    if (
        appState.lastRssi != 0 &&
        appState.network.rssi <
            appState.lastRssi - 10
    ) {

        appState.network.rssiDrops++;

        if (
            appState.network.rssiDrops < 5
        ) {
            recordEvent(
                EVENT_WARNING,
                "RSSI drop: " +
                    String(
                        appState.network.rssi
                    ) + " dBm"
            );
        }
    }

    appState.lastPing = appState.network.internetPing;
    appState.lastRssi = appState.network.rssi;
    appState.lastLoss = appState.network.packetLoss;
}

// ============================================================
// HEALTH SCORE
// ============================================================

void SentinelWeb::computeHealthScore() {

    // -------------------------------------------------------
    // Stability score (0-100): based on recent spike counts.
    // Each spike subtracts 5 points.
    // -------------------------------------------------------

    uint16_t totalSpikes =
        appState.network.pingSpikes +
        appState.network.lossSpikes +
        appState.network.rssiDrops;

    int stability = 100 - (int)min(totalSpikes, (uint16_t)100) / 2;

    if (stability < 0) stability = 0;

    appState.network.stabilityScore =
        (uint8_t)stability;

    // -------------------------------------------------------
    // Health score (0-100): combination of
    //   - WiFi connectivity (40)
    //   - Internet reachability (30)
    //   - DNS reachability (15)
    //   - Loss-free (10)
    //   - Latency under threshold (5)
    // -------------------------------------------------------

    int health = 0;

    if (appState.network.wifiConnected)
        health += 40;

    if (appState.network.internetOnline)
        health += 30;

    if (appState.network.dnsOnline)
        health += 15;

    if (appState.network.packetLoss == 0)
        health += 10;

    if (
        appState.network.internetPing > 0 &&
        appState.network.internetPing < 100
    )
        health += 5;

    appState.network.healthScore =
        (uint8_t)health;
}

// ============================================================
// EVENTS JSON / BROADCAST
// ============================================================

String SentinelWeb::createEventsJSON() {

    String json = "{";

    json += "\"type\":\"events\",";
    json += "\"count\":" +
        String(appState.eventCount) +
        ",";
    json += "\"events\":[";

    for (uint8_t i = 0; i < appState.eventCount; i++) {

        // Oldest first
        uint8_t idx =
            (appState.eventHead +
                MAX_EVENTS -
                appState.eventCount +
                i
            ) % MAX_EVENTS;

        EventEntry& e = appState.events[idx];

        if (i > 0) json += ",";

        const char* sev = "INFO";
        if (e.severity == EVENT_WARNING) sev = "WARNING";
        else if (e.severity == EVENT_CRITICAL) sev = "CRITICAL";

        json += "{";
        json += "\"time\":" + String(e.timestamp) + ",";
        json += "\"severity\":\"" + String(sev) + "\",";
        json += "\"message\":\"" + e.message + "\"";
        json += "}";
    }

    json += "]}";

    return json;
}

void SentinelWeb::broadcastEvents() {

    if (webSocket.connectedClients() == 0) {
        return;
    }

    String events = createEventsJSON();

    webSocket.broadcastTXT(events);
}

void SentinelWeb::handleEvents() {

    server.send(
        200,
        "application/json",
        createEventsJSON()
    );
}


// ============================================================
// LAN JSON
// ============================================================

String SentinelWeb::createLANJSON() {

    auto& scanner =
        appState.network.lanScanner;

    String json = "{";

    json += "\"type\":\"lan\",";

    json +=
        "\"localIP\":\"" +
        appState.network.localIP.toString() +
        "\",";

    json +=
        "\"gateway\":\"" +
        appState.network.gatewayIP.toString() +
        "\",";

    json +=
        "\"subnetMask\":\"" +
        appState.network.subnetMask.toString() +
        "\",";

    json +=
        "\"subnetStart\":\"" +
        scanner.subnetStart.toString() +
        "\",";

    json +=
        "\"subnetEnd\":\"" +
        scanner.subnetEnd.toString() +
        "\",";

    json +=
        "\"scanning\":" +
        String(
            scanner.scanning
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"scanComplete\":" +
        String(
            scanner.scanComplete
                ? "true"
                : "false"
        ) +
        ",";

    json +=
        "\"scannedHosts\":" +
        String(scanner.scannedHosts) +
        ",";

    json +=
        "\"totalHosts\":" +
        String(scanner.totalHosts) +
        ",";

    json +=
        "\"deviceCount\":" +
        String(scanner.deviceCount) +
        ",";

    json +=
        "\"autoScanning\":" +
        String(
            scanner.autoScanning
                ? "true"
                : "false"
        ) +
        ",";

    uint32_t timeToNext = 0;
    if (!scanner.scanning) {
        int32_t elapsed = millis() - scanner.lastScan;
        if (elapsed < LAN_SCAN_INTERVAL_MS) {
            timeToNext = LAN_SCAN_INTERVAL_MS - elapsed;
        }
    }
    json +=
        "\"timeToNextScan\":" +
        String(timeToNext) +
        ",";

    json += "\"devices\":[";

    for (
        int i = 0;
        i < scanner.deviceCount;
        i++
    ) {

        if (i > 0) {
            json += ",";
        }

        LANDevice& device =
            scanner.devices[i];

        json += "{";

        json +=
            "\"ip\":\"" +
            device.ip.toString() +
            "\",";

        json +=
            "\"hostname\":\"" +
            device.hostname +
            "\",";

        json +=
            "\"mac\":\"" +
            device.mac +
            "\",";

        json +=
            "\"online\":" +
            String(
                device.online
                    ? "true"
                    : "false"
            ) +
            ",";

        json +=
            "\"latency\":" +
            String(
                device.latency >= 0
                    ? (int)device.latency
                    : -1
            );

        json += "}";
    }

    json += "]}";

    return json;
}
