#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Ping.h>

// ============================================================
// CONFIGURATION
// ============================================================

const char* WIFI_SSID     = "Hacked_Ext1";
const char* WIFI_PASSWORD = "OJH_m@23*+#";

// ============================================================
// OLED
// ============================================================

#define OLED_SDA 8
#define OLED_SCL 9
#define OLED_ADDR 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ============================================================
// JOYSTICK
// ============================================================

#define JOY_X  4
#define JOY_Y  5
#define JOY_SW 6

// Measured center from your joystick
#define JOY_CENTER_X 1911
#define JOY_CENTER_Y 1899

// Extreme-only thresholds
#define JOY_LEFT_THRESHOLD   500
#define JOY_RIGHT_THRESHOLD  3300

#define JOY_UP_THRESHOLD     500
#define JOY_DOWN_THRESHOLD   3300

// ============================================================
// JOYSTICK STATE
// ============================================================

enum JoyDirection {
  JOY_NONE,
  JOY_LEFT,
  JOY_RIGHT,
  JOY_UP,
  JOY_DOWN
};

JoyDirection lastDirection = JOY_NONE;

bool previousButtonState = HIGH;

unsigned long lastButtonTime = 0;

#define BUTTON_DEBOUNCE 200

// ============================================================
// APPLICATION STATE
// ============================================================

enum AppScreen {
  SCREEN_MENU,
  SCREEN_DASHBOARD,
  SCREEN_WIFI,
  SCREEN_INTERNET,
  SCREEN_NETWORK,
  SCREEN_SCANNER,
  SCREEN_SYSTEM
};

AppScreen screen = SCREEN_MENU;

// ============================================================
// MAIN MENU
// ============================================================

const char* menuItems[] = {
  "Dashboard",
  "WiFi Analyzer",
  "Internet Test",
  "LAN Scanner",
  "Network Info",
  "System"
};

const int MENU_COUNT = 6;

int menuSelection = 0;

// ============================================================
// NETWORK DATA
// ============================================================

volatile bool wifiConnected = false;

volatile bool gatewayOnline = false;

volatile bool internetOnline = false;

volatile int wifiRSSI = 0;

volatile int wifiChannel = 0;

volatile float gatewayPing = -1;

volatile float internetPing = -1;

volatile int packetLoss = 100;

volatile unsigned long dnsTime = 0;

// ============================================================
// NETWORK INFORMATION
// ============================================================

String currentSSID;
String currentBSSID;

IPAddress localIP;
IPAddress gatewayIP;
IPAddress subnetMask;
IPAddress dnsIP;

// ============================================================
// WIFI SCANNER
// ============================================================

struct WiFiNetwork {

  String ssid;

  int32_t rssi;

  int32_t channel;

  String encryption;
};

WiFiNetwork networks[20];

volatile int networkCount = 0;

volatile int selectedNetwork = 0;

// ============================================================
// SYSTEM
// ============================================================

unsigned long bootTime = 0;

unsigned long lastNetworkCheck = 0;

#define NETWORK_INTERVAL 10000

// ============================================================
// DISPLAY
// ============================================================

unsigned long lastDisplayUpdate = 0;

#define DISPLAY_INTERVAL 150

// ============================================================
// NETWORK TASK
// ============================================================

TaskHandle_t networkTaskHandle = NULL;

// ============================================================
// FORWARD DECLARATIONS
// ============================================================

void performJoystickAction(
  JoyDirection direction
);

void processJoystick();

void processButton();

void checkNetwork();

void scanWiFi();

void drawDisplay();

void openSelectedMenu();

void returnToMenu();

// ============================================================
// JOYSTICK DIRECTION
// ============================================================

JoyDirection getJoystickDirection() {

  int x = analogRead(JOY_X);
  int y = analogRead(JOY_Y);

  // ----------------------------------------------------------
  // X AXIS
  // ----------------------------------------------------------

  if (x <= JOY_LEFT_THRESHOLD) {
    return JOY_LEFT;
  }

  if (x >= JOY_RIGHT_THRESHOLD) {
    return JOY_RIGHT;
  }

  // ----------------------------------------------------------
  // Y AXIS
  // ----------------------------------------------------------

  if (y <= JOY_UP_THRESHOLD) {
    return JOY_UP;
  }

  if (y >= JOY_DOWN_THRESHOLD) {
    return JOY_DOWN;
  }

  return JOY_NONE;
}

