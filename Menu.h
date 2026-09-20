#pragma once

#include <Arduino.h>

class Menu {

public:

    void begin();

    void draw();

    void up();

    void down();

    void select();

    void back();

    int selected();

    const char* selectedName();

    bool isInside();

private:

    int selectedItem = 0;

    bool inside =
        false;

    const char* items[8] = {

        "Dashboard",
        "WiFi Analyzer",
        "LAN Scanner",
        "Internet",
        "Statistics",
        "Events",
        "Network Info",
        "System"
    };

    static constexpr int ITEM_COUNT = 8;
};

extern Menu menu;