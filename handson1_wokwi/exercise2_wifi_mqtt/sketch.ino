// Exercise 2: WiFi + MQTT Publish on Wokwi
// EECE5155 Hands-On Session 1
// Wiring: same as Exercise 1 (DHT22 on GPIO 15)
// IMPORTANT: Replace YOUR_NAME with your actual name before running

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define WIFI_SSID "Wokwi-GUEST"  // Wokwi virtual WiFi
#define WIFI_PASS ""
#define MQTT_BROKER "broker.hivemq.com"  // Free public broker
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

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectWiFi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    String payload = "{\"temp\":" + String(t,1) +
                     ",\"hum\":" + String(h,1) + "}";
    mqtt.publish("eece5155/YOUR_NAME/sensors", payload.c_str());
    Serial.println("Published: " + payload);
  }
  delay(10000);  // Publish every 10 seconds
}