// ============================================================
// JOYSTICK PROCESSING
// ============================================================

void processJoystick() {

  JoyDirection direction =
    getJoystickDirection();

  // ----------------------------------------------------------
  // CENTER
  // ----------------------------------------------------------

  if (direction == JOY_NONE) {

    // Returning to center re-arms
    // the joystick.

    lastDirection = JOY_NONE;

    return;
  }

  // ----------------------------------------------------------
  // SAME EXTREME STILL HELD
  // ----------------------------------------------------------

  if (direction == lastDirection) {

    return;
  }

  // ----------------------------------------------------------
  // NEW EXTREME
  // ----------------------------------------------------------

  lastDirection = direction;

  performJoystickAction(direction);
}

// ============================================================
// JOYSTICK ACTION
// ============================================================

void performJoystickAction(
  JoyDirection direction
) {

  // ==========================================================
  // MENU
  // ==========================================================

  if (screen == SCREEN_MENU) {

    switch (direction) {

      case JOY_UP:

        menuSelection--;

        if (menuSelection < 0) {
          menuSelection =
            MENU_COUNT - 1;
        }

        break;

      case JOY_DOWN:

        menuSelection++;

        if (
          menuSelection >= MENU_COUNT
        ) {
          menuSelection = 0;
        }

        break;

      case JOY_RIGHT:

        openSelectedMenu();

        break;

      case JOY_LEFT:

        // Already at menu
        break;

      default:
        break;
    }

    return;
  }

  // ==========================================================
  // INSIDE A SCREEN
  // ==========================================================

  switch (direction) {

    case JOY_LEFT:

      returnToMenu();

      break;

    case JOY_RIGHT:

      // RIGHT acts as SELECT/REFRESH

      if (
        screen == SCREEN_SCANNER
      ) {

        scanWiFi();

      } else {

        checkNetwork();
      }

      break;

    case JOY_UP:

      if (
        screen == SCREEN_SCANNER
      ) {

        if (
          selectedNetwork > 0
        ) {

          selectedNetwork--;
        }
      }

      break;

    case JOY_DOWN:

      if (
        screen == SCREEN_SCANNER
      ) {

        if (
          selectedNetwork <
          networkCount - 1
        ) {

          selectedNetwork++;
        }
      }

      break;

    default:
      break;
  }
}

// ============================================================
// BUTTON
// ============================================================

void processButton() {

  bool currentButton =
    digitalRead(JOY_SW);

  unsigned long now =
    millis();

  // ----------------------------------------------------------
  // NEW PRESS
  // ----------------------------------------------------------

  if (
    currentButton == LOW &&
    previousButtonState == HIGH &&
    now - lastButtonTime >=
      BUTTON_DEBOUNCE
  ) {

    lastButtonTime = now;

    Serial.println(
      "BUTTON PRESSED"
    );

    if (
      screen == SCREEN_MENU
    ) {

      openSelectedMenu();

    } else {

      // ------------------------------------------------------
      // SCREEN ACTION
      // ------------------------------------------------------

      if (
        screen == SCREEN_SCANNER
      ) {

        scanWiFi();

      } else {

        checkNetwork();
      }
    }
  }

  previousButtonState =
    currentButton;
}

// ============================================================
// OPEN MENU ITEM
// ============================================================

void openSelectedMenu() {

  switch (menuSelection) {

    case 0:

      screen =
        SCREEN_DASHBOARD;

      break;

    case 1:

      screen =
        SCREEN_WIFI;

      break;

    case 2:

      screen =
        SCREEN_INTERNET;

      break;

    case 3:

      screen =
        SCREEN_NETWORK;

      break;

    case 4:

      screen =
        SCREEN_NETWORK;

      break;

    case 5:

      screen =
        SCREEN_SYSTEM;

      break;
  }

  // ----------------------------------------------------------
  // LAN SCANNER
  // ----------------------------------------------------------

  if (menuSelection == 3) {

    // V5 implementation
    // will replace this screen.

    screen =
      SCREEN_NETWORK;
  }

  Serial.print(
    "OPEN MENU: "
  );

  Serial.println(
    menuItems[menuSelection]
  );
}

// ============================================================
// RETURN TO MENU
// ============================================================

void returnToMenu() {

  screen =
    SCREEN_MENU;

  lastDirection =
    JOY_NONE;
}

