#pragma once
#include <Arduino.h>

enum ButtonId {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_BACK,
    BUTTON_OK,
    BUTTON_COUNT
};

class Buttons {
public:
    void begin();
    void update();

    bool wasPressed(ButtonId button);
    bool wasLongPressed(ButtonId button);
    bool isHeld(ButtonId button);

private:
    static const uint32_t DEBOUNCE_MS = 25;
    static const uint32_t LONG_PRESS_MS = 1000;

    struct ButtonState {
        bool lastState = HIGH;
        bool stableState = HIGH;
        bool changed = false;
        bool pressed = false;
        bool longPressedReported = false;
        uint32_t lastChangeTime = 0;
        uint32_t pressedTime = 0;
    };

    ButtonState states[BUTTON_COUNT];
    const uint8_t pins[BUTTON_COUNT] = {4, 5, 6, 7}; // UP, DOWN, BACK, OK
};