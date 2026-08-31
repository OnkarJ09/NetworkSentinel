#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Ping.h>

// ============================================================
// WIFI CONFIGURATION
// ============================================================

const char* WIFI_SSID = "Hacked_Ext1";
const char* WIFI_PASSWORD = "OJH_m@23*+#";

// ============================================================
// OLED CONFIGURATION
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
// JOYSTICK CONFIGURATION
// ============================================================

#define JOY_X 4
#define JOY_Y 5
#define JOY_SW 6

// Measured joystick center
#define JOY_CENTER_X 1911
#define JOY_CENTER_Y 1899

// Extreme-only thresholds
#define JOY_LEFT_THRESHOLD   500
#define JOY_RIGHT_THRESHOLD  3300

#define JOY_UP_THRESHOLD     500
#define JOY_DOWN_THRESHOLD   3300

// ============================================================
// PAGES
// ============================================================

enum Page {

  PAGE_DASHBOARD,
  PAGE_WIFI,
  PAGE_INTERNET,
  PAGE_NETWORK,
  PAGE_SCANNER,
  PAGE_SYSTEM,

  PAGE_COUNT
};

volatile int currentPage = PAGE_DASHBOARD;

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

// ============================================================
// BUTTON STATE
// ============================================================

bool previousButton = HIGH;

unsigned long lastButtonTime = 0;

#define BUTTON_DEBOUNCE 200

// ============================================================
// DISPLAY TIMING
// ============================================================

unsigned long lastDisplayUpdate = 0;

#define DISPLAY_INTERVAL 150

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

WiFiNetwork networks[15];

volatile int networkCount = 0;

volatile int selectedNetwork = 0;

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

void scanWiFi();

void checkNetwork();

void connectWiFi();

void drawDisplay();

// ============================================================
// GET JOYSTICK DIRECTION
// ============================================================

JoyDirection getJoystickDirection() {

  int x = analogRead(JOY_X);

  int y = analogRead(JOY_Y);

  // ----------------------------------------------------------
  // LEFT
  // ----------------------------------------------------------

  if (x <= JOY_LEFT_THRESHOLD) {

    return JOY_LEFT;
  }

  // ----------------------------------------------------------
  // RIGHT
  // ----------------------------------------------------------

  if (x >= JOY_RIGHT_THRESHOLD) {

    return JOY_RIGHT;
  }

  // ----------------------------------------------------------
  // UP
  // ----------------------------------------------------------

  if (y <= JOY_UP_THRESHOLD) {

    return JOY_UP;
  }

  // ----------------------------------------------------------
  // DOWN
  // ----------------------------------------------------------

  if (y >= JOY_DOWN_THRESHOLD) {

    return JOY_DOWN;
  }

  // ----------------------------------------------------------
  // CENTER
  // ----------------------------------------------------------

  return JOY_NONE;
}

// ============================================================
// PROCESS JOYSTICK
// ============================================================

void processJoystick() {

  JoyDirection direction =
    getJoystickDirection();

  // ----------------------------------------------------------
  // CENTER = RE-ARM
  // ----------------------------------------------------------

  if (direction == JOY_NONE) {

    lastDirection = JOY_NONE;

    return;
  }

  // ----------------------------------------------------------
  // SAME DIRECTION STILL HELD
  // ----------------------------------------------------------

  if (direction == lastDirection) {

    return;
  }

  // ----------------------------------------------------------
  // NEW EXTREME MOVEMENT
  // ----------------------------------------------------------

  lastDirection = direction;

  performJoystickAction(direction);
}

// ============================================================
// PERFORM JOYSTICK ACTION
// ============================================================