// ============================================================
// WIFI CONNECTION
// ============================================================

void connectWiFi() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setCursor(
    0,
    0
  );

  display.println(
    "NETWORK MONITOR"
  );

  display.setCursor(
    0,
    18
  );

  display.println(
    "Connecting WiFi..."
  );

  display.display();

  Serial.println();

  Serial.println(
    "Connecting to WiFi..."
  );

  WiFi.mode(
    WIFI_STA
  );

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  int attempts = 0;

  while (
    WiFi.status() != WL_CONNECTED &&
    attempts < 40
  ) {

    delay(250);

    Serial.print(".");

    attempts++;
  }

  Serial.println();

  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    wifiConnected =
      true;

    currentSSID =
      WiFi.SSID();

    currentBSSID =
      WiFi.BSSIDstr();

    localIP =
      WiFi.localIP();

    gatewayIP =
      WiFi.gatewayIP();

    subnetMask =
      WiFi.subnetMask();

    dnsIP =
      WiFi.dnsIP();

    wifiRSSI =
      WiFi.RSSI();

    wifiChannel =
      WiFi.channel();

    Serial.println(
      "WiFi connected!"
    );

    Serial.print(
      "IP: "
    );

    Serial.println(
      localIP
    );

  } else {

    wifiConnected =
      false;

    Serial.println(
      "WiFi connection FAILED"
    );
  }
}

// ============================================================
// GATEWAY TEST
// ============================================================

void pingGateway() {

  if (!wifiConnected) {

    gatewayOnline =
      false;

    gatewayPing =
      -1;

    return;
  }

  bool result =
    Ping.ping(
      gatewayIP,
      3
    );

  if (result) {

    gatewayOnline =
      true;

    gatewayPing =
      Ping.averageTime();

  } else {

    gatewayOnline =
      false;

    gatewayPing =
      -1;
  }
}

// ============================================================
// INTERNET TEST
// ============================================================

void pingInternet() {

  if (!wifiConnected) {

    internetOnline =
      false;

    internetPing =
      -1;

    packetLoss =
      100;

    return;
  }

  const int packets =
    10;

  int successful =
    0;

  float totalPing =
    0;

  for (
    int i = 0;
    i < packets;
    i++
  ) {

    bool result =
      Ping.ping(
        IPAddress(
          8,
          8,
          8,
          8
        ),
        1
      );

    if (result) {

      successful++;

      totalPing +=
        Ping.averageTime();
    }

    delay(50);
  }

  packetLoss =
    (
      (packets - successful)
      * 100
    ) / packets;

  if (
    successful > 0
  ) {

    internetOnline =
      true;

    internetPing =
      totalPing /
      successful;

  } else {

    internetOnline =
      false;

    internetPing =
      -1;
  }
}

// ============================================================
// DNS TEST
// ============================================================

void testDNS() {

  if (!wifiConnected) {

    dnsTime =
      0;

    return;
  }

  unsigned long start =
    millis();

  IPAddress resolved;

  bool result =
    WiFi.hostByName(
      "example.com",
      resolved
    );

  if (result) {

    dnsTime =
      millis() - start;

  } else {

    dnsTime =
      0;
  }
}

// ============================================================
// COMPLETE NETWORK TEST
// ============================================================

