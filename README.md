You are working on my existing ESP32-S3 Arduino project called:

NetworkSentinel

Project path:

C:\home\pcp\projects\IoT\ESP\NetworkSentinel

IMPORTANT:
This is an EXISTING working project.

DO NOT rewrite the project from scratch.

DO NOT replace working modules unnecessarily.

DO NOT invent structures, fields, functions, pins, or APIs when the existing code already defines them.

Your first job is to inspect the entire project and understand the current architecture.

============================================================
PROJECT GOAL
============================================================

NetworkSentinel is an ESP32-S3 based network monitoring device.

Hardware currently includes:

- ESP32-S3
- 16x1 LCD
- Analog joystick with push button

The joystick is already working and has been optimized so movement occurs mainly at directional extremes rather than reacting excessively to small analog fluctuations.

The Wi-Fi scanner is already implemented.

The local UI is already implemented.

A browser-based web UI is already implemented and currently RUNNING.

The current web UI is served directly by the ESP32.

The project already has a SentinelWeb module.

============================================================
CURRENT WORKING STATE
============================================================

The following has already been completed and MUST NOT be broken:

1. ESP32-S3 boots correctly.

2. Wi-Fi connection works.

3. Wi-Fi scanning works.

4. Joystick works.

5. LCD/local UI works.

6. NetworkManager works.

7. AppState works.

8. Browser web UI works.

9. REST API works.

10. SentinelWeb works.

11. The ESP32 WebServer library is already being used.

12. The WebSockets library by Markus Sattler is installed.

The project previously had a naming collision because the project contained:

WebServer.h
WebServer.cpp

which conflicted with the ESP32 WebServer library.

This has already been resolved.

The project now uses:

SentinelWeb.h
SentinelWeb.cpp

and inside SentinelWeb.h the ESP32 library is included as:

#include <WebServer.h>

DO NOT recreate WebServer.h or WebServer.cpp.

============================================================
CURRENT PROJECT STRUCTURE
============================================================

First inspect the actual directory.

Expected structure is approximately:

NetworkSentinel/
│
├── NetworkSentinel.ino
│
├── AppState.h
├── AppState.cpp
│
├── NetworkManager.h
├── NetworkManager.cpp
│
├── Joystick.h
├── Joystick.cpp
│
├── OLED.h
├── OLED.cpp
│
├── Menu.h
├── Menu.cpp
│
├── SentinelWeb.h
├── SentinelWeb.cpp
│
└── other existing files

The actual project may contain additional files.

Inspect them.

============================================================
PHASE 5.2 OBJECTIVE
============================================================

Implement a robust LIVE TELEMETRY and NETWORK HISTORY ENGINE.

The browser dashboard should receive live network telemetry through WebSockets.

The ESP32 should retain a fixed-size RAM history buffer.

The browser should display live graphs.

Metrics:

- Internet ping
- Wi-Fi RSSI
- Packet loss
- DNS latency
- Wi-Fi/network status

The system must remain responsive.

The joystick, LCD, Wi-Fi scanner, NetworkManager, REST API and WebSocket server must continue working.

============================================================
IMPORTANT ARCHITECTURE
============================================================

The architecture should be:

                       ESP32-S3
                           │
             ┌─────────────┼─────────────┐
             │             │             │
        NetworkManager   Wi-Fi Scanner   System
             │             │             │
             └─────────────┼─────────────┘
                           │
                        AppState
                           │
                  ┌────────┴────────┐
                  │                 │
                REST            WebSocket
                  │                 │
                  └────────┬────────┘
                           │
                       Browser
                           │
              ┌────────────┼────────────┐
              │            │            │
          Dashboard       WiFi         LAN
              │
           Graphs
              │
      Ping/RSSI/Loss/DNS


AppState must remain the SINGLE SOURCE OF TRUTH.

Do not duplicate network state unnecessarily.

============================================================
STEP 1 — INSPECT EXISTING CODE
============================================================

Before editing anything:

Inspect:

