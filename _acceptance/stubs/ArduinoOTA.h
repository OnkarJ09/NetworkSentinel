// Minimal ArduinoOTA stub for g++ validation.
#pragma once
#include <Arduino.h>

class ArduinoOTAClass {
public:
    void setHostname(const char*) {}
    void setPassword(const char*) {}
    void setPort(uint16_t) {}
    void begin() {}
    void handle() {}
};

inline ArduinoOTAClass ArduinoOTA;
