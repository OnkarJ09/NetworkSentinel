// Minimal Arduino.h stub for syntax-only compilation of NetworkSentinel .cpp files.
// This DOES NOT exercise runtime behavior; it only lets g++ -fsyntax-only parse
// the code so we can catch typos, missing semicolons, mismatched braces, etc.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

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
    String operator+(const String&) const { return *this; }
    String operator+(const char*) const { return *this; }
    bool operator==(const char*) const { return false; }
    bool operator!=(const char*) const { return false; }
    bool operator==(const String&) const { return false; }
    int length() const { return 0; }
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

inline int snprintf(char*, size_t, const char*, ...) { return 0; }
