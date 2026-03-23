// Exercise 1: Read a Virtual DHT22 on Wokwi
// EECE5155 Hands-On Session 1
// Wiring: VCC -> 3.3V, GND -> GND, Data -> GPIO 15

#include <DHT.h>
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("DHT22 initialized");
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("ERROR: Failed to read DHT22!");
    return;  // Handle sensor failure gracefully
  }
  Serial.printf("Temp: %.1f C | Humidity: %.1f %%\n", temp, hum);
  delay(2000);  // DHT22 min interval = 2 seconds
}
