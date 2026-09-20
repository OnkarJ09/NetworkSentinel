// Minimal Arduino.h stub for g++ -fsyntax-only validation.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <algorithm>
#include <cstdio>

typedef bool boolean;
typedef uint8_t byte;

class String {
public:
    String() {}
    String(const char*) {}
    String(const String&) {}
    String(int) {}
    String(unsigned int) {}
    String(long) {}
    String(unsigned long) {}
    String(unsigned long long) {}
    String(float, int = 2) {}
    String(double, int = 2) {}
    String& operator=(const String&) { return *this; }
    String& operator=(const char*) { return *this; }
    String& operator+=(const String&) { return *this; }
    String& operator+=(const char*) { return *this; }
    String& operator+=(int) { return *this; }
    String& operator+=(unsigned int) { return *this; }
    String& operator+=(long) { return *this; }
    String& operator+=(unsigned long) { return *this; }
    String& operator+=(float) { return *this; }
    String& operator+=(double) { return *this; }
    String operator+(const String&) const { return *this; }
    String operator+(const char*) const { return *this; }
    String operator+(int) const { return *this; }
    friend String operator+(const char* lhs, const String& rhs) { return String(lhs) + rhs; }
    bool operator==(const char*) const { return false; }
    bool operator!=(const char*) const { return false; }
    bool operator==(const String&) const { return false; }
    operator const char*() const { return ""; }
    char charAt(unsigned int) const { return 0; }
    int length() const { return 0; }
    String substring(unsigned int) const { return *this; }
    String substring(unsigned int, unsigned int) const { return *this; }
    const char* c_str() const { return ""; }
    int toInt() const { return 0; }
    float toFloat() const { return 0; }
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
    int getChipModel() { return 0; }
    uint8_t getChipRevision() { return 0; }
    const char* getSdkVersion() { return ""; }
    void restart() {}
    int arduinoVersion() { return 0; }
};
extern ESPClass ESP;
inline uint32_t getCpuFrequencyMhz() { return 0; }

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
