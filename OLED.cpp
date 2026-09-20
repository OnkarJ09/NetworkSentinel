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
// SPARKLINE
// ============================================================

void OLED::sparkline(
    int x,
    int y,
    int width,
    int height,
    const float* data,
    int count,
    float minVal,
    float maxVal
) {

    if (count <= 0 || width <= 0 || height <= 0) {
        return;
    }

    float range = maxVal - minVal;
    if (range <= 0.0f) {
        range = 1.0f;
    }

    float stepX = (float)width / (float)count;

    int prevPx = x;
    int prevPy = y + height - 1;

    for (int i = 0; i < count; i++) {

        float v = data[i];
        if (v < minVal) v = minVal;
        if (v > maxVal) v = maxVal;

        float normalized = (v - minVal) / range;

        int px = x + (int)(i * stepX);
        if (px >= x + width) {
            px = x + width - 1;
        }

        int py = y + (height - 1) - (int)(normalized * (height - 1));
        if (py < y) py = y;
        if (py >= y + height) py = y + height - 1;

        if (i == 0) {
            display.drawPixel(
                px,
                py,
                SSD1306_WHITE
            );
        } else {
            display.drawLine(
                prevPx,
                prevPy,
                px,
                py,
                SSD1306_WHITE
            );
        }

        prevPx = px;
        prevPy = py;
    }
}

// ============================================================
// RAW
// ============================================================

Adafruit_SSD1306& OLED::raw() {

    return display;
}