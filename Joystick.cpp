#include "Joystick.h"

#include "config.h"

// ============================================================
// BEGIN
// ============================================================

void Joystick::begin() {

    analogReadResolution(12);

    pinMode(
        JOY_SW,
        INPUT_PULLUP
    );

    lastDirection =
        JoyDirection::NONE;

    previousButton =
        HIGH;
}

// ============================================================
// READ DIRECTION
// ============================================================

JoyDirection Joystick::readDirection() {

    int x =
        analogRead(JOY_X);

    int y =
        analogRead(JOY_Y);

    // --------------------------------------------------------
    // EXTREME LEFT
    // --------------------------------------------------------

    if (
        x <= JOY_LEFT_THRESHOLD
    ) {

        return JoyDirection::LEFT;
    }

    // --------------------------------------------------------
    // EXTREME RIGHT
    // --------------------------------------------------------

    if (
        x >= JOY_RIGHT_THRESHOLD
    ) {

        return JoyDirection::RIGHT;
    }

    // --------------------------------------------------------
    // EXTREME UP
    // --------------------------------------------------------

    if (
        y <= JOY_UP_THRESHOLD
    ) {

        return JoyDirection::UP;
    }

    // --------------------------------------------------------
    // EXTREME DOWN
    // --------------------------------------------------------

    if (
        y >= JOY_DOWN_THRESHOLD
    ) {

        return JoyDirection::DOWN;
    }

    return JoyDirection::NONE;
}

// ============================================================
// UPDATE
// ============================================================

void Joystick::update() {

    buttonPressed =
        false;

    JoyDirection direction =
        readDirection();

    // --------------------------------------------------------
    // CENTER
    // --------------------------------------------------------

    if (
        direction ==
        JoyDirection::NONE
    ) {

        // Re-arm joystick
        lastDirection =
            JoyDirection::NONE;
    }

    // --------------------------------------------------------
    // NEW DIRECTION
    // --------------------------------------------------------

    else if (
        direction !=
        lastDirection
    ) {

        lastDirection =
            direction;
    }

    // --------------------------------------------------------
    // BUTTON
    // --------------------------------------------------------

    bool button =
        digitalRead(JOY_SW);

    unsigned long now =
        millis();

    if (
        button == LOW &&
        previousButton == HIGH &&
        now - lastButtonTime >=
            BUTTON_DEBOUNCE_MS
    ) {

        buttonPressed =
            true;

        lastButtonTime =
            now;
    }

    previousButton =
        button;
}

// ============================================================
// GET DIRECTION
// ============================================================

JoyDirection Joystick::getDirection() {

    // IMPORTANT:
    // Only return the direction on the
    // transition into an extreme.

    static JoyDirection reported =
        JoyDirection::NONE;

    JoyDirection current =
        lastDirection;

    if (
        current == JoyDirection::NONE
    ) {

        reported =
            JoyDirection::NONE;

        return JoyDirection::NONE;
    }

    if (
        current != reported
    ) {

        reported =
            current;

        return current;
    }

    return JoyDirection::NONE;
}

// ============================================================
// BUTTON
// ============================================================

bool Joystick::wasPressed() {

    return buttonPressed;
}