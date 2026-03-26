# Hands-On 2: Node-RED IoT Automation — Speech Script
**Date**: Wednesday, March 25, 2026  
**Duration**: 60 minutes  
**Audience**: EECE5155 students (graduate level)  
**Context**: Students have Wokwi ESP32 firmware from Monday. No hardware yet (arrives Mar 29).

---

## Opening (3 min) — Slides 1-4

Welcome back. On Monday we built the device side: ESP32 firmware that reads sensors and publishes MQTT. Today we build the other half: what happens when the data arrives.

We are going to use Node-RED. It is an open-source, visual programming tool for IoT, built on Node.js. We drag nodes, connect them with wires, and we have a data pipeline. IBM created it in 2013, and it is used in production by Bosch, Siemens, Hitachi, and across industrial IoT. It is a real middleware tool used in the field, not just for teaching.

**Why Node-RED instead of writing backend code?** Because for an MVP, speed matters. We could write a Python script that subscribes to MQTT, parses JSON, checks thresholds, stores to a database, and serves a dashboard. That is maybe 300 lines across multiple files. Or we wire up 6 nodes in Node-RED and have it running in 10 minutes. For prototyping, that trade-off is clear.

Beyond dashboards, Node-RED also handles actuation (sending commands back to devices), data fusion (combining multiple sensor streams), and protocol bridging (MQTT to HTTP to Modbus). We are only scratching the surface today, but the tool scales to real deployments.

Let's get it installed.

---

## Step 0: Install Node-RED (5 min) — Slides 6-7

Two options. If we have Node.js installed (check with `node --version`, we need version 18 or higher), just run `npm install -g --unsafe-perm node-red`. Then type `node-red` and it starts.

If we still have Docker from the cloud hands-on two weeks ago, even easier: `docker run -it -p 1880:1880 --name nodered nodered/node-red`. Done.

Either way, open the browser to localhost:1880. We should see the Node-RED flow editor: node palette on the left, canvas in the middle, info/debug panel on the right.

Before we start building flows, we need one extra thing: the dashboard nodes. Go to the hamburger menu (top right), click "Manage Palette," go to the "Install" tab, and search for `@flowfuse/node-red-dashboard`. Install it. This is Dashboard 2.0, not the old deprecated node-red-dashboard. It gives us gauges, charts, and notification widgets.

Everyone got Node-RED running with the dashboard installed? Good. Let's wire up our first flow.

---

## Exercise 1: Receive MQTT Data (15 min) — Slide 8

Here is what we are going to do: subscribe to the MQTT topic that our Wokwi ESP32 publishes to, and see the data arrive in Node-RED.

From the left palette, drag an **mqtt in** node onto the canvas. Double-click it to configure. For the server, add a new one: broker.hivemq.com, port 1883. For the topic: `eece5155/YOUR_NAME/sensors`, same topic we used on Monday.

Now drag a **debug** node from the palette and connect them with a wire. Click the output port of the mqtt node and drag to the input port of the debug node. That is it. Our first flow.

Click the red **Deploy** button in the top right. Node-RED is now live and listening.

Now we need data flowing. Two options:

**Option A**: Open Wokwi in another tab and run the Monday project. It should start publishing MQTT messages immediately.

**Option B**: If Wokwi is not available, use the Python MQTT simulator on the slide. It connects to HiveMQ and publishes random temperature and humidity every 5 seconds. Either way works.

Look at the debug panel on the right side of Node-RED. We should see messages arriving: JSON strings with temp and humidity, every 5 or 10 seconds.

**What just happened?** We built a data receiver in about 2 minutes. No code. No server setup. Two nodes and a wire. That is why Node-RED works well for prototyping.

Notice the progression we are following: receive first, then visualize, then automate. That is the same progression we would follow in any real IoT project. So let's move to visualization.

---

## Exercise 2: Parse JSON + Dashboard (20 min) — Slides 9-11

Now let's make it visual. We are going to parse the incoming JSON and create a real-time dashboard with gauges and a time-series chart.

First, the incoming MQTT message is a string: `{"temp":23.5,"hum":65.2}`. We need to parse it into a JavaScript object so Node-RED can work with the individual fields.

Drag a **json** node from the palette and put it between the mqtt-in and debug nodes. The json node automatically parses strings into objects and vice versa. Deploy.

Now check the debug panel. Instead of seeing a string, we should see an object with `temp` and `hum` properties. That is the parsed data.

Next, we need to extract the temperature value. Drag a **function** node and connect it after the json node. Double-click it, and write one line: `msg.payload = msg.payload.temp; return msg;`. That takes the full JSON object and extracts just the temperature.

Now here is where the dashboard comes in. From the "Dashboard 2.0" section of the palette, drag a **ui-gauge** node. Connect it to the temperature function node. Double-click the gauge to configure:

- Create a new group called "Sensors"
- Create a new tab called "My MVP"
- Set the range from 0 to 50
- Set the units to °C
- Give it a label like "Temperature"

Deploy. Now open a new tab to `localhost:1880/dashboard`. We should see the dashboard with a temperature gauge updating in real time.

Do the same for humidity: another function node that extracts `msg.payload.hum`, another ui-gauge with range 0-100 and units "%".

