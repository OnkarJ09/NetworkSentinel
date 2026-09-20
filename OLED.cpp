#include "OLED.h"

#include "config.h"

OLED oled;

// ============================================================
// BEGIN
// ============================================================

bool OLED::begin() {

    Wire.begin(
        OLED_SDA,
        OLED_SCL
    );

    if (
        !display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDR
        )
    ) {

        return false;
    }

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setTextSize(1);

    display.display();

    return true;
}

// ============================================================
// CLEAR
// ============================================================

void OLED::clear() {

    display.clearDisplay();
}

// ============================================================
// HEADER
// ============================================================

void OLED::header(
    const char* title
) {

    display.setTextSize(1);

    display.setCursor(
        0,
        0
    );

    display.print(
        title
    );

    display.drawLine(
        0,
        9,
        127,
        9,
        SSD1306_WHITE
    );
}

// ============================================================
// TEXT
// ============================================================

void OLED::text(
    int x,
    int y,
    const String& value
) {

    display.setCursor(
        x,
        y
    );

    display.print(
        value
    );
}

// ============================================================
// UPDATE
// ============================================================

void OLED::update() {

    display.display();
}

// ============================================================
// RAW
// ============================================================

Adafruit_SSD1306& OLED::raw() {

    return display;
}