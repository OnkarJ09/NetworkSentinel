#pragma once

// ============================================================
// WIFI
// ============================================================

#define WIFI_SSID     "Hacked_Ext1"
#define WIFI_PASSWORD "OJH_m@23*+#"

// ============================================================
// OLED
// ============================================================

#define OLED_SDA 8
#define OLED_SCL 9
#define OLED_ADDR 0x3C

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// ============================================================
// JOYSTICK
// ============================================================

#define JOY_X  4
#define JOY_Y  5
#define JOY_SW 6

// Your measured center
#define JOY_CENTER_X 1911
#define JOY_CENTER_Y 1899

// Extreme thresholds
#define JOY_LEFT_THRESHOLD   500
#define JOY_RIGHT_THRESHOLD  3300

#define JOY_UP_THRESHOLD     500
#define JOY_DOWN_THRESHOLD   3300

// ============================================================
// TIMING
// ============================================================

#define DISPLAY_UPDATE_MS 150
#define NETWORK_UPDATE_MS 10000
#define BUTTON_DEBOUNCE_MS 200

// Interval for automatic LAN scans (default 60 seconds)
#define LAN_SCAN_INTERVAL_MS 60000