void checkNetwork() {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    wifiConnected =
      false;

    gatewayOnline =
      false;

    internetOnline =
      false;

    gatewayPing =
      -1;

    internetPing =
      -1;

    packetLoss =
      100;

    return;
  }

  wifiConnected =
    true;

  // ----------------------------------------------------------
  // WIFI DATA
  // ----------------------------------------------------------

  wifiRSSI =
    WiFi.RSSI();

  wifiChannel =
    WiFi.channel();

  currentSSID =
    WiFi.SSID();

  currentBSSID =
    WiFi.BSSIDstr();

  localIP =
    WiFi.localIP();

  gatewayIP =
    WiFi.gatewayIP();

  subnetMask =
    WiFi.subnetMask();

  dnsIP =
    WiFi.dnsIP();

  // ----------------------------------------------------------
  // TESTS
  // ----------------------------------------------------------

  pingGateway();

  pingInternet();

  testDNS();

  // ----------------------------------------------------------
  // SERIAL
  // ----------------------------------------------------------

  Serial.println();

  Serial.println(
    "========== NETWORK =========="
  );

  Serial.print(
    "SSID: "
  );

  Serial.println(
    currentSSID
  );

  Serial.print(
    "RSSI: "
  );

  Serial.print(
    wifiRSSI
  );

  Serial.println(
    " dBm"
  );

  Serial.print(
    "Gateway: "
  );

  if (gatewayOnline) {

    Serial.print(
      gatewayPing
    );

    Serial.println(
      " ms"
    );

  } else {

    Serial.println(
      "FAIL"
    );
  }

  Serial.print(
    "Internet: "
  );

  if (internetOnline) {

    Serial.print(
      internetPing
    );

    Serial.println(
      " ms"
    );

  } else {

    Serial.println(
      "FAIL"
    );
  }

  Serial.print(
    "Packet loss: "
  );

  Serial.print(
    packetLoss
  );

  Serial.println(
    "%"
  );

  Serial.print(
    "DNS: "
  );

  Serial.print(
    dnsTime
  );

  Serial.println(
    " ms"
  );
}

// ============================================================
// WIFI SCANNER
// ============================================================

void scanWiFi() {

  Serial.println();

  Serial.println(
    "========== WIFI SCAN =========="
  );

  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setCursor(
    0,
    0
  );

  display.println(
    "WIFI SCANNER"
  );

  display.setCursor(
    0,
    20
  );

  display.println(
    "Scanning..."
  );

  display.display();

  int count =
    WiFi.scanNetworks(
      false,
      true
    );

  networkCount =
    min(
      count,
      20
    );

  selectedNetwork =
    0;

  for (
    int i = 0;
    i < networkCount;
    i++
  ) {

    networks[i].ssid =
      WiFi.SSID(i);

    networks[i].rssi =
      WiFi.RSSI(i);

    networks[i].channel =
      WiFi.channel(i);

    networks[i].encryption =
      "SEC";
  }

  WiFi.scanDelete();

  Serial.print(
    "Networks found: "
  );

  Serial.println(
    networkCount
  );

  for (
    int i = 0;
    i < networkCount;
    i++
  ) {

    Serial.print(
      i + 1
    );

    Serial.print(
      ". "
    );

    Serial.print(
      networks[i].ssid
    );

    Serial.print(
      " | "
    );

    Serial.print(
      networks[i].rssi
    );

    Serial.print(
      " dBm | CH "
    );

    Serial.println(
      networks[i].channel
    );
  }
}

// ============================================================
// DRAW HEADER
// ============================================================

void drawHeader(
  const char* title
) {

  display.setTextSize(1);

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setCursor(
    0,
    0
  );

  display.println(
    title
  );

  display.drawLine(
    0,
    9,
    127,
    9,
    SSD1306_WHITE
  );
}

// ============================================================
// MENU
// ============================================================

void drawMenu() {

  display.setTextSize(1);

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setCursor(
    0,
    0
  );

  display.println(
    "NETWORK MONITOR"
  );

  display.drawLine(
    0,
    9,
    127,
    9,
    SSD1306_WHITE
  );

  // ----------------------------------------------------------
  // Show 5 items around selection
  // ----------------------------------------------------------

  int start =
    menuSelection - 2;

  if (start < 0) {
    start = 0;
  }

  if (
    start >
    MENU_COUNT - 5
  ) {

    start =
      max(
        0,
        MENU_COUNT - 5
      );
  }

  for (
    int i = 0;
    i < 5 &&
    start + i < MENU_COUNT;
    i++
  ) {

    int index =
      start + i;

    int y =
      14 + (i * 10);

    display.setCursor(
      0,
      y
    );

    if (
      index == menuSelection
    ) {

      display.print(
        ">"
      );

    } else {

      display.print(
        " "
      );
    }

    display.print(
      menuItems[index]
    );
  }
}

// ============================================================
// DASHBOARD
// ============================================================

void drawDashboard() {

  drawHeader(
    "DASHBOARD"
  );

  display.setCursor(
    0,
    13
  );

  display.print(
    "NET: "
  );

  display.println(
    internetOnline
      ? "ONLINE"
      : "OFFLINE"
  );

  display.setCursor(
    0,
    25
  );

  display.print(
    "PING: "
  );

  if (
    internetPing >= 0
  ) {

    display.print(
      (int)internetPing
    );

    display.println(
      "ms"
    );

  } else {

    display.println(
      "---"
    );
  }

  display.setCursor(
    0,
    37
  );

  display.print(
    "LOSS: "
  );

  display.print(
    packetLoss
  );

  display.println(
    "%"
  );

  display.setCursor(
    0,
    49
  );

  display.print(
    "RSSI: "
  );

  display.print(
    wifiRSSI
  );

  display.println(
    "dBm"
  );
}

