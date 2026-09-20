#pragma once
#include "Arduino.h"
#include <functional>

enum WStype_t {
    WStype_DISCONNECTED,
    WStype_CONNECTED,
    WStype_TEXT,
    WStype_BIN,
    WStype_ERROR
};

class WebSocketsServer {
public:
    WebSocketsServer(int) {}
    void begin() {}
    void loop() {}
    void onEvent(void(*)(uint8_t, WStype_t, uint8_t*, size_t)) {}
    void onEvent(std::function<void(uint8_t, WStype_t, uint8_t*, size_t)>) {}
    void sendTXT(uint8_t, const char*) {}
    void sendTXT(uint8_t, const char*, size_t) {}
    void broadcastTXT(const char*) {}
    void broadcastTXT(const char*, size_t) {}
    void disconnect(uint8_t) {}
    int connectedClients() { return 0; }
};