- NetworkSentinel.ino
- AppState.h
- AppState.cpp
- NetworkManager.h
- NetworkManager.cpp
- SentinelWeb.h
- SentinelWeb.cpp
- Joystick files
- OLED/LCD files
- Menu files
- config files

Understand:

- exact NetworkState structure
- exact analyzer structure
- exact telemetry fields already available
- exact Wi-Fi scanner fields
- exact setup() sequence
- exact loop() sequence
- existing FreeRTOS tasks if any
- existing mutexes if any
- existing timers
- existing WebServer routes
- existing WebSocket implementation

DO NOT make assumptions.

============================================================
STEP 2 — VERIFY WEBSOCKET IMPLEMENTATION
============================================================

The project should use:

#include <WebSocketsServer.h>

and:

WebSocketsServer webSocket{81};

HTTP server:

WebServer server{80};

WebSocket:

port 81

HTTP:

port 80

The WebSocket library installed is:

WebSockets by Markus Sattler

IMPORTANT:

This version of the library expects String references.

For example:

WRONG:

webSocket.broadcastTXT(
    createTelemetryJSON()
);

RIGHT:

String telemetry = createTelemetryJSON();

webSocket.broadcastTXT(
    telemetry
);

Likewise:

WRONG:

webSocket.sendTXT(
    clientNum,
    createTelemetryJSON()
);

RIGHT:

String telemetry =
    createTelemetryJSON();

webSocket.sendTXT(
    clientNum,
    telemetry
);

Use actual String variables whenever the library requires String&.

============================================================
STEP 3 — PRESERVE EXISTING REST API
============================================================

DO NOT remove:

/api

DO NOT remove:

/api/wifi

The REST API should remain functional.

It is useful for:

- initial page load
- debugging
- external clients
- future integrations

WebSocket is an addition, not a replacement.

============================================================
STEP 4 — WEBSOCKET TELEMETRY
============================================================

The WebSocket server must:

- accept browser connections
- detect connect
- detect disconnect
- handle incoming text messages
- broadcast telemetry
- broadcast Wi-Fi state

WebSocket endpoint:

ws://ESP32_IP:81/

On connection:

1. Print client number to Serial.
2. Send current telemetry.
3. Send current Wi-Fi analyzer state.
4. Send current history buffer.

Do not block while doing this.

============================================================
STEP 5 — TELEMETRY MESSAGE
============================================================

Use JSON.

The exact field values must come from the EXISTING AppState.

Preferred message:

{
    "type": "telemetry",
    "wifi": true,
    "gateway": true,
    "internet": true,
    "dns": true,
    "rssi": -48,
    "channel": 6,
    "ping": 21,
    "packetLoss": 0,
    "dnsTime": 14,
    "heap": 180000,
    "uptime": 123456
}

IMPORTANT:

If one of these metrics does not currently exist in AppState, DO NOT invent a fake value.

Instead:

1. Inspect NetworkManager.
2. Determine whether the metric is already calculated elsewhere.
3. If it is already available, use it.
4. If it is not available, implement the smallest appropriate measurement in NetworkManager.
5. Keep measurement logic in NetworkManager rather than SentinelWeb whenever possible.

Do not fabricate network measurements.

============================================================
STEP 6 — FIXED-SIZE HISTORY BUFFER
============================================================

Implement a fixed-size circular RAM buffer.

Target:

60 samples.

One sample per second.

Structure:

TelemetrySample

containing:

- timestamp
- ping
- rssi
- packetLoss
- dnsTime

Use the actual types appropriate for the existing project.

For example:

struct TelemetrySample {

    uint32_t timestamp;

    float ping;

    int rssi;

    uint8_t packetLoss;

    uint32_t dnsTime;
};

But adapt types if existing fields use different types.

The buffer must be:

TELEMETRY_HISTORY_SIZE = 60

Do not use a dynamically growing array.

Do not allocate memory continuously.

Do not store one sample per second in flash.

DO NOT use:

EEPROM
SPIFFS
LittleFS

for per-second telemetry history.

RAM only.

============================================================
STEP 7 — CIRCULAR BUFFER LOGIC
============================================================

