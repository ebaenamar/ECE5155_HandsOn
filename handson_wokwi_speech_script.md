# Hands-On 1: ESP32 Firmware on Wokwi Speech Script
**Date**: Monday, March 23, 2026  
**Duration**: 60 minutes  
**Audience**: EECE5155 students (graduate level)  
**Context**: Hardware arrives Mar 29. Browser-only session using Wokwi simulator.

---

## Opening (2 min) — Slide 1

Alright everyone, welcome to the first of three hands-on sessions this week. Quick reminder of where we are: your BOM is submitted, your hardware is ordered, and it arrives on Saturday March 29. So what do we do until then? We write the firmware.

This is actually how professional teams work. At Tesla, at Amazon Robotics, at any serious IoT company, firmware engineers start on a simulator. They write the code, test every logic path, handle every edge case. Then when the board arrives, they flash it and the only bugs they're chasing are hardware-specific: timing, voltage levels, antenna issues.

Today you're going to write code that you actually understand, that handles real-world edge cases, and that you can flash onto your real ESP32 next Saturday.

Let's go. Open your browsers.

---

## What is Wokwi? (3 min) — Slide 2

Go to wokwi.com. That's W-O-K-W-I dot com. Wokwi is a free electronics simulator that runs entirely in the browser. You can use it right now, on any laptop, with zero setup.

Why Wokwi and why this over other options? Tinkercad only supports Arduino, it lacks WiFi and MQTT simulation. Proteus requires a paid license and a heavy install. Wokwi gives you ESP32 with a real network stack: WiFi actually works, MQTT actually connects to real brokers on the internet, HTTP requests go through. That's what makes it useful for us: we can test the full pipeline from sensor to cloud, all in the browser.

It supports the ESP32 variants we care about: ESP32, S2, S3, C3. Also Arduino, STM32, and RPi Pico if you ever need those. Both Arduino framework and ESP-IDF are supported.

Free to use. You only need an account if you want to save your projects, which you will want to do before you leave today.

---

## Interface Tour (3 min) — Slide 3

Let me walk you through the interface. When you open a project you see four areas.

On the left: the **code editor**. This is where your `.ino` sketch lives. You'll also see tabs for `diagram.json` (which defines your circuit) and `libraries.txt` (which lists your dependencies). It has syntax highlighting and basic autocomplete.

On the right: the **simulation view**. This is your virtual breadboard. You'll see your ESP32 and any components you add. The key thing here: you can click on components to interact with them. Click a DHT22 to change its temperature. Click a button to press it. Click a potentiometer to turn it. This is how you test edge cases.

At the bottom: the **Serial Monitor**. This appears once you start the simulation. It shows everything your code prints with `Serial.print()`. It also has an input field if you need to send data to your sketch.

At the top: the **toolbar**. Green Play to start, Red Stop to stop, and Save/Share/Download buttons.

---

## Adding Components and Wiring (3 min) — Slide 4

To add a component, click the **blue "+" button** on the simulation view. Search by name: "DHT22", "LED", "servo", whatever you need. Click the component to add it, then drag it into position.

