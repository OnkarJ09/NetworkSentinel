#pragma once
#include <Arduino.h>

// Button pin definitions
#define BUTTON_UP_PIN    13
#define BUTTON_DOWN_PIN  10
#define BUTTON_BACK_PIN  11
#define BUTTON_OK_PIN    12

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
    const uint8_t pins[BUTTON_COUNT] = {BUTTON_UP_PIN, BUTTON_DOWN_PIN, BUTTON_BACK_PIN, BUTTON_OK_PIN}; // UP, DOWN, BACK, OK
};
