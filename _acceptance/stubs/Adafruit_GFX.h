#pragma once
#include "Arduino.h"

class Adafruit_GFX {
public:
    void drawPixel(int16_t, int16_t, uint16_t) {}
    void drawLine(int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void drawRect(int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void fillRect(int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void drawCircle(int16_t, int16_t, int16_t, uint16_t) {}
    void fillCircle(int16_t, int16_t, int16_t, uint16_t) {}
    void drawTriangle(int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void fillTriangle(int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void drawRoundRect(int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void fillRoundRect(int16_t, int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void print(const char*) {}
    void setCursor(int16_t, int16_t) {}
    void setTextColor(uint16_t) {}
    void setTextSize(uint8_t) {}
    void setTextWrap(bool) {}
    void setRotation(uint8_t) {}
    void width() {}
    void height() {}
};
