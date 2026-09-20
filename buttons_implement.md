# Network Sentinel — Buttons Implementation

## 1. Objective

Replace the analog joystick navigation with four physical push buttons:

- UP
- DOWN
- BACK
- OK / SCAN

The OLED remains on the existing I2C pins.

The implementation must be modular, non-blocking, debounced, and compatible with the existing Network Sentinel architecture.

## 2. GPIO Mapping

### Buttons

| Button | ESP32-S3 GPIO | Other terminal |
|---|---:|---|
| UP | GPIO 4 | GND |
| DOWN | GPIO 5 | GND |
| BACK | GPIO 6 | GND |
| OK / SCAN | GPIO 7 | GND |

Use the ESP32 internal pull-up resistors.

### OLED

| OLED Pin | ESP32-S3 GPIO |
|---|---:|
| SDA | GPIO 8 |
| SCL | GPIO 9 |
| VCC | 3.3V |
| GND | GND |

Do not change the working OLED wiring.

## 3. Button Electrical Configuration

Each button is wired between its GPIO and GND:

```text
GPIO 4 ─── [UP BUTTON] ─── GND
GPIO 5 ─── [DOWN BUTTON] ─ GND
GPIO 6 ─── [BACK BUTTON] ─ GND
GPIO 7 ─── [OK BUTTON] ─── GND
```

Configure every button with:

```cpp
pinMode(pin, INPUT_PULLUP);
```

Therefore:

- HIGH = released
- LOW = pressed

No external pull-up resistor is required.

## 4. Physical Button Layout

Recommended front-panel arrangement:

```text
        [ UP ]

[ BACK ] [ OK ]

       [ DOWN ]
```

The exact physical placement can be changed without affecting the firmware.

## 5. Button Abstraction

Create a dedicated button module instead of scattering `digitalRead()` throughout the project.

Files:

```text
Buttons.h
Buttons.cpp
```

The rest of the application should interact with button events rather than raw GPIO reads.

Recommended logical events:

```cpp
BUTTON_UP
BUTTON_DOWN
BUTTON_BACK
BUTTON_OK
```

Example:

```cpp
if (buttons.wasPressed(BUTTON_OK)) {
    // Enter/select/start scan
}
```

## 6. Required Button Features

### Debouncing

Implement software debouncing.

Requirements:

- prevent mechanical switch bounce from generating multiple presses
- be non-blocking
- do not use `delay()`
- work continuously from the main application loop

A debounce interval around 25–50 ms is appropriate.

### Short Press

Each button must generate a single press event for a normal press.

Examples:

```text
UP    → move selection upward
DOWN  → move selection downward
BACK  → return to previous screen
OK    → select/enter/start action
```

### Long Press

Support long-press detection so functionality can be added later.

Initial behavior:

```text
BACK long press → return to Home/Dashboard
```

Do not make long-press handling interfere with normal short-press detection.

### Optional Repeat

Design UP/DOWN so key-repeat can be added later for holding a button.

Do not use blocking loops.

## 7. Navigation Semantics

### UP

```text
Menu:
↑ Previous item

Lists:
↑ Previous item/device/network

Pages:
Previous item/page where applicable
```

### DOWN

```text
Menu:
↓ Next item

Lists:
↓ Next item/device/network

Pages:
Next item/page where applicable
```

### BACK

```text
Current page → Previous menu

Submenu → Parent menu

Long press → Home/Dashboard
```

### OK / SCAN

Normal behavior:

```text
Select / Enter
```

Context-sensitive behavior:

```text
Wi-Fi page → Start Wi-Fi scan
LAN page   → Start LAN discovery
```

The button remains one logical OK action; individual screens decide whether that action starts a scan.

## 8. Suggested OLED Navigation Structure

Main menu:

```text
┌────────────────────────┐
│ NETWORK SENTINEL       │
│                        │
│ > Dashboard            │
│   Wi-Fi                │
│   LAN                  │
│   Internet             │
│   System               │
│   Events               │
│                        │
└────────────────────────┘
```

Controls:

```text
UP/DOWN → Navigate
OK      → Select
BACK    → Return
```

Wi-Fi submenu:

```text
Wi-Fi
├── Status
├── Networks
├── Channel
├── Signal
└── Scan
```

LAN submenu:

```text
LAN
├── Devices
├── Gateway
├── Discovery
└── Scan
```

Keep the menu structure compatible with the existing Menu/AppState implementation.

## 9. Implementation Requirements

Before editing code:

1. Inspect the existing project files.
2. Identify the current joystick module and all references to it.
3. Identify the existing Menu, AppState, OLED, NetworkManager/SentinelNetwork, and SentinelWeb code.
4. Preserve working features.
5. Do not rename unrelated classes or variables.
6. Do not rewrite the entire project unnecessarily.

The current working project is the source of truth for existing field and class names.

## 10. Joystick Removal

The joystick is being replaced completely for navigation.

Find and remove or disable:

- joystick GPIO definitions
- joystick initialization
- joystick ADC reads
- joystick direction calculations
- joystick button/SW handling
- joystick-specific navigation logic

Do not leave duplicate navigation paths active.

The new button system must become the single navigation input source.

## 11. Recommended Buttons API

Use a simple API similar to:

```cpp
enum ButtonId {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_BACK,
    BUTTON_OK,
    BUTTON_COUNT
};
```

Recommended methods:

```cpp
class Buttons {
public:
    void begin();
    void update();

    bool wasPressed(ButtonId button);
    bool wasLongPressed(ButtonId button);
    bool isHeld(ButtonId button);
};
```

Adapt the API to the existing project style if an equivalent input abstraction already exists.

Do not introduce unnecessary dependencies.

## 12. Non-Blocking Architecture

The button module must be compatible with the existing non-blocking architecture.

Avoid:

```cpp
delay(...)
while (digitalRead(...) == LOW) { ... }
```

Prefer:

```cpp
buttons.update();
```

called continuously from the main loop.

Use `millis()` or equivalent non-blocking timing.

## 13. Event Handling

A button press should be consumed once.

Example:

```cpp
buttons.update();

if (buttons.wasPressed(BUTTON_UP)) {
    // navigation
}

if (buttons.wasPressed(BUTTON_DOWN)) {
    // navigation
}

if (buttons.wasPressed(BUTTON_BACK)) {
    // navigation
}

if (buttons.wasPressed(BUTTON_OK)) {
    // selection / context action
}
```

Do not repeatedly trigger an action just because the button remains LOW unless explicit key-repeat behavior is implemented.

## 14. Integration With Existing Menu System

Integrate button events into the existing Menu system.

Do not create a second independent menu architecture.

Intended flow:

```text
Buttons
   ↓
Button events
   ↓
Menu / navigation logic
   ↓
AppState
   ↓
OLED
```

For scan operations:

```text
OK / SCAN
    ↓
Current screen checks context
    ↓
Wi-Fi scan / LAN discovery
    ↓
Existing network subsystem
    ↓
AppState
    ↓
OLED + Web UI
```

## 15. OLED Behavior

Do not change the working OLED hardware configuration.

Keep:

```text
SDA → GPIO 8
SCL → GPIO 9
```

The OLED should continue displaying:

- main menu
- selected menu item
- network status
- Wi-Fi scan results
- LAN results when implemented
- system information
- events

Redraw only when necessary rather than continuously redrawing the entire display.

## 16. Physical Wiring Checklist

Before powering the ESP32-S3:

```text
[ ] UP button: GPIO 4 ↔ button ↔ GND
[ ] DOWN button: GPIO 5 ↔ button ↔ GND
[ ] BACK button: GPIO 6 ↔ button ↔ GND
[ ] OK button: GPIO 7 ↔ button ↔ GND

[ ] OLED SDA → GPIO 8
[ ] OLED SCL → GPIO 9
[ ] OLED VCC → 3.3V
[ ] OLED GND → GND

[ ] No joystick connected to GPIO 4–7
[ ] No external pull-up resistors required
[ ] Common GND is connected
```

For 4-pin tactile push buttons, verify which two pins are electrically connected when pressed. Usually the two pins on each side are internally common, and pressing connects the two sides.

## 17. Testing Plan

### Test 1 — Individual buttons

Display:

```text
UP
DOWN
BACK
OK
```

when each button is pressed.

Verify:

- one event per press
- no false presses
- no excessive bouncing

### Test 2 — Navigation

Verify:

```text
UP/DOWN → selection movement
OK      → enter/select
BACK    → return
```

### Test 3 — Long press

Verify:

```text
BACK long press → Home/Dashboard
```

### Test 4 — Existing functionality

After button integration verify that:

- OLED still works
- Wi-Fi still works
- Wi-Fi scanning still works
- network telemetry still works
- WebSocket still works
- Web UI still works
- history still works
- no old joystick code causes conflicts

## 18. Important Constraints

Do not:

- change OLED GPIO 8/9
- introduce blocking delays
- introduce a new UI framework
- add unnecessary libraries
- break the existing WebSocket/REST implementation
- rename working NetworkManager/SentinelNetwork classes unnecessarily
- assume AppState field names without inspecting the project
- rewrite working network functionality
- add LAN discovery in this implementation unless it already exists

This task is specifically the physical-button navigation replacement.

## 19. Completion Criteria

The implementation is complete when:

1. Four buttons work reliably.
2. GPIO mapping is exactly:
   - GPIO 4 = UP
   - GPIO 5 = DOWN
   - GPIO 6 = BACK
   - GPIO 7 = OK/SCAN
3. OLED remains on GPIO 8/9.
4. Debouncing works.
5. Short presses generate one event.
6. Long press support works.
7. Menu navigation works.
8. OK performs context-sensitive selection/scan actions.
9. BACK returns to the previous screen.
10. Existing Network Sentinel functionality continues working.
11. No blocking `delay()`-based input handling is introduced.
12. The project compiles cleanly under the currently installed ESP32 Arduino core.

## 20. Final Target

```text
                 ┌─────────┐
                 │   UP    │
                 └─────────┘
                     │
                     ▲

        ┌─────────┐       ┌─────────┐
        │  BACK   │       │   OK    │
        └─────────┘       └─────────┘

                     ▼
                 ┌─────────┐
                 │  DOWN   │
                 └─────────┘
```

Network Sentinel should feel like a small standalone network-monitoring appliance:

```text
             NETWORK SENTINEL
                    │
          ┌─────────┴─────────┐
          │                   │
        OLED              WEB UI
          │                   │
       Buttons          WebSocket/HTTP
          │
     ┌────┴────┐
     │         │
   UP/DOWN   BACK/OK
     │         │
     └────┬────┘
          │
       Menu/AppState
          │
    Network subsystems
```

Implement this cleanly and incrementally. Preserve all existing working functionality.