void performJoystickAction(
  JoyDirection direction
) {

  switch (direction) {

    // --------------------------------------------------------
    // LEFT
    // --------------------------------------------------------

    case JOY_LEFT:

      currentPage--;

      if (currentPage < 0) {

        currentPage =
          PAGE_COUNT - 1;
      }

      Serial.println(
        "JOYSTICK -> LEFT"
      );

      break;

    // --------------------------------------------------------
    // RIGHT
    // --------------------------------------------------------

    case JOY_RIGHT:

      currentPage++;

      if (
        currentPage >= PAGE_COUNT
      ) {

        currentPage = 0;
      }

      Serial.println(
        "JOYSTICK -> RIGHT"
      );

      break;

    // --------------------------------------------------------
    // UP
    // --------------------------------------------------------

    case JOY_UP:

      if (
        currentPage ==
        PAGE_SCANNER
      ) {

        if (
          selectedNetwork > 0
        ) {

          selectedNetwork--;
        }
      }

      Serial.println(
        "JOYSTICK -> UP"
      );

      break;

    // --------------------------------------------------------
    // DOWN
    // --------------------------------------------------------

    case JOY_DOWN:

      if (
        currentPage ==
        PAGE_SCANNER
      ) {

        if (
          selectedNetwork <
          networkCount - 1
        ) {

          selectedNetwork++;
        }
      }

      Serial.println(
        "JOYSTICK -> DOWN"
      );

      break;

    // --------------------------------------------------------
    // NONE
    // --------------------------------------------------------

    default:

      break;
  }
}

// ============================================================
// PROCESS JOYSTICK BUTTON
// ============================================================

void processButton() {

  bool button =
    digitalRead(JOY_SW);

  unsigned long now =
    millis();

  // ----------------------------------------------------------
  // BUTTON PRESS DETECTION
  // ----------------------------------------------------------

  if (
    button == LOW &&
    previousButton == HIGH &&
    now - lastButtonTime >
      BUTTON_DEBOUNCE
  ) {

    lastButtonTime = now;

    Serial.println(
      "JOYSTICK -> PRESS"
    );

    // --------------------------------------------------------
    // WIFI SCANNER
    // --------------------------------------------------------

    if (
      currentPage ==
      PAGE_SCANNER
    ) {

      scanWiFi();
    }

    // --------------------------------------------------------
    // OTHER PAGES
    // --------------------------------------------------------

    else {

      Serial.println(
        "Network refresh requested"
      );

      // Network task continuously updates
      // the network information.
    }
  }

  previousButton = button;
}

// ============================================================
// CONNECT TO WIFI
// ============================================================

void connectWiFi() {

  Serial.println();

  Serial.println(
    "============================"
  );

  Serial.println(
    "CONNECTING TO WIFI"
  );

  Serial.println(
    "============================"
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

  // ----------------------------------------------------------
  // SUCCESS
  // ----------------------------------------------------------

  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    wifiConnected = true;

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
      "SSID: "
    );

    Serial.println(
      currentSSID
    );

    Serial.print(
      "IP: "
    );

    Serial.println(
      localIP
    );

    Serial.print(
      "Gateway: "
    );

    Serial.println(
      gatewayIP
    );

    Serial.print(
      "RSSI: "
    );

    Serial.println(
      wifiRSSI
    );

    Serial.print(
      "Channel: "
    );

    Serial.println(
      wifiChannel
    );
  }

  // ----------------------------------------------------------
  // FAILURE
  // ----------------------------------------------------------

  else {

    wifiConnected = false;

    Serial.println(
      "WiFi connection FAILED"
    );
  }
}

// ============================================================
// PING GATEWAY
// ============================================================

void pingGateway() {

  if (!wifiConnected) {

    gatewayOnline = false;

    gatewayPing = -1;

    return;
  }

  bool result =
    Ping.ping(
      gatewayIP,
      2
    );

  if (result) {

    gatewayOnline = true;

    gatewayPing =
      Ping.averageTime();

  } else {

    gatewayOnline = false;

    gatewayPing = -1;
  }
}

// ============================================================
// INTERNET PING
// ============================================================

void pingInternet() {

  if (!wifiConnected) {

    internetOnline = false;

    internetPing = -1;

    packetLoss = 100;

    return;
  }

  const int packets = 5;

  int successful = 0;

  float totalPing = 0;

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

  // ----------------------------------------------------------
  // PACKET LOSS
  // ----------------------------------------------------------

  packetLoss =
    ((packets - successful) * 100)
    / packets;

  // ----------------------------------------------------------
  // INTERNET STATUS
  // ----------------------------------------------------------

  if (
    successful > 0
  ) {

    internetOnline = true;

    internetPing =
      totalPing /
      successful;

  } else {

    internetOnline = false;

    internetPing = -1;
  }
}

