#pragma once
#include "Arduino.h"
#include <functional>

class WebServer {
public:
    WebServer(int) {}
    void begin() {}
    void handleClient() {}
    void on(const char*, void(*)()) {}
    void on(const char*, int, void(*)()) {}
    void onNotFound(void(*)()) {}
    void onNotFound(std::function<void()>) {}
    void on(const char*, int, std::function<void()>) {}
    void send(int, const char*, const char*) {}
    void sendHeader(const char*, const char*) {}
    void send_P(int, const char*, const char*, const char*) {}
    void send_P(int, const char*, const char*) {}
    int args() { return 0; }
    String arg(int) { return ""; }
    String arg(const char*) { return ""; }
    bool hasArg(const char*) { return false; }
};
