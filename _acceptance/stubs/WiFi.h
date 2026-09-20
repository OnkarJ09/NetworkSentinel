#pragma once
#include "Arduino.h"

enum wifi_auth_mode_t { WIFI_AUTH_OPEN, WIFI_AUTH_WEP, WIFI_AUTH_WPA_PSK, WIFI_AUTH_WPA2_PSK, WIFI_AUTH_WPA_WPA2_PSK, WIFI_AUTH_WPA2_ENTERPRISE, WIFI_AUTH_WPA3_PSK, WIFI_AUTH_WPA2_WPA3_PSK, WIFI_AUTH_WAPI_PSK, WIFI_AUTH_MAX };
enum { WIFI_SCAN_FAILED = -1, WIFI_SCAN_RUNNING = -2 };
enum wl_status_t { WL_NO_SHIELD, WL_IDLE_STATUS, WL_NO_SSID_AVAIL, WL_SCAN_COMPLETED, WL_CONNECTED, WL_CONNECT_FAILED, WL_CONNECTION_LOST, WL_DISCONNECTED };

class WiFiClass {
public:
    int32_t RSSI() { return 0; }
    int8_t getTxPower() { return 0; }
    String macAddress() { return ""; }
    String SSID() { return ""; }
    uint8_t* BSSID() { return nullptr; }
    String BSSIDstr() { return ""; }
    String BSSIDstr(uint8_t) { return ""; }
    int32_t channel() { return 0; }
    int32_t channel(uint8_t) { return 0; }
    IPAddress localIP() { return IPAddress(); }
    IPAddress gatewayIP() { return IPAddress(); }
    IPAddress subnetMask() { return IPAddress(); }
    IPAddress dnsIP() { return IPAddress(); }
    int32_t scanNetworks(bool, bool) { return 0; }
    void scanDelete() {}
    int32_t RSSI(uint8_t) { return 0; }
    String SSID(uint8_t) { return ""; }
    wifi_auth_mode_t encryptionType(uint8_t) { return WIFI_AUTH_OPEN; }
    uint8_t* BSSID(uint8_t) { return nullptr; }
    int hostByName(const char*, IPAddress&) { return 0; }
    int8_t scanComplete() { return 0; }
    bool isConnected() { return false; }
    wl_status_t status() { return wl_status_t{}; }
    void begin(const char*, const char*) {}
    void disconnect() {}
    void mode(int) {}
    void setSleep(bool) {}
    void setTxPower(int8_t) {}
};

extern WiFiClass WiFi;

class WiFiClient {
public:
    WiFiClient() {}
    int connect(IPAddress, uint16_t) { return 0; }
    int connect(const char*, uint16_t) { return 0; }
    int connect(IPAddress, uint16_t, int) { return 0; }
    int connect(const char*, uint16_t, int) { return 0; }
    void stop() {}
    bool connected() { return false; }
    int available() { return 0; }
    int read() { return 0; }
    int read(uint8_t*, int) { return 0; }
    operator bool() { return false; }
};

class PingClass {
public:
    bool ping(IPAddress, uint8_t, uint8_t) { return false; }
};
extern PingClass Ping;