// ============================================================
// DNS TEST
// ============================================================

void testDNS() {

  if (!wifiConnected) {

    dnsTime = 0;

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

    dnsTime = 0;
  }
}

// ============================================================
// COMPLETE NETWORK CHECK
// ============================================================

void checkNetwork() {

  // ----------------------------------------------------------
  // WIFI STATUS
  // ----------------------------------------------------------

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    wifiConnected = false;

    gatewayOnline = false;

    internetOnline = false;

    gatewayPing = -1;

    internetPing = -1;

    packetLoss = 100;

    return;
  }

  wifiConnected = true;

  // ----------------------------------------------------------
  // WIFI INFORMATION
  // ----------------------------------------------------------

  wifiRSSI =
    WiFi.RSSI();

  wifiChannel =
    WiFi.channel();

  localIP =
    WiFi.localIP();

  gatewayIP =
    WiFi.gatewayIP();

  subnetMask =
    WiFi.subnetMask();

  dnsIP =
    WiFi.dnsIP();

  currentSSID =
    WiFi.SSID();

  currentBSSID =
    WiFi.BSSIDstr();

  // ----------------------------------------------------------
  // GATEWAY
  // ----------------------------------------------------------

  pingGateway();

  // ----------------------------------------------------------
  // INTERNET
  // ----------------------------------------------------------

  pingInternet();

  // ----------------------------------------------------------
  // DNS
  // ----------------------------------------------------------

  testDNS();

  // ----------------------------------------------------------
  // SERIAL OUTPUT
  // ----------------------------------------------------------

  Serial.println();

  Serial.println(
    "========== NETWORK =========="
  );

  Serial.print(
    "WiFi: "
  );

  Serial.println(
    wifiConnected
      ? "CONNECTED"
      : "DISCONNECTED"
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
    "============================"
  );

  Serial.println(
    "WIFI SCAN"
  );

  Serial.println(
    "============================"
  );

  // ----------------------------------------------------------
  // OLED SCANNING MESSAGE
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // SCAN
  // ----------------------------------------------------------

  int count =
    WiFi.scanNetworks(
      false,
      true
    );

  networkCount =
    min(
      count,
      15
    );

  selectedNetwork = 0;

  // ----------------------------------------------------------
  // STORE RESULTS
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // SERIAL
  // ----------------------------------------------------------

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
// DASHBOARD PAGE
// ============================================================