// ============================================================
// WIFI SCREEN
// ============================================================

void drawWiFiScreen() {

  drawHeader(
    "WIFI"
  );

  display.setCursor(
    0,
    13
  );

  display.print(
    "SSID: "
  );

  display.println(
    currentSSID
  );

  display.setCursor(
    0,
    26
  );

  display.print(
    "RSSI: "
  );

  display.print(
    wifiRSSI
  );

  display.println(
    "dBm"
  );

  display.setCursor(
    0,
    39
  );

  display.print(
    "CH: "
  );

  display.println(
    wifiChannel
  );

  display.setCursor(
    0,
    52
  );

  display.print(
    "BSSID:"
  );

  if (
    currentBSSID.length() >= 11
  ) {

    display.println(
      currentBSSID.substring(
        0,
        11
      )
    );

  } else {

    display.println(
      currentBSSID
    );
  }
}

// ============================================================
// INTERNET SCREEN
// ============================================================

void drawInternetScreen() {

  drawHeader(
    "INTERNET TEST"
  );

  display.setCursor(
    0,
    13
  );

  display.print(
    "GW: "
  );

  if (gatewayOnline) {

    display.print(
      (int)gatewayPing
    );

    display.println(
      "ms OK"
    );

  } else {

    display.println(
      "FAIL"
    );
  }

  display.setCursor(
    0,
    27
  );

  display.print(
    "NET: "
  );

  if (internetOnline) {

    display.print(
      (int)internetPing
    );

    display.println(
      "ms OK"
    );

  } else {

    display.println(
      "FAIL"
    );
  }

  display.setCursor(
    0,
    41
  );

  display.print(
    "DNS: "
  );

  if (dnsTime > 0) {

    display.print(
      dnsTime
    );

    display.println(
      "ms"
    );

  } else {

    display.println(
      "FAIL"
    );
  }

  display.setCursor(
    0,
    55
  );

  display.print(
    "LOSS: "
  );

  display.print(
    packetLoss
  );

  display.println(
    "%"
  );
}

// ============================================================
// NETWORK SCREEN
// ============================================================

void drawNetworkScreen() {

  drawHeader(
    "NETWORK INFO"
  );

  display.setCursor(
    0,
    13
  );

  display.print(
    "IP:"
  );

  display.println(
    localIP
  );

  display.setCursor(
    0,
    26
  );

  display.print(
    "GW:"
  );

  display.println(
    gatewayIP
  );

  display.setCursor(
    0,
    39
  );

  display.print(
    "MASK:"
  );

  display.println(
    subnetMask
  );

  display.setCursor(
    0,
    52
  );

  display.print(
    "DNS:"
  );

  display.println(
    dnsIP
  );
}

// ============================================================
// WIFI SCANNER SCREEN
// ============================================================

void drawScannerScreen() {

  drawHeader(
    "WIFI ANALYZER"
  );

  if (
    networkCount == 0
  ) {

    display.setCursor(
      0,
      20
    );

    display.println(
      "No scan data"
    );

    display.setCursor(
      0,
      35
    );

    display.println(
      "PRESS = SCAN"
    );

    return;
  }

  WiFiNetwork& n =
    networks[
      selectedNetwork
    ];

  display.setCursor(
    0,
    13
  );

  display.print(
    selectedNetwork + 1
  );

  display.print(
    "/"
  );

  display.println(
    networkCount
  );

  display.setCursor(
    0,
    26
  );

  display.println(
    n.ssid
  );

  display.setCursor(
    0,
    39
  );

  display.print(
    n.rssi
  );

  display.print(
    "dBm CH"
  );

  display.println(
    n.channel
  );

  display.setCursor(
    0,
    52
  );

  display.println(
    "PRESS=RESCAN"
  );
}

// ============================================================
// SYSTEM SCREEN
// ============================================================

