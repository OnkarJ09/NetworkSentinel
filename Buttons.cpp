#include "Buttons.h"
#include "config.h"

void Buttons::begin() {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(pins[i], INPUT_PULLUP);
    }

    // Initialize button states
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        states[i].lastState = HIGH;
        states[i].stableState = HIGH;
        states[i].changed = false;
        states[i].pressed = false;
        states[i].longPressedReported = false;
        states[i].lastChangeTime = 0;
        states[i].pressedTime = 0;
    }
}

void Buttons::update() {
    uint32_t now = millis();

    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        bool rawState = digitalRead(pins[i]);
        ButtonState& state = states[i];

        // Check if state changed
        if (rawState != state.lastState) {
            state.lastChangeTime = now;
            state.changed = true;
        } else {
            state.changed = false;
        }

        // Check if debounce period has passed
        if (now - state.lastChangeTime >= DEBOUNCE_MS) {
            // State is stable
            if (rawState != state.stableState) {
                state.stableState = rawState;

                if (state.stableState == LOW) {
                    // Button just pressed
                    state.pressed = true;
                    state.pressedTime = now;
                    state.longPressedReported = false;
                } else {
                    // Button just released
                    state.pressed = false;
                }
            }
        }

        // Check for long press
        if (state.stableState == LOW && !state.longPressedReported) {
            if (now - state.pressedTime >= LONG_PRESS_MS) {
                state.longPressedReported = true;
            }
        }

        state.lastState = rawState;
    }
}

bool Buttons::wasPressed(ButtonId button) {
    if (button >= BUTTON_COUNT) return false;

    bool result = states[button].pressed;
    states[button].pressed = false; // Clear after reading
    return result;
}

bool Buttons::wasLongPressed(ButtonId button) {
    if (button >= BUTTON_COUNT) return false;

    bool result = states[button].longPressedReported;
    states[button].longPressedReported = false; // Clear after reading
    return result;
}

bool Buttons::isHeld(ButtonId button) {
    if (button >= BUTTON_COUNT) return false;

    return states[button].stableState == LOW;
}