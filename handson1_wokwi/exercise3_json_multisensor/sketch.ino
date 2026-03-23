// Exercise 3: Structured JSON + Multiple Sensors on Wokwi
// EECE5155 Hands-On Session 1
// Wiring: DHT22 on GPIO 15, LDR on GPIO 34
// IMPORTANT: Replace YOUR_NAME with your actual name before running

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define LDR_PIN 34
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient mqtt(espClient);
DHT dht(DHTPIN, DHTTYPE);

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println(" Connected! IP: " + WiFi.localIP().toString());
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "esp32-" + String(random(0xffff), HEX);
    if (mqtt.connect(clientId.c_str())) {
      Serial.println(" Connected!");
    } else {
      Serial.printf(" Failed (rc=%d). Retrying in 5s...\n",
                     mqtt.state());
      delay(5000);
    }
  }
}

void publishSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int light = analogRead(LDR_PIN);  // 0-4095 (12-bit ADC)
  float voltage = light * (3.3 / 4095.0);

  // Structured JSON with metadata
  String json = "{";
  json += "\"device_id\":\"esp32-node-01\",";
  json += "\"timestamp\":" + String(millis()) + ",";
  json += "\"sensors\":{";
  json += "\"temperature\":{\"value\":" + String(temp,1)
        + ",\"unit\":\"C\"},";
  json += "\"humidity\":{\"value\":" + String(hum,1)
        + ",\"unit\":\"%\"},";
  json += "\"light\":{\"raw\":" + String(light)
        + ",\"voltage\":" + String(voltage,2) + "}";
  json += "}}";

  mqtt.publish("eece5155/YOUR_NAME/data", json.c_str());
  Serial.println("Published: " + json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWiFi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
  publishSensorData();
  delay(10000);
}
