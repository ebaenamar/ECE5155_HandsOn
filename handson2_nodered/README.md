# Hands-On 2: Node-RED IoT Automation & Dashboards

## Prerequisites

- **Node.js ≥ 18**: Check with `node --version`. Install from [nodejs.org](https://nodejs.org/) if needed.
- **Python 3** with `paho-mqtt`: for the MQTT simulator.
- Your Wokwi ESP32 project from Monday (or use the simulator below).

## Setup

### Option A: npm (recommended)

```bash
npm install -g --unsafe-perm node-red
node-red
```

### Option B: Docker

```bash
docker run -it -p 1880:1880 --name nodered nodered/node-red
```

Then open [http://localhost:1880](http://localhost:1880) in your browser.

### Install Dashboard Nodes

1. In Node-RED, click the **hamburger menu** (top right)
2. **Manage Palette** → **Install** tab
3. Search for `@flowfuse/node-red-dashboard` and install it
   - ⚠️ Do NOT install the old `node-red-dashboard` (deprecated)

## MQTT Simulator

If your Wokwi project is not available, use the provided simulator:

```bash
pip install paho-mqtt
python mqtt_simulator.py
```

**Important:** Edit `mqtt_simulator.py` and change `YOUR_NAME` in the topic to your actual name.

## Exercises

### Exercise 1: Receive MQTT Data (15 min)

Build a flow that subscribes to your MQTT topic and displays messages in the debug panel.

**Nodes needed:** `mqtt in` → `debug`

MQTT configuration:
- Server: `broker.hivemq.com`
- Port: `1883`
- Topic: `eece5155/YOUR_NAME/sensors`

### Exercise 2: Parse JSON + Dashboard (20 min)

Parse the incoming JSON and build a real-time dashboard with gauges and a chart.

**Nodes needed:** `mqtt in` → `json` → `function` → `ui-gauge` / `ui-chart`

Hints:
- The `json` node converts the raw MQTT string into a JavaScript object
- Use `function` nodes to extract individual fields from the parsed object
- Dashboard widgets need a **Group** and a **Page** configured

### Exercise 3: Threshold Alerts (15 min)

Add conditional logic: alert when a value crosses a threshold.

**Nodes needed:** `switch` → `function` → `ui-notification`

Hints:
- The `switch` node evaluates `msg.payload` against rules
- Think about what threshold makes sense for YOUR project (not just 30°C)

## Key Concept: The Message Object

Every piece of data in Node-RED flows as a `msg` object:

```json
{
  "topic": "eece5155/.../sensors",
  "payload": { "temp": 23.5, "hum": 65.2 },
  "_msgid": "abc123"
}
```

**Rule:** Every function node must end with `return msg;` or the message stops.

## Deliverables

1. **Export your flow**: Menu → Export → Download as `nodered_flow.json`
2. **Screenshot your dashboard** with live data
3. **Post both** on the Canvas discussion board

## Troubleshooting

| Problem | Fix |
|---------|-----|
| Node-RED won't start | Check `node --version` ≥ 18 |
| Permission errors (Mac) | `sudo npm install -g --unsafe-perm node-red` |
| Dashboard nodes missing | Install `@flowfuse/node-red-dashboard` from Manage Palette |
| MQTT "disconnected" | Verify broker: `broker.hivemq.com`, port `1883`, no auth |
| Dashboard blank at `/dashboard` | Create a Group + Page in widget config; verify data is flowing |
| Function node error | Check for missing `return msg;` at the end |
