#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLED {

public:

    bool begin();

    void clear();

    void header(
        const char* title
    );

    void text(
        int x,
        int y,
        const String& value
    );

    void update();

    Adafruit_SSD1306& raw();

private:

    Adafruit_SSD1306 display{
        128,
        64,
        &Wire,
        -1
    };
};

extern OLED oled;