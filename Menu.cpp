#include "Menu.h"

#include "OLED.h"

Menu menu;

// ============================================================
// BEGIN
// ============================================================

void Menu::begin() {

    selectedItem =
        0;

    inside =
        false;
}

// ============================================================
// UP
// ============================================================

void Menu::up() {

    if (inside) {

        return;
    }

    selectedItem--;

    if (
        selectedItem < 0
    ) {

        selectedItem =
            ITEM_COUNT - 1;
    }
}

// ============================================================
// DOWN
// ============================================================

void Menu::down() {

    if (inside) {

        return;
    }

    selectedItem++;

    if (
        selectedItem >= ITEM_COUNT
    ) {

        selectedItem =
            0;
    }
}

// ============================================================
// SELECT
// ============================================================

void Menu::select() {

    inside =
        true;
}

// ============================================================
// BACK
// ============================================================

void Menu::back() {

    inside =
        false;
}

// ============================================================
// SELECTED
// ============================================================

int Menu::selected() {

    return selectedItem;
}

// ============================================================
// NAME
// ============================================================

const char* Menu::selectedName() {

    return items[
        selectedItem
    ];
}

// ============================================================
// INSIDE
// ============================================================

bool Menu::isInside() {

    return inside;
}

// ============================================================
// DRAW
// ============================================================

void Menu::draw() {

    oled.clear();

    // --------------------------------------------------------
    // MAIN MENU
    // --------------------------------------------------------

    if (!inside) {

        oled.header(
            "NETWORK SENTINEL"
        );

        int start =
            selectedItem - 2;

        if (start < 0) {

            start = 0;
        }

        if (
            start >
            ITEM_COUNT - 5
        ) {

            start =
                max(
                    0,
                    ITEM_COUNT - 5
                );
        }

        for (
            int i = 0;
            i < 5 &&
            start + i < ITEM_COUNT;
            i++
        ) {

            int index =
                start + i;

            int y =
                13 + i * 10;

            oled.raw().setCursor(
                0,
                y
            );

            if (
                index ==
                selectedItem
            ) {

                oled.raw().print(
                    ">"
                );

            } else {

                oled.raw().print(
                    " "
                );
            }

            oled.raw().print(
                items[index]
            );
        }

        oled.update();

        return;
    }

    // --------------------------------------------------------
    // SELECTED SCREEN
    // --------------------------------------------------------

    oled.header(
        selectedName()
    );

    oled.text(
        0,
        20,
        "Coming in V6..."
    );

    oled.text(
        0,
        35,
        "LEFT = BACK"
    );

    oled.update();
}