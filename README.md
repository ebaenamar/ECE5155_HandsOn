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

## Session 2: Node-RED IoT Automation & Dashboards (Mar 25)

📁 `handson2_nodered/`

| Exercise | What you build | Nodes used |
|----------|---------------|------------|
| 1 | Subscribe to MQTT and see data in debug panel | `mqtt in` → `debug` |
| 2 | Parse JSON + live dashboard with gauges and chart | `json` → `function` → `ui-gauge` / `ui-chart` |
| 3 | Threshold alerts with conditional logic | `switch` → `function` → `ui-notification` |

### Quick start

1. Install Node-RED: `npm install -g --unsafe-perm node-red` (requires Node.js ≥ 18)
2. Run `node-red` and open [http://localhost:1880](http://localhost:1880)
3. Install dashboard nodes: Menu → Manage Palette → Install → `@flowfuse/node-red-dashboard`
4. Start your data source (Wokwi from Session 1, or `python mqtt_simulator.py`)

Replace `YOUR_NAME` in `mqtt_simulator.py` before running.

### Deliverables

1. Export your flow as `nodered_flow.json` (Menu → Export → Download)
2. Screenshot your dashboard with live data
3. Post both on Canvas

## Session 3: IoT Dataset Analysis (Mar 26)

📁 `handson3_dataset/`

| Exercise | What you build | Key skill |
|----------|---------------|-----------|
| 1 | Data quality audit: missing values, outliers, sensor failures | Diagnose deployment health from data |
| 2 | Sensor comparison: correlation, offset, drift | Validate sensor accuracy |
| 3 | Anomaly detection: fixed thresholds vs rolling z-score | Detect real events vs sensor errors |

### Quick start

```bash
pip install pandas matplotlib jupyter
curl -L -o data.txt.gz "https://db.csail.mit.edu/labdata/data.txt.gz"
gunzip data.txt.gz
jupyter notebook dataset_starter.ipynb
```

### Rules

Use **any AI tool** to generate code. **You** write the interpretation in the LaTeX report. Grading: 40% code · 60% interpretation.

### Deliverables

1. LaTeX report (PDF) — use `dataset_report_template.tex`
2. Three plots embedded in report: `data_quality.png`, `sensor_comparison.png`, `anomalies.png`
3. Jupyter notebook (.ipynb) with your code
4. Submit all on Canvas