void drawSystemScreen() {

  drawHeader(
    "SYSTEM"
  );

  unsigned long uptime =
    millis() / 1000;

  unsigned long hours =
    uptime / 3600;

  unsigned long minutes =
    (uptime % 3600) / 60;

  unsigned long seconds =
    uptime % 60;

  display.setCursor(
    0,
    13
  );

  display.printf(
    "UP %02lu:%02lu:%02lu",
    hours,
    minutes,
    seconds
  );

  display.setCursor(
    0,
    27
  );

  display.print(
    "RAM:"
  );

  display.print(
    ESP.getFreeHeap() /
    1024
  );

  display.println(
    "KB"
  );

  display.setCursor(
    0,
    41
  );

  display.print(
    "CPU:"
  );

  display.print(
    getCpuFrequencyMhz()
  );

  display.println(
    "MHz"
  );

  display.setCursor(
    0,
    55
  );

  display.print(
    "RSSI:"
  );

  display.print(
    wifiRSSI
  );
}

// ============================================================
// DRAW DISPLAY
// ============================================================

void drawDisplay() {

  display.clearDisplay();

  switch (screen) {

    case SCREEN_MENU:

      drawMenu();

      break;

    case SCREEN_DASHBOARD:

      drawDashboard();

      break;

    case SCREEN_WIFI:

      drawWiFiScreen();

      break;

    case SCREEN_INTERNET:

      drawInternetScreen();

      break;

    case SCREEN_NETWORK:

      drawNetworkScreen();

      break;

    case SCREEN_SCANNER:

      drawScannerScreen();

      break;

    case SCREEN_SYSTEM:

      drawSystemScreen();

      break;
  }

  display.display();
}

// ============================================================
// NETWORK FREERTOS TASK
// ============================================================

void networkTask(
  void* parameter
) {

  Serial.println(
    "Network task started"
  );

  while (true) {

    checkNetwork();

    vTaskDelay(
      pdMS_TO_TICKS(
        NETWORK_INTERVAL
      )
    );
  }
}

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(
    115200
  );

  delay(500);

  bootTime =
    millis();

  Serial.println();

  Serial.println(
    "================================"
  );

  Serial.println(
    "ESP32-S3 NETWORK MONITOR V4"
  );

  Serial.println(
    "================================"
  );

  // ----------------------------------------------------------
  // ADC
  // ----------------------------------------------------------

  analogReadResolution(
    12
  );

  // ----------------------------------------------------------
  // JOYSTICK BUTTON
  // ----------------------------------------------------------

  pinMode(
    JOY_SW,
    INPUT_PULLUP
  );

  // ----------------------------------------------------------
  // OLED
  // ----------------------------------------------------------

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (
    !display.begin(
      SSD1306_SWITCHCAPVCC,
      OLED_ADDR
    )
  ) {

    Serial.println(
      "OLED FAILED!"
    );

    while (true) {

      delay(1000);
    }
  }

  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setCursor(
    0,
    0
  );

  display.println(
    "NETWORK MONITOR"
  );

  display.setCursor(
    0,
    20
  );

  display.println(
    "Initializing..."
  );

  display.display();

  delay(1000);

  // ----------------------------------------------------------
  // WIFI
  // ----------------------------------------------------------

  connectWiFi();

  delay(500);

  // ----------------------------------------------------------
  // FIRST NETWORK CHECK
  // ----------------------------------------------------------

  checkNetwork();

  // ----------------------------------------------------------
  // NETWORK TASK
  // ----------------------------------------------------------

  xTaskCreatePinnedToCore(

    networkTask,

    "NetworkTask",

    8192,

    NULL,

    1,

    &networkTaskHandle,

    0
  );

  // ----------------------------------------------------------
  // START MENU
  // ----------------------------------------------------------

  screen =
    SCREEN_MENU;

  menuSelection =
    0;

  drawDisplay();

  Serial.println();

  Serial.println(
    "SYSTEM READY"
  );
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // INPUT
  // ----------------------------------------------------------

  processJoystick();

  processButton();

  // ----------------------------------------------------------
  // DISPLAY
  // ----------------------------------------------------------

  if (
    millis() -
    lastDisplayUpdate >=
    DISPLAY_INTERVAL
  ) {

    lastDisplayUpdate =
      millis();

    drawDisplay();
  }

  // ----------------------------------------------------------
  // YIELD
  // ----------------------------------------------------------

  delay(5);
}