Maintain:

- history array
- current index
- sample count

Behavior:

Initially:

count = 0

Each new sample:

write to current index

increment index

wrap index at 60

increment count until 60

Once full:

new samples overwrite oldest samples.

Example:

0
1
2
...
59
0
1
2
...

The browser must receive samples in chronological order.

============================================================
STEP 8 — RECORDING FREQUENCY
============================================================

Record telemetry once every:

1000 ms

Do NOT use delay(1000).

Use millis()-based scheduling.

Existing SentinelWeb::update() must remain non-blocking.

For example:

if (
    now - lastTelemetryTime >= 1000
) {

    lastTelemetryTime = now;

    recordTelemetry();

    broadcastTelemetry();
}

Adapt this to the existing architecture.

============================================================
STEP 9 — HISTORY JSON
============================================================

Implement:

createHistoryJSON()

Output:

{
    "type": "history",
    "samples": [
        {
            "time": 1000,
            "ping": 20,
            "rssi": -45,
            "loss": 0,
            "dns": 15
        },
        ...
    ]
}

Send history:

- immediately when a WebSocket client connects
- periodically if useful

Do not send excessive amounts of data unnecessarily.

Recommended:

full history on connection

then live telemetry every second.

If periodic history is useful, send it no more frequently than every 5 seconds.

============================================================
STEP 10 — WIFI WEBSOCKET MESSAGE
============================================================

Implement:

type = "wifi"

Example:

{
    "type": "wifi",
    "count": 12,
    "averageRSSI": -65,
    "open": 1,
    "busiestChannel": 6,
    "congestion": 42,
    "scanning": false
}

Use the existing:

appState.network.analyzer

structure.

IMPORTANT:

Earlier versions used old fields:

wifiScanRunning
wifiNetworkCount

Those fields are NO LONGER valid.

The current architecture uses:

appState.network.analyzer.scanning

and:

appState.network.analyzer.networkCount

Use the actual current project definitions after inspecting them.

============================================================
STEP 11 — WEB DASHBOARD
============================================================

Keep the existing dashboard styling.

Do not redesign the entire page.

Add live graphs.

Graphs:

1. Ping
2. RSSI
3. Packet loss
4. DNS latency

Dashboard should show:

--------------------------------------------

NETWORK DASHBOARD

PING
23 ms

RSSI
-48 dBm

PACKET LOSS
0 %

CHANNEL
6

--------------------------------------------

PING GRAPH

--------------------------------------------

RSSI GRAPH

--------------------------------------------

PACKET LOSS GRAPH

--------------------------------------------

DNS LATENCY GRAPH

--------------------------------------------

The graphs must update without refreshing the page.

============================================================
STEP 12 — DO NOT ADD HEAVY JAVASCRIPT DEPENDENCIES
============================================================

The web page is served directly by the ESP32.

Avoid downloading large JavaScript libraries.

Do NOT add:

React
Vue
Angular
Chart.js
D3
etc.

unless absolutely necessary.

Prefer a lightweight HTML5 Canvas implementation.

Use:

<canvas>

and native JavaScript.

This minimizes:

- flash usage
- RAM usage
- network transfer
- browser loading time

============================================================
STEP 13 — CANVAS GRAPH ENGINE
============================================================

Implement a reusable function:

drawGraph(
    canvasId,
    values,
    minValue,
    maxValue,
    unit
)

It should:

- resize according to container
- support devicePixelRatio
- clear canvas
- draw grid
- draw graph line
- display current value
- gracefully handle fewer than 2 samples

Do not hardcode graph colors unless the existing UI already has a color scheme.

Use the existing CSS theme.

============================================================
STEP 14 — BROWSER HISTORY
============================================================

Maintain a browser-side array:

history

Maximum:

60 samples.

When a history message arrives:

clear current browser history

load received samples

draw graphs

When live telemetry arrives:

add new sample

remove oldest sample when > 60

redraw graphs

Do not allow unlimited browser-side history.

============================================================
STEP 15 — WEBSOCKET CONNECTION
============================================================

