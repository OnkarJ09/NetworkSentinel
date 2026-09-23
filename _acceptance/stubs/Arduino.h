// Minimal Arduino.h stub for g++ -fsyntax-only validation.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <algorithm>
#include <cstdio>
#include <string>

typedef bool boolean;
typedef uint8_t byte;

class String {
public:
    String() {}
    String(const char* v) : s(v ? v : "") {}
    String(const String& v) : s(v.s) {}
    String(int v) : s(std::to_string(v)) {}
    String(unsigned int v) : s(std::to_string(v)) {}
    String(long v) : s(std::to_string(v)) {}
    String(unsigned long v) : s(std::to_string(v)) {}
    String(unsigned long long v) : s(std::to_string(v)) {}
    String(float v, int = 2) : s(std::to_string(v)) {}
    String(double v, int = 2) : s(std::to_string(v)) {}
    String& operator=(const String& v) { s = v.s; return *this; }
    String& operator=(const char* v) { s = v ? v : ""; return *this; }
    String& operator+=(const String& v) { s += v.s; return *this; }
    String& operator+=(const char* v) { if (v) s += v; return *this; }
    String& operator+=(int v) { s += std::to_string(v); return *this; }
    String& operator+=(unsigned int v) { s += std::to_string(v); return *this; }
    String& operator+=(long v) { s += std::to_string(v); return *this; }
    String& operator+=(unsigned long v) { s += std::to_string(v); return *this; }
    String& operator+=(float v) { s += std::to_string(v); return *this; }
    String& operator+=(double v) { s += std::to_string(v); return *this; }
    String operator+(const String& v) const { String r(*this); r.s += v.s; return r; }
    String operator+(const char* v) const { String r(*this); if (v) r.s += v; return r; }
    String operator+(int v) const { return *this + String(v); }
    friend String operator+(const char* lhs, const String& rhs) { return String(lhs) + rhs; }
    bool operator==(const char* v) const { return s == (v ? v : ""); }
    bool operator!=(const char* v) const { return !(*this == v); }
    bool operator==(const String& v) const { return s == v.s; }
    bool operator!=(const String& v) const { return s != v.s; }
    operator const char*() const { return s.c_str(); }
    char charAt(unsigned int i) const { return i < s.size() ? s[i] : 0; }
    int length() const { return (int)s.size(); }
    String substring(unsigned int a) const { return a < s.size() ? String(s.substr(a).c_str()) : String(); }
    String substring(unsigned int a, unsigned int b) const {
        if (a >= s.size() || b <= a) return String();
        return String(s.substr(a, b - a).c_str());
    }
    int indexOf(char c) const { auto p = s.find(c); return p == std::string::npos ? -1 : (int)p; }
    int indexOf(const char* c) const { auto p = s.find(c); return p == std::string::npos ? -1 : (int)p; }
    bool startsWith(const char* c) const { return s.rfind(c, 0) == 0; }
    void trim() {
        auto ws = [](char c){ return c==' '||c=='\t'||c=='\r'||c=='\n'; };
        while (!s.empty() && ws(s.front())) s.erase(s.begin());
        while (!s.empty() && ws(s.back())) s.pop_back();
    }
    const char* c_str() const { return s.c_str(); }
    int toInt() const { return atoi(s.c_str()); }
    float toFloat() const { return (float)atof(s.c_str()); }
private:
    std::string s;
};

class IPAddress {
public:
    IPAddress() {}
    IPAddress(uint8_t, uint8_t, uint8_t, uint8_t) {}
    IPAddress(const IPAddress&) {}
    String toString() const { return String(); }
    operator uint32_t() const { return 0; }
    uint8_t operator[](int) const { return 0; }
    uint8_t& operator[](int) { static uint8_t v; return v; }
};

inline unsigned long millis() { return 0; }
inline void delay(unsigned long) {}
inline void delayMicroseconds(unsigned int) {}
inline long random(long) { return 0; }
inline long random(long, long) { return 0; }

#define PROGMEM
#define F(x) x
#define pgm_read_byte(x) 0
#define pgm_read_word(x) 0
#define pgm_read_dword(x) 0
#define PSTR(x) x
#define snprintf_P snprintf
#define strcpy_P strcpy
#define strlen_P strlen

// ESP logging
#define ESP_LOG_LEVEL_LOCAL 0
#define ESP_LOGE(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGD(...) ((void)0)

// Serial
namespace { struct Serial_t { void println(...){} void printf(...){} void print(...){} void begin(...){} void end(...){} } Serial; }

// FreeRTOS stub
typedef void* TaskHandle_t;
inline int xTaskCreate(void(*)(void*), const char*, uint32_t, void*, uint8_t, TaskHandle_t*) { return 0; }
inline int xTaskCreatePinnedToCore(void(*)(void*), const char*, uint32_t, void*, uint8_t, TaskHandle_t*, int) { return 0; }
inline void vTaskDelay(uint32_t) {}
inline void vTaskDelete(TaskHandle_t) {}
inline uint32_t pdMS_TO_TICKS(uint32_t x) { return x; }

// ESP class stub
class ESPClass {
public:
    uint32_t getFreeHeap() { return 0; }
    uint32_t getMinFreeHeap() { return 0; }
    uint32_t getHeapSize() { return 0; }
    uint32_t getCpuFreqMHz() { return 0; }
    const char* getChipModel() { return "ESP32"; }
    uint8_t getChipRevision() { return 0; }
    uint32_t getFlashChipSize() { return 0; }
    const char* getSdkVersion() { return ""; }
    void restart() {}
    int arduinoVersion() { return 0; }
};
extern ESPClass ESP;
inline uint32_t getCpuFrequencyMhz() { return 0; }

// Reset reason enum stub
typedef int esp_reset_reason_t;
#define ESP_RST_UNKNOWN   0
#define ESP_RST_POWERON   1
#define ESP_RST_EXT       2
#define ESP_RST_SW        3
#define ESP_RST_PANIC     4
#define ESP_RST_INT_WDT   5
#define ESP_RST_TASK_WDT  6
#define ESP_RST_WDT       7
#define ESP_RST_DEEPSLEEP 8
#define ESP_RST_BROWNOUT  9
#define ESP_RST_SDIO      10
inline esp_reset_reason_t esp_reset_reason() { return ESP_RST_POWERON; }

// Arduino constants
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

inline int digitalRead(int) { return HIGH; }
inline void pinMode(int, int) {}
inline int analogRead(int) { return 0; }
inline void analogReadResolution(int) {}

#define WIFI_STA 1
#define WIFI_AP 2
#define WIFI_AP_STA 3

#define HTTP_GET 0
#define HTTP_POST 1
#define HTTP_PUT 2
#define HTTP_DELETE 3
#define HTTP_PATCH 4
#define HTTP_OPTIONS 5
#define HTTP_ANY 6

// ESP32 sys/param.h provides min/max as global functions, not macros.
// We declare them in the global namespace to mimic that.
template <typename T> inline const T& _ns_max(const T& a, const T& b) { return a < b ? b : a; }
template <typename T> inline const T& _ns_min(const T& a, const T& b) { return a < b ? a : b; }
using ::_ns_max;
using ::_ns_min;
// Provide unqualified names so bare `max(...)` works in Menu.cpp.
// We use weak symbols to avoid collisions with std::max.
__attribute__((weak)) int max(int a, int b) { return a > b ? a : b; }
__attribute__((weak)) int min(int a, int b) { return a < b ? a : b; }
