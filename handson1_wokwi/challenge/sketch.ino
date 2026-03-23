// ============================================================
// EECE5155 Session 1 Challenge: Adapt to YOUR MVP Project
// ============================================================
// WiFi + MQTT boilerplate is done for you.
// Your job: add YOUR sensor, YOUR topic, YOUR JSON schema.
//
// Deliverable: post your Wokwi project link on Canvas
//              before Wednesday March 25 end of day.
// ============================================================

#include <WiFi.h>
#include <PubSubClient.h>

// --- TODO 1: Add your sensor library -------------------------
// Example: #include <DHT.h>
// Example: #include <Wire.h>  (for I2C sensors like MPU6050)
// Your library here:


// --- TODO 2: Define your sensor pins -------------------------
// Example: #define DHTPIN 15
// Example: #define TRIG_PIN 5  (for HC-SR04)
// Your pins here:


// --- WiFi + MQTT (done for you) ------------------------------
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient mqtt(espClient);

// --- TODO 3: Create your sensor object -----------------------
// Example: DHT dht(DHTPIN, DHTTYPE);
// Your sensor object here:


// --- WiFi connection (done for you) --------------------------
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected! IP: " + WiFi.localIP().toString());
}

// --- MQTT connection (done for you) --------------------------
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "esp32-" + String(random(0xffff), HEX);
    if (mqtt.connect(clientId.c_str())) {
      Serial.println(" Connected!");
    } else {
      Serial.printf(" Failed (rc=%d). Retrying in 5s...\n", mqtt.state());
      delay(5000);
    }
  }
}

// --- TODO 4: Read your sensor and publish --------------------
// Build a JSON string with:
//   - "device_id": your name or project name
//   - "timestamp": millis()
//   - your sensor readings with units
//
// Handle errors! What if the sensor returns NaN or -1?
//
void publishSensorData() {
  // --- Read your sensor here ---

  // --- Check for errors here ---

  // --- Build your JSON string here ---
  // Example format:
  // {"device_id":"hubert-beacon-bin","timestamp":12345,
  //  "sensors":{"proximity":{"value":42,"unit":"cm"}}}

  String json = "{}";  // <-- Replace this with your JSON

  // --- TODO 5: Set your MQTT topic ---------------------------
  // Format: eece5155/<your_name>/<project>/data
  // Example: eece5155/hubert/beacon-bin/data
  const char* topic = "eece5155/CHANGE_ME/data";

  mqtt.publish(topic, json.c_str());
  Serial.println("Published: " + json);
}

void setup() {
  Serial.begin(115200);

  // --- TODO 6: Initialize your sensor here ---

  connectWiFi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);

  Serial.println("=== YOUR PROJECT NAME HERE ===");
  Serial.println("Sensor initialized. Starting publish loop...");
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
  publishSensorData();
  delay(10000);  // Publish every 10 seconds
}