Browser must connect automatically.

Use:

const protocol =
    location.protocol === "https:"
        ? "wss:"
        : "ws:";

const socket =
    new WebSocket(
        protocol +
        "//" +
        location.hostname +
        ":81/"
    );

Handle:

onopen
onmessage
onclose
onerror

On disconnect:

show:

RECONNECTING

Then attempt reconnection after approximately 2 seconds.

Do not create multiple simultaneous WebSocket connections.

============================================================
STEP 16 — MESSAGE TYPES
============================================================

The browser must support:

"type": "telemetry"

"type": "wifi"

"type": "history"

Future messages should be easy to add.

Structure JavaScript message handling cleanly:

if (
    data.type === "telemetry"
) {

    updateLiveTelemetry(data);

}

else if (
    data.type === "wifi"
) {

    updateLiveWiFi(data);

}

else if (
    data.type === "history"
) {

    updateHistory(data);

}

============================================================
STEP 17 — KEEP REST FALLBACK
============================================================

The existing REST refresh should remain available.

However, once WebSocket connects successfully, the browser should use WebSocket for live updates.

REST can be used for:

- initial page population
- fallback
- debugging

Do not run REST polling every second if WebSocket is connected.

This avoids unnecessary traffic.

============================================================
STEP 18 — RESPONSIVENESS
============================================================

This is extremely important.

The ESP32 must remain responsive to:

- joystick
- LCD
- Wi-Fi scanner
- web server
- WebSocket
- NetworkManager

Avoid:

delay(1000)

Avoid long blocking loops.

Avoid scanning or pinging inside WebSocket callbacks.

Avoid large dynamic String allocations every loop if possible.

Use millis()-based scheduling.

============================================================
STEP 19 — MEMORY SAFETY
============================================================

ESP32-S3 has limited resources compared with a PC.

Avoid:

- unbounded String growth
- dynamic arrays that grow continuously
- storing unlimited history
- flash writes every second
- huge JSON responses
- unnecessary copies

Monitor:

ESP.getFreeHeap()

If useful, expose it through telemetry.

============================================================
STEP 20 — WEB SOCKET CALLBACK SAFETY
============================================================

WebSocket callbacks must be lightweight.

For:

WStype_CONNECTED

only:

- log connection
- send current state
- send history

For:

WStype_DISCONNECTED

log it.

For:

WStype_TEXT

do not execute heavy operations.

If commands are later added, queue them for the main application logic rather than performing long operations inside the callback.

============================================================
STEP 21 — COMPATIBILITY WITH CURRENT APPSTATE
============================================================

This is critical.

Before editing:

read AppState.h completely.

Determine exact definitions of:

NetworkState
WiFi analyzer state
network arrays
RSSI
ping
packet loss
DNS
gateway
internet status
timestamps

Use those exact names.

Do not assume fields.

For example, if the actual code contains:

appState.network.internetPing

use that.

If it contains:

appState.network.ping

use that.

If a field doesn't exist:

do not simply add a duplicate field.

First determine where that information is currently calculated.

============================================================
STEP 22 — COMPATIBILITY WITH NETWORKMANAGER
============================================================

Inspect NetworkManager.cpp.

Determine:

- how Wi-Fi scanning works
- how ping works
- how gateway detection works
- how internet detection works
- how DNS testing works
- how analyzer results are stored

Do not duplicate these calculations in SentinelWeb.

SentinelWeb should primarily:

- expose existing state
- serialize it
- send it to browser

NetworkManager should own network measurements.

============================================================
STEP 23 — WIFI SCANNER MUST NOT BREAK
============================================================

Existing Wi-Fi scanner must continue working.

The following must remain functional:

- scan start
- scan progress
- scan completion
- AP count
- SSID
- BSSID
- RSSI
- channel
- encryption/security
- analyzer statistics

WebSocket must not interfere with scanning.

If scanning is asynchronous, do not block it.

============================================================
STEP 24 — SERIAL DEBUG OUTPUT
============================================================

Add useful but restrained debug output.

