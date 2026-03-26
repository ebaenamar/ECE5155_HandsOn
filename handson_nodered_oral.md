# Hands-On 2: Node-RED IoT Automation — Oral Script

**Date**: Wednesday, March 25, 2026
**Duration**: 60 minutes
**Assumes**: No prior Node-RED or Node.js experience. Students completed Monday's Wokwi ESP32 session.

---

## Opening (5 min) — Slides 1-2

Good afternoon. Welcome to the second hands-on session this week.

Let's recall where we are. On Monday we built the device side of an IoT system. We wrote firmware for an ESP32 microcontroller that reads a temperature sensor and publishes that data over MQTT to a cloud broker. Some of you used the Wokwi simulator. Some of you also ran the Python MQTT simulator we provided. Either way, you have data flowing to a broker on the internet right now, or you can start it in two minutes.

So the question for today is: what happens to that data after the broker receives it?

Think about it. Right now your sensor readings arrive at broker.hivemq.com and they just sit there. If nobody is subscribed, the messages are discarded. Nobody is storing them. Nobody is watching them. Nobody is acting on them. Your device is publishing into the void.

That's the gap we fill today. We're going to build the middleware layer: the piece of software that subscribes to your MQTT topics, receives the data, parses it, visualizes it on a live dashboard, and triggers alerts when values cross a threshold. And we're going to do it with a tool called Node-RED.

By the end of this session, you will have a working end-to-end IoT pipeline running on your laptop. Device to broker to middleware to dashboard. The same architecture pattern used in production IoT systems at scale.

---

## What is Node-RED? (5 min) — Slide 3

Node-RED is an open-source, visual programming tool designed for wiring together IoT devices, APIs, and online services. It runs on Node.js, which is a JavaScript runtime, and it gives you a browser-based editor where you build logic by dragging nodes onto a canvas and connecting them with wires.

Now, let's look at the history, because it matters for understanding why this tool exists and what problem it solves.

Node-RED was created in 2013 by Nick O'Leary and Dave Conway-Jones at IBM's Emerging Technology group in the UK. IBM was working on IoT projects and they kept running into the same problem: connecting a sensor to a database to a dashboard to an alert system required writing hundreds of lines of glue code. Every project was different, but the pattern was always the same. Read data from source A, transform it, send it to destination B. Over and over.

So they built a tool that makes that wiring visual. Instead of writing the glue code, you drag a node that represents the data source, drag another node that represents the destination, and draw a wire between them. The tool handles the plumbing. You focus on the logic.

IBM open-sourced it in 2013. It joined the JS Foundation in 2016 and is now part of the OpenJS Foundation. It has over 5,000 community-contributed nodes, which means there's a node for almost anything: databases, cloud services, hardware interfaces, machine learning models, messaging protocols. It's used in production by IBM, Bosch, Siemens, Hitachi, and a large number of industrial IoT deployments.

This is not a toy or a teaching-only tool. It's a real middleware platform used in the field. Learning it here gives you a skill that transfers directly to industry.

For your project specifically, here's what Node-RED does for you. It receives your MQTT sensor data. It parses the JSON. It applies threshold logic, your PES requirements implemented as code. It routes data to storage, to dashboards, to alert mechanisms. And it does all of this without you writing a backend server, managing HTTP routes, or configuring a database connector by hand.

---

## Flow-Based Programming (5 min) — Slide 4

Before we install anything, we need to understand the programming paradigm that Node-RED is built on, because it's probably different from anything you've used before.

Node-RED implements something called flow-based programming. This is a real computer science concept, not a marketing term. It was invented by J. Paul Morrison at IBM in the early 1970s. Morrison was working on batch processing systems for a Canadian bank, and he realized that many programs could be decomposed into independent processes connected by data streams. Each process is a black box: it takes data in, does something, and produces data out. The processes don't know about each other. They only know about the data they receive and the data they produce.

This is fundamentally different from the imperative programming you're used to in Python or C. In imperative programming, you write a sequence of instructions: do this, then do that, then do the other thing. The control flow is linear and tightly coupled. If you want to change one step, you often have to modify the steps around it.

In flow-based programming, you think in terms of data streams and transformations. Each node is an independent unit of work. Data flows from node to node through connections. You can rearrange the connections without modifying the nodes. You can replace one node with another as long as it accepts the same input format.