void drawDashboard() {

  drawHeader(
    "DASHBOARD"
  );

  // ----------------------------------------------------------
  // INTERNET STATUS
  // ----------------------------------------------------------

  display.setCursor(
    0,
    13
  );

  display.print(
    "NET "
  );

  display.println(
    internetOnline
      ? "ONLINE"
      : "OFFLINE"
  );

  // ----------------------------------------------------------
  // PING
  // ----------------------------------------------------------

  display.setCursor(
    0,
    25
  );

  display.print(
    "PING "
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

  // ----------------------------------------------------------
  // PACKET LOSS
  // ----------------------------------------------------------

  display.setCursor(
    0,
    37
  );

  display.print(
    "LOSS "
  );

  display.print(
    packetLoss
  );

  display.println(
    "%"
  );

  // ----------------------------------------------------------
  // RSSI
  // ----------------------------------------------------------

  display.setCursor(
    0,
    49
  );

  display.print(
    "RSSI "
  );

  display.print(
    wifiRSSI
  );

  display.println(
    "dBm"
  );
}

// ============================================================
// WIFI PAGE
// ============================================================

void drawWiFiPage() {

  drawHeader(
    "WIFI"
  );

  // SSID

  display.setCursor(
    0,
    13
  );

  display.print(
    "SSID:"
  );

  display.println(
    currentSSID
  );

  // RSSI

  display.setCursor(
    0,
    26
  );

  display.print(
    "RSSI:"
  );

  display.print(
    wifiRSSI
  );

  display.println(
    "dBm"
  );

  // CHANNEL

  display.setCursor(
    0,
    39
  );

  display.print(
    "CHANNEL:"
  );

  display.println(
    wifiChannel
  );

  // BSSID

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
// INTERNET PAGE
// ============================================================

void drawInternetPage() {

  drawHeader(
    "INTERNET"
  );

  // GATEWAY

  display.setCursor(
    0,
    13
  );

  display.print(
    "GW "
  );

  if (
    gatewayOnline
  ) {

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

  // INTERNET

  display.setCursor(
    0,
    27
  );

  display.print(
    "NET "
  );

  if (
    internetOnline
  ) {

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

  // DNS

  display.setCursor(
    0,
    41
  );

  display.print(
    "DNS "
  );

  if (
    dnsTime > 0
  ) {

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

  // LOSS

  display.setCursor(
    0,
    55
  );

  display.print(
    "LOSS "
  );

  display.print(
    packetLoss
  );

  display.println(
    "%"
  );
}

// ============================================================
// NETWORK PAGE
// ============================================================

void drawNetworkPage() {

  drawHeader(
    "NETWORK"
  );

  // IP

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

  // GATEWAY

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

  // MASK

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

  // DNS

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
// WIFI SCANNER PAGE
// ============================================================

void drawScannerPage() {

  drawHeader(
    "WIFI SCANNER"
  );

  // ----------------------------------------------------------
  // NO RESULTS
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // CURRENT NETWORK
  // ----------------------------------------------------------

  WiFiNetwork& n =
    networks[
      selectedNetwork
    ];

  // NUMBER

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

  // SSID

  display.setCursor(
    0,
    26
  );

  display.println(
    n.ssid
  );

  // RSSI + CHANNEL

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

  // ACTION

  display.setCursor(
    0,
    52
  );

  display.println(
    "PRESS=RESCAN"
  );
}

// ============================================================
// SYSTEM PAGE
// ============================================================

void drawSystemPage() {

  drawHeader(
    "SYSTEM"
  );

  // ----------------------------------------------------------
  // UPTIME
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // RAM
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // CPU
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // RSSI
  // ----------------------------------------------------------

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

  display.println(
    "dBm"
  );
}

// ============================================================
// DRAW DISPLAY
// ============================================================

void drawDisplay() {

  display.clearDisplay();

  switch (
    currentPage
  ) {

    case PAGE_DASHBOARD:

      drawDashboard();

      break;

    case PAGE_WIFI:

      drawWiFiPage();

      break;

    case PAGE_INTERNET:

      drawInternetPage();

      break;

    case PAGE_NETWORK:

      drawNetworkPage();

      break;

    case PAGE_SCANNER:

      drawScannerPage();

      break;

    case PAGE_SYSTEM:

      drawSystemPage();

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

    // Wait 5 seconds before
    // next network measurement.

    vTaskDelay(
      pdMS_TO_TICKS(
        5000
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

  Serial.println();

  Serial.println(
    "============================"
  );

  Serial.println(
    "ESP32-S3 NETWORK MONITOR"
  );

  Serial.println(
    "============================"
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
  // OLED I2C
  // ----------------------------------------------------------

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  // ----------------------------------------------------------
  // OLED INITIALIZATION
  // ----------------------------------------------------------

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

  // ----------------------------------------------------------
  // START SCREEN
  // ----------------------------------------------------------

  display.clearDisplay();

  display.setTextColor(
    SSD1306_WHITE
  );

  display.setTextSize(1);

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
    "Starting..."
  );

  display.display();

  delay(1000);

  // ----------------------------------------------------------
  // WIFI
  // ----------------------------------------------------------

  connectWiFi();

  delay(500);

  // ----------------------------------------------------------
  // CREATE NETWORK TASK
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

  Serial.println();

  Serial.println(
    "System ready!"
  );
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // JOYSTICK
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
  // SMALL YIELD
  // ----------------------------------------------------------

  delay(5);
}