On WebSocket connection:

WebSocket client connected: X

On disconnect:

WebSocket client disconnected: X

On server start:

================================
NETWORK SENTINEL WEB
================================

HTTP:
http://ESP32_IP

WebSocket:
ws://ESP32_IP:81

================================

Do NOT print telemetry JSON every second to Serial.

That would flood the serial monitor.

============================================================
STEP 25 — WEB SERVER PORTS
============================================================

HTTP:

80

WebSocket:

81

Do not move the ports unless there is a real conflict.

============================================================
STEP 26 — HTML JAVASCRIPT
============================================================

The current page already contains navigation:

Dashboard
WiFi
LAN
Internet
System

Do not remove those.

Keep existing pages.

The current LAN page may say:

LAN scanner coming next.

That is okay.

Do not implement LAN discovery during this phase.

That is the NEXT phase.

============================================================
STEP 27 — DASHBOARD METRICS
============================================================

Dashboard should update live:

Ping

RSSI

Packet loss

Channel

SSID

IP

Gateway

DNS

WiFi status

Gateway status

Internet status

DNS status

Free heap

CPU frequency

Uptime

Use actual existing data.

============================================================
STEP 28 — WIFI ANALYZER
============================================================

Dashboard Wi-Fi analyzer should update:

Network count

Average RSSI

Open networks

Busiest channel

Congestion

Scanning state

The existing AP table should remain functional.

Do not remove the AP table.

============================================================
STEP 29 — GRAPH BEHAVIOR
============================================================

Ping:

range approximately:

0–200 ms

But dynamically handle values outside this range.

RSSI:

approximately:

-100 to -20 dBm

Packet loss:

0–100 %

DNS:

approximately:

0–500 ms

If a metric is unavailable:

do not graph fake data.

Represent unavailable data gracefully.

============================================================
STEP 30 — ERROR HANDLING
============================================================

If WebSocket connection fails:

browser should show:

RECONNECTING

If REST works but WebSocket doesn't:

dashboard should still show current values using REST.

If Wi-Fi is disconnected:

show:

OFFLINE

If ping is unavailable:

show:

--

Do not crash.

============================================================
STEP 31 — BROWSER RESPONSIVENESS
============================================================

Existing responsive CSS should remain.

Graphs should resize correctly.

Use:

canvas.clientWidth

and devicePixelRatio.

Handle:

window.resize

and redraw graphs.

============================================================
STEP 32 — IMPLEMENTATION ORDER
============================================================

Do NOT edit everything at once.

Follow this sequence:

1. Inspect files.
2. Report exact current architecture briefly.
3. Modify AppState only if necessary.
4. Add telemetry history buffer.
5. Add recordTelemetry().
6. Add WebSocket telemetry.
7. Add history JSON.
8. Add WebSocket history.
9. Add Wi-Fi WebSocket messages.
10. Modify browser JavaScript.
11. Add Canvas graphs.
12. Compile.
13. Fix compile errors.
14. Review for blocking code.
15. Final compile verification.

============================================================
STEP 33 — IMPORTANT C++ RULE
============================================================

The installed WebSockets library uses:

String&

Therefore NEVER pass temporary String return values directly to:

sendTXT()

or:

broadcastTXT()

Wrong:

webSocket.broadcastTXT(
    createTelemetryJSON()
);

Correct:

String telemetry =
    createTelemetryJSON();

webSocket.broadcastTXT(
    telemetry
);

Likewise:

String history =
    createHistoryJSON();

webSocket.broadcastTXT(
    history
);

============================================================
STEP 34 — AVOID NAME COLLISIONS
============================================================

Do NOT create:

WebServer.h
WebServer.cpp

Our web module is:

SentinelWeb.h
SentinelWeb.cpp

The ESP32 library is:

#include <WebServer.h>

Keep these separate.

============================================================
STEP 35 — BUILD VERIFICATION
============================================================

After modifications:

compile the project.

If compilation fails:

read the error carefully.

Fix the actual architecture mismatch.

Do NOT randomly rename classes.

