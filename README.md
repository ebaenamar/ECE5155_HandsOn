# EECE5155 Hands-On Sessions

**Wireless Sensor Networks and IoT — Northeastern University — Spring 2026**

Code and circuit files for the hands-on lab sessions. Each folder contains ready-to-use Wokwi projects you can paste directly into [wokwi.com](https://wokwi.com/).

## Session 1: ESP32 Firmware on Wokwi (Mar 23)

📁 `handson1_wokwi/`

| Exercise | What you build | Folder |
|----------|---------------|--------|
| 1 | Read a virtual DHT22 (temp + humidity) | `exercise1_dht22/` |
| 2 | WiFi + MQTT publish to HiveMQ | `exercise2_wifi_mqtt/` |
| 3 | Structured JSON + multiple sensors (DHT22 + LDR) | `exercise3_json_multisensor/` |

### Quick start

1. Open [wokwi.com](https://wokwi.com/) → **Start from Scratch** → **ESP32**
2. Paste the `sketch.ino` into the code editor
3. Click the `diagram.json` tab and paste the diagram file
4. Add the libraries from `libraries.txt` (Library Manager or `libraries.txt` tab)
5. Press **Play**

For exercises 2 and 3: replace `YOUR_NAME` in the MQTT topic before running.

Verify your MQTT messages at [HiveMQ WebSocket Client](https://www.hivemq.com/demos/websocket-client/) — subscribe to `eece5155/YOUR_NAME/sensors`.

## Session 2: Node-RED + MQTT (Mar 25)

Coming soon.

## Session 3: IoT Dataset Analysis (Mar 26)

Coming soon.