Finally, add a **ui-chart** node connected to the temperature output. This gives us a time-series plot: the last few minutes of temperature data, updating live. This is essentially what the MVP demo will look like.

Let's take a step back. Our Wokwi ESP32 reads a sensor, publishes to MQTT, Node-RED receives it, parses it, and displays it on a live dashboard. End to end. No backend code. Running entirely on our laptop.

One thing to notice: we chose a gauge range of 0-50. Why? Because we are measuring indoor temperature. If we were monitoring a freezer, the range would be -25 to 5. A greenhouse, maybe 10 to 45. **We set the range because we understand the physical context of our deployment.** Same reasoning applies to the thresholds we add next.

---

## Exercise 3: Threshold Alerts (15 min) — Slides 12-13

Now let's add automation logic. A dashboard that shows data is informative, but a system that responds to data is useful.

We are going to add a **switch** node. This is Node-RED's conditional routing: if temperature is above 30°C, send an alert. Otherwise, let it pass normally.

Take the wire coming out of the temperature function node and connect it to a new switch node. Double-click the switch to configure: property is `msg.payload`, rule 1 is "> 30" going to output 1, rule 2 is "otherwise" going to output 2.

On output 1 (the alert path), add a function node that formats the message: `msg.payload = "ALERT: Temp " + msg.payload + "C exceeds 30C!"; return msg;`. Connect that to a **ui-notification** node from the Dashboard 2.0 section. This pops up a toast notification on our dashboard.

On output 2 (the normal path), connect a debug node so we can see normal readings are still flowing.

Deploy. Now go back to the Wokwi tab and click on the DHT22 sensor. Set the temperature above 30°C. Switch to the Node-RED dashboard tab. We should see a notification pop up.

Set it back below 30. The notification stops.

**This is the PES implemented as code.** The PES says something like "alert when temperature exceeds threshold X." The switch node IS that requirement, running. In the final report, we can write: "Threshold alerting was implemented using Node-RED flow logic, triggered at the PES-defined limit of 30°C."

Now, **adapt this to each project.** What threshold makes sense?

- Soil moisture below 20%: drought alert.
- Vibration above 2g: machine failure warning.
- CO2 above 1000 ppm: ventilation alert.
- Cold chain temperature outside 2-8°C: compliance violation.

Change the switch node threshold to match the PES requirement for your project. Change the alert message to match your domain. The threshold value depends entirely on deployment context: 30°C means something different for an HVAC system, a greenhouse, and a server room. We define the threshold because we understand what our system is designed to detect.

---

## Wrap-Up (5 min) — Slides 14-16

Three deliverables before we leave:

1. **Export the Node-RED flow**: Menu → Export → Clipboard → Download as JSON file. Save it as `nodered_flow.json`. We will need this for the final report and for when we connect real hardware.

2. **Screenshot the dashboard** with live data showing: the gauges, the chart, ideally with an alert triggered. This goes in the portfolio.

3. **Post both** on the Canvas discussion board.

Let's look at where we are after two sessions:

Monday we built the device side: ESP32 firmware that reads sensors and publishes MQTT. That is the embedded layer.

Today we built the automation side: Node-RED receives data, parses it, visualizes it, and applies logic. That is the middleware layer.

Tomorrow, Thursday, we do something different: analyzing real IoT datasets from actual deployments. We will see the data quality issues (missing values, outliers, sensor drift) that come up in real systems.

Then starting Monday the 30th, everything connects. We flash the Wokwi firmware onto the real ESP32, it publishes real sensor data to MQTT, the Node-RED flow picks it up and shows it on the dashboard. The entire stack, end to end, working with physical sensors.

**Portfolio note**: the Node-RED flow JSON + dashboard screenshot = evidence of "IoT data pipeline design with automation logic and real-time visualization." That is a concrete, demonstrable engineering skill.

See you tomorrow. Bring laptops; we will be working with Python and Jupyter notebooks.

---

## Quick Reference: Common Issues

**"Node-RED won't start"**: Check Node.js version: `node --version`. Must be ≥18. If you get permission errors on Mac, try `sudo npm install -g --unsafe-perm node-red`.

**"Dashboard nodes not in the palette"**: You need to install them: Menu → Manage Palette → Install → search `@flowfuse/node-red-dashboard` (not the old `node-red-dashboard`, which is deprecated). Click Install.

**"MQTT node shows 'disconnected'"**: Check the broker config: `broker.hivemq.com`, port `1883`, no username/password. Make sure your laptop has internet access.

**"Dashboard shows nothing at /dashboard"**: Make sure you created a Group and Page in the gauge/chart node config. Both are required. Also verify data is actually flowing; check the debug panel first.

**"My function node gives an error"**: Most common mistake: forgetting `return msg;` at the end. Node-RED functions must return the message object.

**"Can we connect Node-RED to InfluxDB from the cloud hands-on?"**: Yes! Install `node-red-contrib-influxdb` from Manage Palette. Add an influxdb-out node after your json parse node. Configure it with your InfluxDB connection from the Docker setup. This gives you persistent storage + Grafana dashboards on top of the Node-RED dashboard.
