// Minimal LittleFS stub for g++ validation and host-side parser tests.
#pragma once
#include <Arduino.h>
#include <cstddef>

class File {
public:
    operator bool() const { return false; }
    bool operator!() const { return true; }
    int available() { return 0; }
    String readString() { return String(); }
    String readStringUntil(char) { return String(); }
    size_t print(const String&) { return 0; }
    size_t print(const char*) { return 0; }
    size_t print(char) { return 0; }
    size_t print(int) { return 0; }
    size_t print(unsigned int) { return 0; }
    size_t print(long) { return 0; }
    size_t print(unsigned long) { return 0; }
    size_t println(const String&) { return 0; }
    size_t println(const char*) { return 0; }
    size_t println(int) { return 0; }
    size_t println(unsigned int) { return 0; }
    size_t println(long) { return 0; }
    size_t println(unsigned long) { return 0; }
    void close() {}
};

class FS {
public:
    bool begin(bool = false, const char* = "/littlefs", uint8_t = 0) { return true; }
    void end() {}
    bool exists(const char*) { return false; }
    bool remove(const char*) { return true; }
    File open(const char*, const char* = "r") { return File(); }
    File open(const String&, const char* = "r") { return File(); }
};

inline FS LittleFS;
