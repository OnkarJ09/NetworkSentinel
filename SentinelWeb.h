#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

class SentinelWeb {

public:

    void begin();

    void update();

private:

    WebServer server{80};

    WebSocketsServer webSocket{81};

    void setupRoutes();

    void setupWebSocket();

    void webSocketEvent(
        uint8_t clientNum,
        WStype_t type,
        uint8_t* payload,
        size_t length
    );

    void broadcastTelemetry();

    void broadcastWiFi();

    void broadcastLAN();

    void handleRoot();

    void handleAPI();

    void handleWiFi();

    void handleLAN();

    void handleLANScan();

    void handleNotFound();

    void recordTelemetry();

    void broadcastHistory();

    String createJSON();

    String createWiFiJSON();

    String createTelemetryJSON();

    String createWiFiEventJSON();

    String createHistoryJSON();

    String createLANJSON();

    uint32_t lastTelemetryBroadcast = 0;

    uint32_t lastWiFiBroadcast = 0;

    uint32_t lastHistoryBroadcast = 0;

    uint32_t lastLANBroadcast = 0;
};

extern SentinelWeb sentinelWeb;