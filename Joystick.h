#pragma once

#include <Arduino.h>

enum class JoyDirection {

    NONE,

    LEFT,

    RIGHT,

    UP,

    DOWN
};

class Joystick {

public:

    void begin();

    void update();

    JoyDirection getDirection();

    bool wasPressed();

private:

    JoyDirection readDirection();

    JoyDirection lastDirection =
        JoyDirection::NONE;

    bool previousButton =
        HIGH;

    bool buttonPressed =
        false;

    unsigned long lastButtonTime =
        0;
};