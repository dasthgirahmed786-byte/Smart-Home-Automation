#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// WiFi credentials for Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Pin definitions
#define LIGHT_PIN 26
#define FAN_PIN 27
#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

bool lightState = false;
bool fanState = false;

// Create the web dashboard
void handleRoot() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  String tempText = isnan(temperature)
      ? "Sensor error" : String(temperature, 1) + " &deg;C";

  String humText = isnan(humidity)
      ? "Sensor error" : String(humidity, 1) + " %";

  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Smart Home Dashboard</title>
    <meta name="viewport"
          content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="5">
    <style>
      body {
        font-family: Arial;
        text-align: center;
        background: #eef2f3;
        margin: 30px;
      }
      .card {
        background: white;
        padding: 20px;
        margin: 15px auto;
        max-width: 400px;
        border-radius: 12px;
        box-shadow: 0 2px 8px #aaa;
      }
      button {
        padding: 12px 25px;
        font-size: 17px;
        margin: 8px;
        border: none;
        border-radius: 8px;
        color: white;
        cursor: pointer;
      }
      .on { background: #4CAF50; }
      .off { background: #f44336; }
    </style>
  </head>
  <body>
    <h1>Smart Home Dashboard</h1>

    <div class="card">
      <h2>Living Room Light</h2>
      <h3>)rawliteral";

  html += lightState ? "ON" : "OFF";

  html += R"rawliteral(</h3>
      <a href="/light/on">
        <button class="on">Turn ON</button>
      </a>
      <a href="/light/off">
        <button class="off">Turn OFF</button>
      </a>
    </div>

    <div class="card">
      <h2>Fan</h2>
      <h3>)rawliteral";

  html += fanState ? "ON" : "OFF";

  html += R"rawliteral(</h3>
      <a href="/fan/on">
        <button class="on">Turn ON</button>
      </a>
      <a href="/fan/off">
        <button class="off">Turn OFF</button>
      </a>
    </div>

    <div class="card">
      <h2>Room Conditions</h2>
      <p>Temperature: )rawliteral";

  html += tempText;

  html += R"rawliteral(</p>
      <p>Humidity: )rawliteral";

  html += humText;

  html += R"rawliteral(</p>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Light controls
void handleLightOn() {
  lightState = true;
  digitalWrite(LIGHT_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLightOff() {
  lightState = false;
  digitalWrite(LIGHT_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

// Fan controls
void handleFanOn() {
  fanState = true;
  digitalWrite(FAN_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleFanOff() {
  fanState = false;
  digitalWrite(FAN_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  dht.begin();

  // Connect to Wokwi WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED &&
         attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Configure web server routes
    server.on("/", handleRoot);
    server.on("/light/on", handleLightOn);
    server.on("/light/off", handleLightOff);
    server.on("/fan/on", handleFanOn);
    server.on("/fan/off", handleFanOff);

    server.begin();
    Serial.println("Web server started!");
    Serial.println("Open the IP address above.");
  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.println("Restart the simulation and try again.");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }
}