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
// TIMING
// ============================================================

#define DISPLAY_UPDATE_MS 150
#define NETWORK_UPDATE_MS 10000
#define BUTTON_DEBOUNCE_MS 25
#define BUTTON_LONG_PRESS_MS 1000

// Interval for automatic LAN scans (default 60 seconds)
#define LAN_SCAN_INTERVAL_MS 60000