Wiring is simple: click on a component pin (you'll see a green dot), then click on the destination pin on the ESP32. A wire appears automatically. Click a wire to delete it.

Two files are important to understand. The `diagram.json` stores every component and every wire as JSON. You can edit it directly, copy it from examples, or share your entire project as a single file. This is also what we put in the GitHub repo for you, so you can paste it directly.

The `libraries.txt` file lists the Arduino libraries your sketch needs, one per line. For example, `DHT sensor library` and `PubSubClient`. Wokwi fetches them automatically when you run the simulation.

---

## Documentation and Resources (2 min) — Slide 5

Bookmark these links. The Wokwi docs at docs.wokwi.com are well-written: ESP32 guide, component reference for every supported part, and a WiFi simulation guide that explains how the virtual network works.

I've also linked two example projects on the slide: one for ESP32 + DHT22, one for ESP32 + MQTT. A good workflow: open an example, click "Duplicate", then modify it. Faster than building from scratch.

Now, about AI. You can absolutely ask ChatGPT or Copilot to generate Arduino code for Wokwi. But always: verify pin numbers match your wiring, check library names are correct, test edge cases like sensor failure and WiFi drops, and read the Wokwi docs for simulator-specific details. For example, the WiFi SSID in Wokwi must be `Wokwi-GUEST`. AI will often generate `"your_ssid"` as a placeholder. That kind of detail matters.

---

## Why Simulate First? (2 min) — Slide 6

**And here's the AI angle**: You could ask ChatGPT right now "write me ESP32 code to read a DHT22 and publish to MQTT." And it would give you something. It might even compile. But does it handle WiFi disconnection? Does it handle sensor failure gracefully? Does it respect the 2-second minimum read interval of the DHT22? Does it match YOUR specific BOM and YOUR topic schema?

The professional firmware workflow is: write code on the simulator, test all logic paths, flash to real hardware, then debug only hardware-specific issues. That's what we're doing today. You leave with firmware you can flash on March 29.

---

## Step 0: Open Wokwi (2 min) — Slide 7

OK, let's get hands-on. Click "Start from Scratch" and select ESP32. You should see the code editor on the left, the virtual board on the right, and the green play button at the top.

If you want to save your work later, create a free account. But for now, let's just start coding.

One thing I love about Wokwi: the mental model you're building here transfers directly to the physical build next week. Same pins, same wiring concepts, same code.

Everyone got Wokwi open? Good. Let's add our first sensor.

---

## Exercise 1: Read a Virtual DHT22 (15 min) — Slides 8-9

Click the "+" on the board, search for "DHT22," and add it. Now wire it: VCC goes to 3.3V, GND to GND, and the data pin to GPIO 15. In Wokwi this is drag-and-drop wiring—just click the pin and drag to the destination.

Now look at the code on the slide. Let's walk through it line by line because I want you to understand every single line, not just copy it.

First: `#include <DHT.h>`. This is the library that handles the one-wire protocol for the DHT22. The sensor uses a custom timing-based protocol, different from I2C or SPI. The library abstracts that for you.

`#define DHTPIN 15`: this is the GPIO pin. Why 15? For the simulator it could be anything, but on a real ESP32, you choose based on which GPIOs are safe to use during boot. GPIO 15 is fine. Some GPIOs on the ESP32 have restrictions: GPIO 6 through 11 are connected to the internal flash, for example. **This is the kind of thing AI skips unless you specifically ask.**

Now the important part: the `if (isnan(temp) || isnan(hum))` check. This is error handling. In the real world, sensors fail. The wire comes loose, the sensor gets wet, the power supply is noisy. If you don't check for NaN, your code will publish garbage to MQTT and your entire pipeline—Node-RED, InfluxDB, Grafana—will have garbage data.

Go ahead and run the simulation. Click the green play button.

You should see temperature and humidity readings in the Serial Monitor at the bottom. Now here's the cool part—click on the DHT22 in the simulator. You can change the temperature and humidity values manually. Try setting it to -40°C—that's the sensor minimum. Then try 80°C—that's the maximum. Does your code still work? Good.

Now try something: disconnect the data wire by clicking on it and deleting it. What happens? Your code should print "ERROR: Failed to read DHT22!" instead of crashing. That's the error handling working.

**This is what separates your code from AI-generated code.** AI gives you the happy path. You're testing the failure paths. In a real deployment, sensors fail. Your firmware must handle that gracefully.

Quick engineering question for you: look at the `delay(2000)` at the end. Why 2000 milliseconds? Because the DHT22 datasheet specifies a minimum 2-second interval between readings. If you read faster, you get stale data or errors. **Did the AI tell you that? Probably the datasheet did.** This is exactly the kind of context-driven decision we talked about in the BOM session.

---

## Exercise 2: WiFi + MQTT Publish (25 min) — Slides 10-12

Now we're going to make this useful. Reading a sensor locally is fine for debugging, but your MVP needs to send data to the cloud. That means WiFi and MQTT.

Look at the code on the slide. We're adding three new things: WiFi connection, MQTT client, and JSON publishing.

Let's talk about the WiFi part first. In Wokwi, the virtual WiFi network is called "Wokwi-GUEST" with an empty password. On your real ESP32, you'll change this to your actual network credentials. **Important security note**: keep WiFi credentials out of code that goes to GitHub. Use a separate config file or environment variables. AI-generated code almost always hardcodes credentials.

Now the `connectWiFi()` function. Notice the `while (WiFi.status() != WL_CONNECTED)` loop. This is a blocking reconnection loop. In a prototype, it's fine. In production, you'd use a non-blocking approach with a state machine so the MCU can do other things while waiting for WiFi. But for your MVP, this works.

The MQTT part uses `PubSubClient`, which is the standard Arduino MQTT library. We're connecting to `broker.hivemq.com`, a free public broker. Perfect for testing. For your final project, you might want your own broker (Mosquitto on Docker, like we set up in the cloud hands-on).

Now look at the `connectMQTT()` function. Notice the random client ID: `"esp32-" + String(random(0xffff), HEX)`. Why random? Because if two clients connect to the same MQTT broker with the same ID, the broker kicks one off. In a classroom with 20 people all using "esp32-client" you'd have chaos. **This is a practical constraint that AI might miss, but you'd catch it after debugging one MQTT collision.**

The publishing part creates a JSON string: `{"temp":23.5,"hum":65.2}`. We'll make this more structured in Exercise 3, but for now this is enough to verify the pipeline works.

Everyone run the simulation. You should see "Connecting to WiFi..." then "Connected!" then "Connecting to MQTT..." then "Connected!" and then "Published:" messages every 10 seconds.

Now here's the verification step that matters. Open a new browser tab and go to hivemq.com/demos/websocket-client/. Click "Connect" with the default settings. Then subscribe to the topic `eece5155/YOUR_NAME/sensors`—replace YOUR_NAME with whatever you used in the code. You should see your JSON messages arriving in real time.

**This is end-to-end verification.** Your virtual ESP32 is reading a virtual sensor, publishing over MQTT to a real cloud broker, and you're seeing the data in a web client. When the real hardware arrives, the only thing that changes is the sensor is physical instead of virtual.

Now test resilience. What happens if you stop the simulation and restart it? Does it reconnect automatically? It should, because the `if (!mqtt.connected()) connectMQTT()` check in `loop()` handles that. AI-generated MQTT code often forgets this reconnection logic. Yours has it.

---

## Exercise 3: Structured JSON + Multiple Sensors (20 min) — Slides 13-14

Okay, let's make the data more professional. In Exercise 2 we published `{"temp":23.5,"hum":65.2}`. That works, but it's missing critical information: which device sent this? When? What units?

Look at the code on the slide. We're adding a photoresistor (LDR) on GPIO 34—that's an analog input. And we're building a proper JSON structure with `device_id`, `timestamp`, `sensors` with `value` and `unit` for each.

Why does this matter? Because on Wednesday, when we build Node-RED flows, and in your cloud pipeline with InfluxDB, the system needs to parse this JSON reliably. If every student has a different JSON schema, the pipeline breaks. **Consistent schema = reliable pipeline.** This is a systems engineering decision, not just a coding one.

Add the LDR to your Wokwi board: click "+", search for "Photoresistor" or "LDR." Connect it to GPIO 34 with a voltage divider. In Wokwi you can click on the LDR to change the light level.

Notice the ADC conversion: `light * (3.3 / 4095.0)`. The ESP32 has a 12-bit ADC, so analog readings go from 0 to 4095. We convert to voltage because voltage is a physically meaningful quantity. Raw ADC values are meaningless to anyone reading your data.

Now here's your task for the rest of this exercise: **adapt the code to YOUR project.** Look at the table on slide 14. Wokwi has DHT22, DS18B20, HC-SR04, PIR, MPU6050, potentiometers, and more. Find the sensor closest to what's in your BOM and swap it in.

If your specific sensor isn't available in Wokwi—say you're using a soil moisture sensor—use a potentiometer as an analog proxy. Map the 0-4095 range to your sensor's output range. For soil moisture, that might be 0-100%. The firmware logic is the same; the only difference is the physical transducer, which you'll swap on March 29.

**This is professional firmware development.** You prototype with simulated inputs, validate the logic, then swap in real hardware. The code structure doesn't change.

---

## Wrap-Up (5 min) — Slide 15

Before you leave, three things:

1. **Save your Wokwi project.** Create an account if you haven't. Click Save. You'll need this link for your portfolio and for next Saturday.

2. **Post your Wokwi project link** on the Canvas discussion board. I want to see everyone's projects before Wednesday.

3. **Verify your MQTT is working** on HiveMQ web client. If it's not working, stay after and I'll help debug.

Here's the roadmap for this week: today you built the device side, the firmware that reads sensors and publishes data. Wednesday we build the automation side: Node-RED flows that receive your MQTT data, apply logic, and create dashboards. Thursday we analyze real IoT datasets to understand data quality issues you'll face with your own sensors. And Saturday, March 29, the hardware arrives. You walk in with code that's already tested. You flash it. It works.

**Portfolio note**: include your Wokwi project link in your final report. Write something like "firmware was developed and tested in simulation before hardware deployment, reducing integration time." That sentence alone shows a professional development methodology.

Any questions? 

Good. See you Wednesday. If you get stuck between now and then, post on the Canvas discussion or come to office hours tomorrow.

---

## Quick Reference: Common Issues

If students ask during the session:

**"Wokwi WiFi fails to connect"**: Make sure you're using `"Wokwi-GUEST"` with an empty password. Case sensitive.

**"MQTT keeps disconnecting"**: Check the client ID is unique. Two students with the same ID will kick each other off.

**"DHT22 returns NaN"**: Check wiring: VCC to 3.3V (use 3.3V, avoid 5V on ESP32), data to the correct GPIO, and ground to GND. In Wokwi, re-check the wire connections.

**"My sensor is missing from Wokwi"**: Use a potentiometer as proxy. Map the analog range to your sensor's range. The firmware logic is identical.

**"Should I use ArduinoJson library?"**: For the MVP, string concatenation is fine. If your JSON gets complex (nested objects, arrays), ArduinoJson is cleaner but adds a dependency. Your call; understand the tradeoff.