Let's make this concrete. Imagine you want to build the following pipeline in Python: subscribe to an MQTT topic, parse the JSON payload, check if the temperature exceeds 30 degrees, send an alert if it does, and display the value on a web dashboard. How much code is that? You'd need an MQTT client library, a JSON parser, conditional logic, probably Flask or FastAPI for the web server, some JavaScript for the dashboard frontend, WebSockets for real-time updates. Conservatively, that's 300 lines of code across multiple files, plus dependency management, error handling, and a process to keep it running.

In Node-RED, that's six nodes and five wires. Each node encapsulates one responsibility. The mqtt-in node handles the MQTT subscription. The json node handles parsing. The switch node handles the conditional. The ui-notification node handles the alert. The ui-gauge node handles the display. You wire them together on a canvas, click Deploy, and it's running.

Now, does that mean Node-RED replaces writing code? No. For complex business logic, custom algorithms, or high-performance systems, you'll write code. But for the middleware layer of an IoT prototype, for the rapid iteration phase where you're figuring out what your pipeline should look like, flow-based programming is dramatically faster. And that's exactly where you are in your project right now.

---

## Where Node-RED Fits in the IoT Pipeline (5 min) — Slide 5

Look at the architecture diagram on the slide. This is a standard IoT reference architecture, and we should internalize it because every IoT system you'll ever build follows some variation of this pattern.

At the bottom you have the **device layer**. That's your ESP32 with sensors. You built this on Monday. It reads physical quantities, temperature, humidity, light, whatever your project measures, and it converts them to digital data.

Above that, the **transport layer**. That's MQTT running over WiFi. Your device publishes JSON messages to a topic on a broker. The broker is a relay station: it receives messages from publishers and forwards them to subscribers. You're using HiveMQ's free public broker. In production, you'd run your own Mosquitto instance, probably in a Docker container.

Above that, the **middleware layer**. This is where Node-RED sits. Middleware is the software that connects the device world to the application world. It subscribes to the MQTT topics, receives the raw data, and does three things with it. First, it transforms the data: parsing JSON, extracting fields, converting units, validating values. Second, it routes the data: sending it to a database for storage, to a dashboard for visualization, to an alerting system for notifications. Third, it applies logic: threshold checks, rate limiting, aggregation, anomaly detection. This is where your PES requirements become executable code.

Above the middleware, you have the **application layer**: dashboards, databases, alert mechanisms, analytics tools. Node-RED's dashboard capability means we can build a basic version of this layer directly inside Node-RED itself, which is why it's so powerful for prototyping. In a production system, you'd have separate services: InfluxDB for time-series storage, Grafana for dashboards, a notification service for alerts. But for your MVP, Node-RED can serve as a one-stop middleware that also does visualization.

On Monday, you built the bottom two layers: device and transport. Today, you build the middle layer and the visualization. By the end of this session, you have a complete pipeline from sensor to screen. When hardware arrives on Saturday, the only thing that changes is the device layer becomes physical instead of simulated. Everything else stays the same.

---

## The Node-RED Interface (3 min) — Slide 6

When you open Node-RED in your browser, you'll see four main areas.

On the left, the **node palette**. This is your toolbox. Nodes are organized by category: common, function, network, dashboard, and so on. Each node represents one operation. You'll drag nodes from here onto the canvas.

In the center, the **flow canvas**. This is where you build your logic. You drag nodes here, position them, and draw wires between them. Each tab at the top is a separate flow. Think of flows like separate programs that run independently.

On the right, the **sidebar**. It has multiple tabs. The most important one for us is the debug tab, which shows the output of debug nodes. When you're building a flow, you'll use debug nodes constantly to verify that data is flowing correctly and that your transformations are doing what you expect.

At the top right, the **Deploy button**. This is critical to understand: nothing runs until you click Deploy. You can make all the changes you want on the canvas, but they're just a draft until you deploy. When you click Deploy, Node-RED pushes your flow to the runtime and it starts executing immediately.

Now, the most important concept in Node-RED: the **message object**. Every piece of data that flows between nodes is wrapped in a JavaScript object called `msg`. The key property is `msg.payload`, which carries the actual data. When an MQTT node receives a message, `msg.payload` contains the message body, in our case the JSON string from the ESP32. When you wire it to a function node, your function receives that same `msg` object. You modify `msg.payload` and return the `msg`. The next node receives it.

If you remember one thing from this explanation, remember this: your job in every function node is to transform `msg.payload` and then `return msg`. If you forget the `return msg` at the end, the message stops there and nothing downstream receives data. That is the single most common beginner mistake, and we guarantee at least a few of you will make it today. When your flow seems to stop working, check your function nodes for a missing return statement.

