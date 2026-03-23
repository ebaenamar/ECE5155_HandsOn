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

## Challenge: Adapt to YOUR MVP Project (`challenge/`)

**Due: Wednesday March 25, end of day.**

You have a starter sketch with WiFi + MQTT already working. Your job: wire up the sensor closest to your BOM, read it, build a JSON payload, and publish it.

### Which Wokwi sensor should I use?

| Your project | BOM sensor | Use in Wokwi | Notes |
|---|---|---|---|
| Hubert — Beacon Bin | PIR (HC-SR501) | **PIR** | Digital pin, HIGH on motion |
| Timothy — Smart Watering | SEN0193 (soil moisture) + DHT22 | **Potentiometer** (soil) + **DHT22** | Map pot 0–4095 → moisture 0–100% |
| Sergey — Crop Cop | HC-SR501 + OV7670 + AM2302 | **PIR** + **DHT22** | Camera has no Wokwi equivalent, skip it |
| Aiisha — Acoustic Monitor | MAX9814 (microphone) | **Potentiometer** | Map pot 0–4095 → dB 30–120 |
| William — TrekTrak | VL53L7CX (ToF) | **HC-SR04** (ultrasonic) | Both measure distance, similar logic |
| Will — MotionSense | MPU-6050 (IMU) | **MPU6050** | Direct match, I2C on GPIO 21/22 |

### What to do

1. Open the `challenge/sketch.ino` in Wokwi
2. Complete **TODO 1–6** in the code:
   - Add your sensor library and pin definitions
   - Create the sensor object
   - Read the sensor in `publishSensorData()`
   - Handle errors (what if the sensor returns NaN or -1?)
   - Build a JSON string with `device_id`, `timestamp`, and your readings with units
   - Set your MQTT topic to `eece5155/<your_name>/<project>/data`
3. Add your sensor component to the board (click **+** in Wokwi)
4. Wire it and test
5. Verify messages arrive at [HiveMQ WebSocket Client](https://www.hivemq.com/demos/websocket-client/)

### Deliverable

Post on Canvas:
1. Your **Wokwi project link** (save the project first)
2. A **screenshot** of HiveMQ receiving at least 3 of your JSON messages

### Grading

| Criteria | Points |
|---|---|
| Sensor reads correctly in Serial Monitor | 30 |
| JSON has device_id, timestamp, sensor value + unit | 30 |
| MQTT topic follows the format `eece5155/<name>/<project>/data` | 10 |
| Error handling (code checks for invalid readings) | 20 |
| Wokwi link + HiveMQ screenshot posted on Canvas | 10 |

## Wokwi Resources

- Docs: https://docs.wokwi.com/
- ESP32 guide: https://docs.wokwi.com/guides/esp32
- WiFi simulation: https://docs.wokwi.com/guides/esp32-wifi
- Component reference: https://docs.wokwi.com/parts/wokwi-dht22

If your sensor has no Wokwi equivalent, use a potentiometer as analog proxy and map 0–4095 to your sensor's output range.
