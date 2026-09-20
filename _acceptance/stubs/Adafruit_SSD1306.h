#pragma once
#include "Arduino.h"
#include "Adafruit_GFX.h"

#define SSD1306_BLACK 0
#define SSD1306_WHITE 1
#define SSD1306_INVERSE 2
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_I2C_ADDRESS 0x3C

class Adafruit_SSD1306 : public Adafruit_GFX {
public:
    Adafruit_SSD1306(int8_t w, int8_t h, void* wire, int8_t rst) : Adafruit_GFX(), width(w), height(h) {}
    Adafruit_SSD1306(int8_t, int8_t, void*, int8_t, bool, bool) {}
    bool begin(uint8_t, bool, bool, unsigned long) { return true; }
    bool begin(uint8_t vcc, uint8_t addr) { return true; }
    void display() {}
    void clearDisplay() {}
    void setTextSize(uint8_t) {}
    void setTextColor(uint16_t) {}
    void setCursor(int16_t, int16_t) {}
    void println(const char*) {}
    void print(const char*) {}
    void printf(const char*, ...) {}
    void drawPixel(int16_t, int16_t, uint16_t) {}
    void drawLine(int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void drawRect(int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    void fillRect(int16_t, int16_t, int16_t, int16_t, uint16_t) {}
    uint8_t width, height;
};

class TwoWire {
public:
    void begin(int, int, unsigned long) {}
    void begin(int, int) {}
    void begin() {}
};
extern TwoWire Wire;