---

## Step 0: Install Node-RED (5 min) — Slide 7

Let's get it running. You need Node.js version 18 or higher. Check with `node --version` in your terminal. If you don't have it, go to nodejs.org and install the LTS version. That takes about two minutes.

Once you have Node.js, install Node-RED globally with npm: `npm install -g --unsafe-perm node-red`. The `--unsafe-perm` flag is needed on some systems to avoid permission errors during the build step.

If you still have Docker from the cloud infrastructure hands-on a couple weeks ago, there's an even faster option: `docker run -it -p 1880:1880 --name nodered nodered/node-red`. That gives you a containerized Node-RED with no system-level installation.

Either way, start Node-RED and open your browser to `http://localhost:1880`. You should see the flow editor we just described: palette on the left, canvas in the center, sidebar on the right.

Before we start building, we need the dashboard nodes. These don't come with the default installation. Go to the hamburger menu in the top right corner of the editor, click "Manage Palette", go to the "Install" tab, and search for `@flowfuse/node-red-dashboard`. Install that package. Note: there's also an older package called `node-red-dashboard` without the `@flowfuse` prefix. That one is deprecated. Make sure you install the one from FlowFuse, which is Dashboard 2.0.

Once installed, you'll see a new category in your node palette called "Dashboard 2.0" with nodes like ui-gauge, ui-chart, ui-text, and ui-notification. We'll use several of these.

Is everyone running? Good. Now, you also need data flowing. If your Wokwi project from Monday is still saved, open it in another tab and start the simulation. If not, open a terminal and run the Python MQTT simulator from the slide. Either way, you need MQTT messages arriving at `eece5155/YOUR_NAME/sensors` on the HiveMQ broker. Verify this is working before you proceed, because if there's no data, your Node-RED flow will sit there doing nothing and you won't know if you wired it correctly.

---

## Exercise 1: Receive MQTT Data (10 min) — Slide 8

Here's your first task, and we're going to give you the goal but not every click. You need to figure out the details.

**Goal**: Build a flow that subscribes to your MQTT topic and displays the incoming messages in the debug panel.

You need two nodes: an **mqtt in** node and a **debug** node. The mqtt in node needs to be configured with the broker address and your topic. The debug node just needs to be connected to the output of the mqtt node.

A few hints. When you double-click the mqtt in node, you'll need to add a new "Server" configuration. The broker address is `broker.hivemq.com`, port `1883`. No username, no password, no TLS. For the topic, use whatever you used on Monday: `eece5155/YOUR_NAME/sensors`.

Wire the two nodes together by clicking the output port of the mqtt node (the small gray square on the right side) and dragging to the input port of the debug node (the small gray square on the left side).

Click Deploy. Then look at the debug panel in the right sidebar. You should see messages appearing every few seconds, the raw JSON strings from your ESP32 or your Python simulator.

If you don't see anything, check three things. First, is your data source actually running? Second, does the topic in Node-RED match exactly what your device is publishing to? MQTT topics are case-sensitive. Third, check the status indicator under the mqtt node. It should say "connected". If it says "disconnected", your broker configuration is wrong.

Take a moment to consider what just happened. You built a data receiver with two nodes and one wire. No code. No server setup. No dependency management. In about 90 seconds of work, you have a live connection to an MQTT broker receiving real-time sensor data. This is why flow-based programming is powerful for prototyping.

But right now, you're just seeing raw strings in a debug panel. That's not useful to anyone. Let's make it visual.

---

## Exercise 2: Parse JSON and Build a Dashboard (20 min) — Slides 9-11

This is the core exercise. By the end of it, you'll have a live web dashboard with gauges and a time-series chart, all driven by your sensor data.

### Step 1: Parse the JSON

The MQTT message arrives as a string: `{"temp":23.5,"hum":65.2}`. Before Node-RED can work with individual fields, that string needs to be parsed into a JavaScript object.

