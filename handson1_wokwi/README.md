# Hands-On Session 1: ESP32 Firmware on Wokwi Simulator

**EECE5155 — Wireless Sensor Networks and IoT — Spring 2026**

Browser only. No install, no hardware, no cloud account.

## How to use

1. Go to [wokwi.com](https://wokwi.com/)
2. Click **Start from Scratch** → **ESP32**
3. Copy the `sketch.ino` code into the editor
4. Copy the `diagram.json` into the diagram tab (click the diagram.json tab)
5. Add the libraries listed in `libraries.txt` (click the Library Manager icon or edit `libraries.txt` tab)
6. Press the green **Play** button

## Exercises

### Exercise 1 — Read a Virtual DHT22 (`exercise1_dht22/`)

Read temperature and humidity from a simulated DHT22 sensor. Includes error handling for sensor failures.

**What to test:**
- Serial Monitor shows readings every 2 seconds
- Click on the DHT22 in the simulator to change values
- Disconnect the data wire to see error handling

### Exercise 2 — WiFi + MQTT Publish (`exercise2_wifi_mqtt/`)

Connect to WiFi using Wokwi's virtual network and publish sensor data to a public MQTT broker.

**Before running:** replace `YOUR_NAME` in the topic string with your actual name.

**What to test:**
- Serial Monitor shows WiFi connection and MQTT publishes
- Open [HiveMQ WebSocket Client](https://www.hivemq.com/demos/websocket-client/) and subscribe to `eece5155/YOUR_NAME/sensors`
- You should see JSON payloads arriving every 10 seconds

### Exercise 3 — Structured JSON + Multiple Sensors (`exercise3_json_multisensor/`)

Add a photoresistor (LDR) and publish structured JSON with metadata (device_id, timestamp, nested sensor objects).

**Before running:** replace `YOUR_NAME` in the topic string with your actual name.

**What to test:**
- JSON includes temperature, humidity, and light readings
- Click the LDR in the simulator to change light levels
- Verify the JSON parses correctly in HiveMQ client

## Wokwi Resources

- Docs: https://docs.wokwi.com/
- ESP32 guide: https://docs.wokwi.com/guides/esp32
- WiFi simulation: https://docs.wokwi.com/guides/esp32-wifi
- Component reference: https://docs.wokwi.com/parts/wokwi-dht22

## Adapt to your project

Wokwi supports: DHT22, DS18B20, HC-SR04, PIR, potentiometer, LDR, MPU6050, servo, LCD, NeoPixel, and more.

If your BOM sensor is not available in Wokwi, use a potentiometer as an analog proxy and map 0–4095 to your sensor's range.
