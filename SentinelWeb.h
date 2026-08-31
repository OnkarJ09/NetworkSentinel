#pragma once

#include <Arduino.h>
#include <WebServer.h>

class SentinelWeb {

public:

    void begin();
    void update();

private:

    WebServer server{80};

    void setupRoutes();

    void handleRoot();
    void handleAPI();
    void handleWiFi();
    void handleNotFound();

    String createJSON();
    String createWiFiJSON();
};

extern SentinelWeb sentinelWeb;