Find the **json** node in the palette (it's under the "parser" category). Drag it onto the canvas and wire it between your mqtt in node and your debug node. Deploy.

Now look at the debug panel. Instead of seeing a string, you should see an object with `temp` and `hum` properties. Click on the object to expand it. You can see the structure. That's the parsed data.

If you still see a string, double-click the json node and make sure the "Action" is set to "Always convert to JavaScript Object."

### Step 2: Extract individual values

Now we need to think about this. The json node outputs an object with both `temp` and `hum`. But a gauge only shows one number. So you need to extract each value separately.

The tool for this is the **function** node. A function node lets you write a small snippet of JavaScript that transforms the message. Drag a function node onto the canvas. Connect it after the json node. Double-click it and write this:

```javascript
msg.payload = msg.payload.temp;
return msg;
```

That's it. Two lines. The first line replaces the full object with just the temperature value. The second line passes the message along to the next node.

Now here's what we want you to figure out on your own: you need a second function node that extracts humidity. Same pattern, different field. Wire it after the json node as well. You should have two parallel paths coming out of the json node: one for temperature, one for humidity.

### Step 3: Add dashboard gauges

From the "Dashboard 2.0" section of the palette, drag a **ui-gauge** node and connect it to your temperature function node. Double-click it to configure.

Dashboard 2.0 has a hierarchy: Base, Page, Group, Widget. The first time you add a widget, it will prompt you to create these. Create a group called "Temperature", on a page called "Sensors". Set the gauge range from 0 to 50 and the units to degrees Celsius. These ranges matter. If you're monitoring indoor temperature, 0 to 50 makes sense. If you're monitoring a freezer, you'd set -30 to 5. Your engineering context determines the range, not a default value.

Do the same for humidity: add a ui-gauge connected to your humidity function node. Create a group called "Humidity", range 0 to 100, units percent.

Deploy. Now open a new browser tab to `http://localhost:1880/dashboard`. You should see your dashboard with two gauges updating in real time.

### Step 4: Add a time-series chart

A gauge shows the current value. A chart shows the history. Drag a **ui-chart** node from the Dashboard 2.0 section. Connect it to the temperature function output, in parallel with the gauge. Create a new group called "History" and give the chart some reasonable time window.

Deploy and check your dashboard again.

### What you should have now

Take a step back and look at what's running. Your ESP32 (or simulator) reads a sensor. It publishes a JSON message to an MQTT broker on the internet. Node-RED, running on your laptop, subscribes to that topic. It receives the message, parses the JSON, extracts temperature and humidity, and displays them on a live web dashboard with gauges and a historical chart. End to end. No backend code. No cloud account. No database configuration.

This is a functional IoT dashboard. It's minimal, but it works. And for an MVP demonstration, "it works" is what matters. You can make it prettier later. Right now, focus on the fact that you have data flowing from a sensor to a screen, and every step in between is visible, debuggable, and modifiable without writing server code.

---

## Exercise 3: Threshold Alerts and Adaptation (15 min) — Slides 12-13

A dashboard that shows data is informative. A system that responds to data is useful. The difference is automation logic, and that's what makes an IoT system more than just a data logger.

Your PES document defines the conditions your system should detect and respond to. "Alert when temperature exceeds 30 degrees." "Notify when soil moisture drops below 20 percent." "Flag when vibration exceeds a safety threshold." Those requirements are the core of your project's value proposition. Today you implement one of them.

### The switch node

Node-RED has a node called **switch** that implements conditional routing. It evaluates a property of the incoming message against one or more rules and sends the message to different outputs based on which rule matches.

Here is your task. We're giving you the architecture, but you need to build it:

1. Take the output of your temperature function node and wire it into a **switch** node.
2. Configure the switch with two rules: one for values greater than 30, and one for "otherwise" (everything else). Each rule gets its own output.
3. On the "greater than 30" output, add a **function** node that formats a human-readable alert message. Something like: "ALERT: Temperature [value] C exceeds threshold."
4. Connect that to a **ui-notification** node from the Dashboard 2.0 palette. This pops up a toast notification on your dashboard.
5. On the "otherwise" output, connect a debug node so you can verify normal readings are still flowing through.

Deploy. Now go to your data source and make the temperature exceed 30. If you're using Wokwi, click the DHT22 and drag the slider above 30. If you're using the Python simulator, it generates random values between 18 and 30, so you might need to modify the range or wait for a value close to 30 and lower the threshold.

Switch to your dashboard tab. You should see a notification pop up when the threshold is exceeded. Set the temperature back below 30. The notifications stop.

### Adapt to your project

This is where the generic exercise becomes specific to your work. The threshold of 30 degrees Celsius for temperature is a placeholder. What threshold is meaningful for your project?

If you're building an agricultural monitoring system, maybe soil moisture below 20 percent triggers an irrigation alert. If you're doing cold chain monitoring, maybe temperature outside the 2 to 8 degree range is a compliance violation. If you're monitoring structural vibration, maybe acceleration above 2g is a maintenance alert.

Change the switch node threshold to match your PES requirement. Change the alert message to use your terminology. If you have time, add a second threshold. A warning level and a critical level, different notifications for each.

Think about what this means for your final report. You can write: "Threshold-based alerting was implemented using Node-RED flow logic. The system monitors [your parameter] and triggers an automated notification when the PES-defined threshold of [your value] is exceeded." That's a concrete, demonstrable implementation of a system requirement. That is engineering.

---

## Wrap-Up (5 min) — Slides 14-16

Three deliverables before you leave today.

First, **export your Node-RED flow**. Go to the hamburger menu in the editor, click "Export", select "Current Flow", and download it as a JSON file. Save it as `nodered_flow.json`. This file contains your entire flow definition. You can import it on any Node-RED instance and it rebuilds everything: nodes, wires, configurations. This is how you share and version-control Node-RED work.

Second, **take a screenshot of your dashboard** with live data visible. The gauges showing real values, the chart with a few minutes of history, ideally with an alert notification visible.

Third, **post both the JSON file and the screenshot** on the Canvas discussion board before Friday.

Let's put this week in perspective. You now have three layers of a working IoT system:

Monday you built the device layer. Firmware that reads sensors, handles errors, formats data as JSON, and publishes it over MQTT. That's the embedded systems piece.

Today you built the middleware and visualization layer. A Node-RED flow that subscribes to MQTT, parses data, routes it to dashboards and alerts, and implements threshold logic. That's the data engineering and automation piece.

Tomorrow, Thursday, we do something conceptually different. We analyze a real-world IoT dataset. Not simulated data, not clean textbook data, but actual sensor logs from a real deployment. You'll see the kinds of problems that happen when systems run for weeks or months: missing values, sensor drift, outliers, timestamp issues, duplicate records. These are the data quality challenges that determine whether an IoT system actually works or just appears to work during a demo.

Then starting Monday the 30th, we move to real hardware. You walk in with firmware tested in simulation, a Node-RED pipeline tested with simulated data, and an understanding of the data quality issues you'll face. You flash the firmware, you point Node-RED at the real data stream, and you have a working MVP.

For your portfolio: your Node-RED flow JSON file plus the dashboard screenshot are evidence of IoT data pipeline design with automation logic and real-time visualization. The flow itself shows your architectural thinking: how you decomposed the pipeline into discrete processing stages, how you implemented your PES requirements, how you routed data to multiple consumers. Include this in your final report.

Questions? See you tomorrow with your laptops. We'll be working in Python and Jupyter notebooks.

---

## Quick Reference: Common Issues

**"Node-RED won't start"**: Check Node.js version with `node --version`. Must be 18 or higher. If you get permission errors on macOS, try `sudo npm install -g --unsafe-perm node-red`.

**"I don't see dashboard nodes in the palette"**: You need to install them from Manage Palette. Search for `@flowfuse/node-red-dashboard`, not the old deprecated `node-red-dashboard`.

**"MQTT node shows 'disconnected'"**: Check the broker configuration. Server: `broker.hivemq.com`, port: `1883`. No username, no password, no TLS. Make sure your laptop has internet access.

**"Dashboard shows nothing at /dashboard"**: The `/dashboard` URL only activates after you deploy at least one Dashboard 2.0 widget node. Make sure you've deployed a flow containing a ui-gauge or ui-chart. Also verify that you created the required Group and Page in the widget configuration.

**"My function node gives an error"**: The most common mistake is forgetting `return msg;` at the end. Every function node must return the message object. Without the return, the message stops at that node and nothing downstream receives data.

**"Debug panel shows a string, not an object"**: You need a json parse node between the mqtt in node and the rest of your flow. The MQTT message arrives as a raw string. The json node converts it to a JavaScript object so you can access individual properties like `.temp` and `.hum`.

**"Can I connect Node-RED to InfluxDB?"**: Yes. Install `node-red-contrib-influxdb` from Manage Palette. Add an influxdb-out node after your json parse node. Configure it with your InfluxDB connection from the Docker setup. This gives you persistent time-series storage with Grafana dashboards on top of the Node-RED dashboard. We won't do it today, but it's a natural extension for your project.

**"Can I run this on a Raspberry Pi?"**: Yes. Node-RED runs natively on Raspberry Pi and is pre-installed on Raspberry Pi OS. If your project uses a Pi as a gateway, your Node-RED flows transfer directly.
