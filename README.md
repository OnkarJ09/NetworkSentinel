# Network Sentinel

![Network Sentinel Banner](https://via.placeholder.com/800x200/0066CC/FFFFFF?text=Network+Sentinel+-+Live+Network+Telemetry+Monitor)

A comprehensive network monitoring appliance built on ESP32-S3 with live telemetry, OLED display, and web interface. Features real-time network statistics, Wi-Fi/LAN scanning, and interactive controls.

![Network Sentinel Diagram](https://via.placeholder.com/600x400/FFFFFF/000000?text=Network+Sentinel+System+Diagram)

## Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Wiring Diagram](#wiring-diagram)
- [Button Layout](#button-layout)
- [Software Features](#software-features)
- [Installation](#installation)
- [Usage](#usage)
- [Screens](#screens)
- [Web Interface](#web-interface)
- [License](#license)

## Features

### Hardware Monitoring
- Real-time network telemetry (ping, packet loss, RSSI)
- Wi-Fi network scanning and analysis
- LAN device discovery
- Internet connectivity testing
- System information (uptime, heap usage, CPU frequency)

### Display
- 128x64 OLED display with multiple screens
- Dashboard view with key metrics
- Detailed Wi-Fi and LAN information
- Network statistics and history graphs
- Sparkline visualizations for signal strength

### Controls
- 4-button navigation (UP, DOWN, BACK, OK/SCAN)
- Debounced input with short/long press support
- Context-sensitive button actions
- Menu-driven interface

### Web Interface
- Live telemetry streaming via WebSocket
- Historical data buffering (60 samples)
- REST API for data access
- Responsive design for mobile/desktop

### Network Analysis
- Wi-Fi network listing with signal strength
- Channel utilization analysis
- Open network detection
- LAN device discovery with hostname resolution
- Latency and online/offline status

## Hardware Requirements

### Main Components
- **ESP32-S3** development board
- **SSD1306** 128x64 I2C OLED display
- **4x Push buttons** (normally open, momentary)
- **Breadboard** and jumper wires
- **USB cable** for power and programming

### Optional
- **Enclosure** for portable use
- **Battery pack** for mobile operation

## Wiring Diagram

```
ESP32-S3        OLED Display      Buttons
--------        --------------      -------
3V3     ─────── VCC
GND     ─────── GND
GPIO4   ─────── SDA
GPIO5   ─────── SCL
GPIO13  ─────── [UP BUTTON] ──── GND
GPIO10  ─────── [DOWN BUTTON] ── GND
GPIO11  ─────── [BACK BUTTON] ── GND
GPIO12  ─────── [OK BUTTON] ──── GND
```

### Button Wiring
Each button connects between its GPIO pin and ground:
```
GPIO 13 ─── [UP BUTTON] ─── GND
GPIO 10 ─── [DOWN BUTTON] ─ GND
GPIO 11 ─── [BACK BUTTON] ─ GND
GPIO 12 ─── [OK BUTTON] ─── GND
```

All buttons use the ESP32's internal pull-up resistors:
- **HIGH** = button released
- **LOW** = button pressed

## Button Layout

Recommended front-panel arrangement:

```
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

## Software Features

### Button Functions
- **Short Press**: Normal navigation and selection
- **Long Press** (BACK only): Return to Home/Dashboard
- **Debouncing**: 25ms software debounce (no bounce, no blocking)
- **Non-blocking**: Uses `millis()` timing for responsive interface

### Screen Navigation
1. **Dashboard** - Connection status, ping, loss, RSSI with sparkline
2. **Wi-Fi Analyzer** - Network scanning, SSID, RSSI, channel, security
3. **LAN Discovery** - Connected devices, IP, hostname, latency
4. **Internet Test** - Wi-Fi, gateway, internet, DNS connectivity
5. **Wi-Fi Statistics** - Network count, average RSSI, open networks, busiest channel
6. **Events** - System events and notifications
7. **Network Info** - Local IP, gateway, DNS, channel
8. **System** - Uptime, heap usage, CPU frequency, core usage

### Web Interface Features
- Real-time telemetry updates via WebSocket
- Historical data charts (60-point circular buffer)
- Responsive layout for all screen sizes
- Network statistics dashboard
- Export capabilities (planned)

## Installation

### 1. Install Dependencies
- Arduino IDE or PlatformIO
- ESP32 board support (ESP32 Arduino core)
- Required libraries:
  - Adafruit SSD1306
  - Adafruit GFX Library
  - WiFi (built-in for ESP32)
  - WebSockets (built-in for ESP32)

### 2. Clone Repository
```bash
git clone https://github.com/yourusername/network-sentinel.git
cd network-sentinel
```

### 3. Configure Wi-Fi
Edit `config.h` to set your Wi-Fi credentials:
```cpp
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
```

### 4. Build and Upload
- Select "ESP32 Dev Module" as board
- Set upload speed to 921600
- Flash the firmware to your ESP32-S3

## Usage

### First Power On
1. Device will boot and display "Starting..."
2. Connects to configured Wi-Fi network
3. Initializes OLED display and button subsystem
4. Begins network monitoring and telemetry collection

### Button Controls
| Button | Main Menu      | Wi-Fi Screen     | LAN Screen       | Other Screens    | Long Press     |
|--------|----------------|------------------|------------------|------------------|----------------|
| **UP** | Previous item  | Previous AP      | Previous device  | Previous item    | -              |
| **DOWN**| Next item     | Next AP          | Next device      | Next item        | -              |
| **OK** | Select item    | Scan Wi-Fi       | Scan LAN         | Update network   | -              |
| **BACK**| Select item   | Go back          | Go back          | Go back          | Home/Dashboard |

### Menu Structure
```
NETWORK SENTINEL
├── Dashboard          ← Default view
├── Wi-Fi              │
│   ├── Scan networks  │
│   ├── Network list   │
│   └── Statistics     │
├── LAN                │
│   ├── Device scan    │
│   ├── Device list    │
│   └── Statistics     │
├── Internet Test      │
├── Wi-Fi Statistics   │
├── Events             │
├── Network Info       │
└── System             ← System diagnostics
```

## Screens

### Dashboard
```
┌─────────────────────────────┐
│        DASHBOARD            │
├─────────────────────────────┤
│ NET: ONLINE                 │
│ PING: 23ms                  │
│ LOSS: 0%                    │
│ RSSI: -45dBm ▁▂▃▄▅▆▇█      │
└─────────────────────────────┘
```

### Wi-Fi Analyzer
```
┌─────────────────────────────┐
│      WI-FI ANALYZER         │
├─────────────────────────────┤
│ 1/3                         │
│ MyHomeNetwork               │
│ -42 dBm  Ch 6  WPA2         │
│ SIG: ||||                  │
└─────────────────────────────┘
```

### LAN Discovery
```
┌─────────────────────────────┐
│      LAN DISCOVERY          │
├─────────────────────────────┤
│ 1/2                         │
│ 192.168.1.100               │
│ my-laptop                   │
│ 15ms ONLINE                 │
└─────────────────────────────┘
```

### Statistics View
```
┌─────────────────────────────┐
│        WI-FI STATS          │
├─────────────────────────────┤
│ APs: 15                     │
│ AVG: -58dBm                 │
│ OPEN: 2                     │
│ BUSY: Ch 6 (5 AP)           │
└─────────────────────────────┘
```

## Web Interface

Access the web interface at: `http://[ESP32_IP_ADDRESS]`

### Features
- **Live Telemetry**: Real-time updating of network metrics
- **Historical Charts**: 60-second history of key metrics
- **Responsive Design**: Works on mobile, tablet, and desktop
- **Network Overview**: Connection status, signal strength, packet loss

### API Endpoints
- `GET /api` - dashboard snapshot (network, system, health)
- `GET /api/wifi` - scanned Wi-Fi networks
- `POST /api/wifi/scan` - start a Wi-Fi scan
- `GET /api/lan` - discovered LAN devices
- `GET /api/lan/scan` - start a LAN scan
- `GET /api/events` - event log
- `GET /api/config` / `POST /api/config` - read / update settings
- `GET /api/debug` - diagnostics counters
- `POST /api/restart` - reboot
- `POST /api/reset` - factory reset

WebSocket (`ws://[IP]:81`) pushes `telemetry`, `wifi`, `history`, `lan`, `events`.

### Flash Files
- `/config.txt` - runtime settings (key=value)
- `/known_aps.txt` - known-AP security baseline
- `/lan_devices.txt` - LAN device history
- `/events.txt` - event log (trimmed to the last 32 on boot)

## Project Structure

```
NetworkSentinel/
├── NetworkSentinel.ino    # Main sketch
├── Buttons.h             # Button handling library
├── Buttons.cpp           # Button handling implementation
├── config.h              # Wi-Fi and hardware configuration
├── AppState.h            # Application state management
├── Config.h              # Runtime config (LittleFS-backed)
├── Config.cpp            # Config load/save implementation
├── OLED.h                # Display handling
├── Menu.h                # Menu system
├── NetworkManager.h      # Network monitoring core
├── SentinelWeb.h         # Web server and WebSocket
├── SentinelWeb.cpp       # Web server implementation
├── LICENSE               # MIT License
├── README.md             # This file
└── buttons_implement.md  # Button implementation specifications
```

## Configuration

### config.h
Modify these settings for your environment:
```cpp
#define WIFI_SSID       "YourNetworkSSID"
#define WIFI_PASSWORD   "YourNetworkPassword"

// Button timing (usually no need to change)
#define BUTTON_DEBOUNCE_MS    25
#define BUTTON_LONG_PRESS_MS  1000

// Display settings
#define DISPLAY_UPDATE_MS 100
```

### Button Timing
- **BUTTON_DEBOUNCE_MS**: Mechanical debounce interval (25ms recommended)
- **BUTTON_LONG_PRESS_MS**: Time to trigger long press (1000ms = 1 second)

### Runtime Configuration
Tunable limits are stored in `/config.txt` on LittleFS and can be edited from
the System page, or via the API:
- `GET /api/config` - current settings (JSON)
- `POST /api/config` - update settings (form-encoded), saved to flash

Settings: `lanScanIntervalMs`, `highLatencyMs`, `outageWindowMs`,
`outageLimit`, `lanAutoScan`.

## How It Works

### Telemetry Collection
1. Background task collects network metrics every second
2. Data includes: ping, packet loss, RSSI, connection status
3. Historical data stored in 60-sample circular buffer
4. Heap monitoring prevents memory leaks

### Button Handling
1. `buttons.update()` called every loop iteration
2. Hardware debouncing filters mechanical noise
3. State machine tracks: idle, pressed, held, long-pressed
4. Events fired once per press/release cycle

### Display System
- Screen redrawn only when data changes
- Sparkline visualizations for trend detection
- Header shows current screen name
- Automatic screen rotation (optional feature)

### WebSocket Communication
- JSON text frames, one per message, tagged by `type`
- Browser client auto-reconnects on disconnect
- Falls back to REST polling (`refresh()`) for the initial load

## Customization

### Adding New Screens
1. Add new case to `menu.selected()` switch in `drawCurrentScreen()`
2. Create corresponding draw function (e.g., `drawNewScreen()`)
3. Add navigation logic in `loop()` button handling if needed
4. Update menu items in Menu.h if adding to main menu

### Changing Button Functions
Modify the button handling sections in `loop()`:
- Wi-Fi screen: menu.selected() == 1
- LAN screen: menu.selected() == 2
- Other screens: else block
- Main menu: !menu.isInside()

### Adjusting Sensitivity
Change thresholds in `Buttons.cpp`:
- Debounce timing: BUTTON_DEBOUNCE_MS constant
- Long press timing: BUTTON_LONG_PRESS_MS constant

## Troubleshooting

### Common Issues

#### OLED Not Displaying
- Check wiring: SDA→GPIO4, SCL→GPIO5
- Verify I2C address (usually 0x3C for SSD1306)
- Ensure 3.3V power to VCC pin
- Try different OLED library version

#### Buttons Not Responding
- Verify button wiring to correct GPIOs (UP 13, DOWN 10, BACK 11, OK 12)
- Check for shorts between button pins and ground
- Ensure no external pull-ups conflicting with internal ones
- Test with Serial.print for debugging

#### Wi-Fi Connection Issues
- Double-check SSID and password in config.h
- Ensure ESP32 is within range of access point
- Check for 2.4GHz network (ESP32 doesn't support 5GHz)
- Monitor serial output for connection errors

#### No Web Interface Access
- Verify device got IP address (check serial monitor)
- Try pinging the ESP32 IP address
- Check firewall settings on your computer
- Ensure computer and ESP32 are on same subnet

## Performance

### Memory Usage
- **Flash Storage**: ~450KB (leaves room for OTA updates)
- **RAM Usage**: ~80KB dynamic (plenty of heap remaining)
- **Heap Monitoring**: Automatic minimum heap tracking

### Update Rates
- **Telemetry Collection**: 1Hz (once per second)
- **OLED Update**: 10Hz (every 100ms, only when changed)
- **Button Polling**: 100Hz (every 10ms in main loop)
- **WebSocket Updates**: Push-based, immediate on change

### Network Impact
- Minimal bandwidth usage (<1kbps idle)
- WebSocket only sends data when values change significantly
- Historical data served efficiently on demand

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- ESP32 Arduino core team
- Adafruit for SSD1306 and GFX libraries
- Open-source networking libraries
- Community contributors and testers

---

*Built with PlatformIO & Arduino IDE*  
*Enjoy monitoring your network! 📡*