Do NOT create duplicate files.

Do NOT create duplicate definitions.

Do NOT remove working modules.

============================================================
STEP 36 — CHECK FOR DUPLICATE FILES
============================================================

Before finalizing, verify there is only:

SentinelWeb.cpp

and not:

WebServer.cpp

Verify there is only:

SentinelWeb.h

for our project web interface.

There must be no duplicate implementation of:

SentinelWeb::begin()
SentinelWeb::update()
SentinelWeb::setupRoutes()
SentinelWeb::handleRoot()
SentinelWeb::handleAPI()
SentinelWeb::handleWiFi()
SentinelWeb::createJSON()
SentinelWeb::createWiFiJSON()
SentinelWeb::createTelemetryJSON()

============================================================
STEP 37 — FINAL FUNCTIONAL TEST
============================================================

After compiling successfully:

Upload to ESP32-S3.

Open Serial Monitor.

Expected approximately:

================================
NETWORK SENTINEL WEB
================================
HTTP:      http://192.168.x.x
WebSocket: ws://192.168.x.x:81
================================

Then open:

http://192.168.x.x

from a device connected to the same Wi-Fi network.

Browser console should show:

WebSocket connected

Dashboard should update without refresh.

Graphs should begin populating.

============================================================
STEP 38 — SUCCESS CRITERIA
============================================================

Phase 5.2 is complete only if ALL of these work:

[ ] Project compiles
[ ] ESP32 boots
[ ] Wi-Fi connects
[ ] Wi-Fi scanner works
[ ] Joystick works
[ ] LCD works
[ ] HTTP web UI works
[ ] REST /api works
[ ] REST /api/wifi works
[ ] WebSocket starts
[ ] Browser connects through WebSocket
[ ] Live telemetry updates
[ ] Wi-Fi statistics update
[ ] History buffer records samples
[ ] History remains capped at 60 samples
[ ] Graphs render
[ ] Graphs update live
[ ] Browser reconnects after WebSocket disconnect
[ ] No 1-second blocking delay
[ ] No unbounded memory growth
[ ] No duplicate WebServer/SentinelWeb files
[ ] No duplicate SentinelWeb definitions

============================================================
DO NOT IMPLEMENT YET
============================================================

Do NOT implement:

- LAN scanning
- ARP scanning
- device fingerprinting
- port scanning
- vulnerability scanning
- packet sniffing
- deauthentication
- Wi-Fi attacks
- credential capture
- packet injection
- captive portals
- promiscuous-mode attacks

Those are outside this phase.

============================================================
NEXT PHASE
============================================================

After Phase 5.2 is successfully compiled and tested, STOP.

Do not automatically implement the next phase.

The next phase will be:

PHASE 6 — LAN DISCOVERY

Planned features:

- local subnet detection
- device discovery
- device IP
- MAC when legitimately available
- hostname where available
- response latency
- device status
- device list in browser
- device count
- gateway identification
- live device events

But DO NOT implement Phase 6 now.

============================================================
FINAL INSTRUCTION
============================================================

Work on the EXISTING NetworkSentinel project.

Inspect first.

Preserve working functionality.

Adapt to the actual codebase.

Do not assume structure.

Do not rewrite unrelated code.

Do not create duplicate files.

Do not create duplicate classes.

Do not create duplicate global objects.

Keep AppState as the source of truth.

Keep NetworkManager responsible for measurements.

Keep SentinelWeb responsible for HTTP/WebSocket transport and serialization.

Keep browser JavaScript lightweight.

Use RAM-only fixed-size telemetry history.

Use WebSocket for live updates.

Use REST as fallback.

Compile before declaring completion.

If compilation fails, fix it before moving forward.

At the end, provide:

1. Files changed
2. What was added
3. Any fields added to AppState
4. WebSocket endpoint
5. REST endpoints preserved
6. How telemetry works
7. How history works
8. How browser graphs work
9. Memory considerations
10. Compilation result
11. Upload/testing instructions
12. Any remaining issues

Do not proceed to LAN discovery until I explicitly tell you to.