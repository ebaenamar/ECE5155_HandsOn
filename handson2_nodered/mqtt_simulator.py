"""
MQTT Simulator for Node-RED Hands-On Session
EECE5155 — Wireless Sensor Networks and IoT

Publishes simulated temperature and humidity readings
to HiveMQ public broker every 5 seconds.

Usage:
    pip install paho-mqtt
    python mqtt_simulator.py

Change YOUR_NAME to your actual name before running.
"""

import paho.mqtt.client as mqtt
import json
import time
import random

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "eece5155/YOUR_NAME/sensors"  # <-- CHANGE THIS

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.connect(BROKER, PORT)
client.loop_start()

print(f"Publishing to {TOPIC} every 5 seconds...")
print("Press Ctrl+C to stop.\n")

try:
    while True:
        data = {
            "temp": round(random.uniform(18, 35), 1),
            "hum": round(random.uniform(40, 80), 1),
        }
        client.publish(TOPIC, json.dumps(data))
        print(f"Published: {data}")
        time.sleep(5)
except KeyboardInterrupt:
    print("\nStopped.")
    client.loop_stop()
    client.disconnect()
