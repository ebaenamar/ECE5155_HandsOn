# Hands-On 1: ESP32 on Wokwi Simulator — Oral Script

**Date**: Monday, March 23, 2026
**Duration**: 60 minutes
**Assumes**: No prior Arduino or embedded systems experience.

---

## Opening (3 min) — Slide 1

Good afternoon everyone. Welcome to the first of three hands-on sessions this week.

Let me set the context. You've already submitted your Bill of Materials, your BOM. The hardware you ordered arrives on Saturday, March 29th. Between now and then, we have an opportunity that most university courses don't give you: you can write and fully test the firmware before the physical board is even in your hands.

This is standard practice in the industry. At companies like Tesla, Amazon Robotics, Bosch, or any serious IoT operation, firmware engineers don't wait around for prototype boards to arrive. They work on simulators. They write all the logic, they test every code path, every error condition, and by the time the hardware shows up, the code is ready. The only bugs left to chase are hardware-specific ones: signal timing, voltage tolerances, antenna placement. The logic itself is already validated.

That's exactly what we're doing today. By the end of this session, you will have working firmware that reads a sensor, connects to WiFi, and publishes data over MQTT to a cloud broker. All tested. When the ESP32 board arrives on Saturday, you flash it and move on to integration.

Some of you may have experience with Arduino or embedded programming. Some of you may have none at all. That's completely fine. I'm going to walk you through everything from scratch.

Go ahead and open your browser. We're going to work with a tool called Wokwi.

---

## What is Wokwi? (3 min) — Slide 2

Wokwi. W-O-K-W-I. Go to wokwi.com.

So what is this? Wokwi is a free, browser-based electronics simulator. Think of it as a virtual lab bench. You get a virtual microcontroller, virtual sensors, virtual wires, and you can write code and run it right there in the browser. Nothing to install. Nothing to configure on your machine.

Let me explain why we're using this particular tool and why it matters for what we're doing in this course.

There are other simulators out there. Tinkercad, for instance, which some of you may have used. The problem with Tinkercad is that it only supports basic Arduino boards. It has no ESP32 support, no WiFi simulation, and no MQTT. For an IoT course, that's a dealbreaker, because our entire data pipeline depends on wireless communication and message brokering.

Proteus is another option, widely used in EE departments. Proteus is powerful, but it requires a paid license and a heavy desktop installation. For a hands-on session where we need everyone productive in five minutes, that's impractical.

Wokwi solves both problems. It supports the ESP32, which is the microcontroller you ordered in your BOM. And critically, it has a real network stack. When your virtual ESP32 connects to WiFi in Wokwi, it actually reaches the internet. When it publishes an MQTT message to a cloud broker, that message actually arrives. This means we can test the full pipeline today: sensor reading, wireless transmission, and cloud reception. All from the browser.

It supports all the ESP32 variants: the original ESP32, the S2, S3, and C3. It works with both the Arduino framework, which is what we'll use today, and the ESP-IDF, which is the Espressif native SDK.

It's free for educational use. You only need to create an account if you want to save your project. I strongly recommend you do that before the end of this session, because you'll need your project link for the homework and for the hardware session on Saturday.

---

## Interface Tour (3 min) — Slide 3

Let me walk you through what you see when you open a Wokwi project. There are four main areas on the screen.

On the left side, you have the **code editor**. This is where you write your program. In Arduino terminology, the file is called a "sketch" and it has a `.ino` extension. If you've never worked with Arduino before, here's what you need to know: a sketch has two main functions. `setup()` runs once when the microcontroller starts. `loop()` runs repeatedly, forever, after setup finishes. That's the entire execution model. Setup once, loop forever. Every Arduino program follows this pattern.

You'll also see tabs at the top of the editor. One is called `diagram.json`, which is a file that describes your circuit: which components are on the board and how they're wired. Another is `libraries.txt`, which lists the external libraries your code depends on. We'll use both of these.

On the right side, you have the **simulation view**. This is your virtual breadboard. You'll see a picture of the ESP32 development board, and as you add components like sensors or LEDs, they appear here. The important thing to know: these components are interactive. Once the simulation is running, you can click on a temperature sensor and drag a slider to change the temperature. You can click on a light sensor and adjust the light level. This lets you test how your code responds to different inputs without needing physical hardware.

At the bottom, the **Serial Monitor**. If you've used a terminal or command line before, this is similar. When your code calls `Serial.print()` or `Serial.println()`, the output appears here. This is your primary debugging tool. Every time something happens in your code, you should print a message so you can see what's going on. When we're working with real hardware, the Serial Monitor connects through a USB cable. In Wokwi, it's built right into the interface.

At the top, the **toolbar**. The green Play button starts the simulation, compiles your code and runs it on the virtual ESP32. The red Stop button halts it. You'll also find Save, Share, and Download options there.

---

## Adding Components and Wiring (3 min) — Slide 4

Now, how do you actually build a circuit in Wokwi?

To add a component, look for the **blue "+" button** in the simulation view. Click it and you get a search dialog. Type the name of what you need: "DHT22" for a temperature and humidity sensor, "LED" for a light-emitting diode, "servo" for a motor, and so on. Click the component and it appears on your virtual board. You can drag it to reposition it.

Wiring works like this: click on one of the component's pins. You'll see a small green dot appear, indicating the pin is selected. Then click on the destination pin, for example a GPIO pin on the ESP32. A colored wire is drawn automatically between the two points. If you need to delete a wire, just click on it.

Now, a quick note for those of you who haven't worked with microcontrollers before. The ESP32 has a set of pins along its edges called GPIO pins. GPIO stands for General Purpose Input/Output. Each pin has a number. Some pins are for digital signals, on or off. Some can read analog voltages, values between 0 and 3.3 volts. Some are reserved for special functions. We'll talk about which pins to use as we go through each exercise.

Behind the scenes, Wokwi stores your entire circuit in a file called `diagram.json`. Every component and every wire is represented in that file as JSON. You can edit it by hand if you want, but more importantly, you can copy-paste it. In the GitHub repository for this session, I've provided the `diagram.json` for each exercise. You can paste it directly into Wokwi and the circuit builds itself. That saves you time on wiring so you can focus on the code.

The other important file is `libraries.txt`. Arduino has a large ecosystem of third-party libraries. When your code needs one, for example the DHT sensor library to communicate with a temperature sensor, you list it in `libraries.txt`, one library per line. Wokwi downloads and links it automatically when you run the simulation.

---

## Documentation and Resources (2 min) — Slide 5

Before we start coding, let me point you to the resources you'll want to bookmark.

The official Wokwi documentation is at docs.wokwi.com. It's well organized and covers every supported component. If you're unsure how to wire something, or what pins a particular sensor uses, that's where you look. There's a specific ESP32 guide and a WiFi simulation guide that explains how the virtual network works.

On the slide I've linked two example projects that are already built and working. One reads a DHT22 sensor. The other connects to MQTT. A productive workflow: open one of these examples, click "Duplicate" to make your own copy, and modify it. This is often faster than building from scratch, and it gives you a working reference to compare against.

Now, regarding AI tools. You are welcome to use ChatGPT, Copilot, or any other AI assistant during this course. But I want you to be deliberate about it. If you ask an AI to generate Arduino code for Wokwi, it will give you something plausible. But you need to verify specific details. Are the pin numbers correct for your wiring? Are the library names spelled exactly right? Does it handle what happens when the sensor fails? Does it know that in Wokwi, the WiFi network name must be exactly `Wokwi-GUEST` with that exact capitalization and no password? AI tools tend to generate generic placeholders for those details. Your job as the engineer is to fill in the specifics and verify the edge cases.

---

## Why Simulate First? (2 min) — Slide 6

Let me make the case for simulation before hardware, because this is a principle that will serve you well beyond this course.

You could ask an AI right now: "Generate ESP32 code to read a DHT22 and publish data over MQTT." And you'd get something. It might even compile on the first try. But think about what it probably doesn't handle. What happens when the WiFi connection drops? What happens when the sensor returns an invalid reading? Does it respect the two-second minimum interval between DHT22 reads that the datasheet specifies? Does the MQTT topic structure match what your pipeline expects?

These are the questions that separate a working prototype from a reliable system. And a simulator is where you answer them, safely, quickly, with no risk of damaging hardware or wasting time on USB driver issues.

The professional workflow is: develop on the simulator, test all logic paths including failures, then move to real hardware and only debug the things that are truly hardware-dependent. That's what we're doing today. When you leave this session, you'll have firmware that's ready to flash onto a real ESP32 on March 29th.

---

## Step 0: Open Wokwi (2 min) — Slide 7

Alright, let's begin. In your browser, on wokwi.com, click "Start from Scratch" and select ESP32.

You should now see the layout I described: code editor on the left, the virtual ESP32 board on the right, and the green Play button at the top. If you see something different, raise your hand.

If you'd like to save your work at any point, click the Sign Up button and create a free account. I recommend doing this now so you don't lose your progress later. But if you'd rather wait, that's fine too.

One thing I want you to appreciate: the code you write here, the wiring you set up, the pin assignments you choose, all of it transfers directly to the physical build next week. Wokwi's ESP32 simulation mirrors the real chip's GPIO layout. So the mental model you're building right now is the same one you'll use with real hardware.

Is everyone in? Does everyone see the ESP32 board and the code editor? Good. Let's add our first sensor.

---

## Exercise 1: Read a Virtual DHT22 (15 min) — Slides 8-9

For those unfamiliar, the DHT22 is a digital sensor that measures temperature and humidity. It's inexpensive, widely available, and very common in IoT projects. Several of you have it in your BOM. Even if your project uses a different sensor, the principles we cover here, reading data, checking for errors, printing to the serial monitor, apply universally.

Let's add it to our board. Click the blue "+" button in the simulation view. Type "DHT22" in the search box. Click it to add it to your board. Now we need to wire it.

The DHT22 has three relevant pins: VCC, which is the power supply; GND, which is ground; and SDA or Data, which is the signal pin. We connect VCC to the ESP32's 3.3V output. We connect GND to one of the ESP32's ground pins. And we connect the data pin to GPIO 15. Why 15? In the simulator, any available GPIO would work. But on a real ESP32, some GPIOs have restrictions. For example, GPIOs 6 through 11 are connected to the internal flash memory and should never be used for external peripherals. GPIO 15 is a safe choice. This kind of detail is in the ESP32 datasheet and the Wokwi docs, and it's the kind of thing AI-generated code often gets wrong.

Now look at the code on the slide. Let me walk through each part.

At the top: `#include <DHT.h>`. This imports the DHT sensor library, which handles the communication protocol between the ESP32 and the sensor. The DHT22 uses a proprietary single-wire timing protocol. It's different from I2C or SPI, which are the standard communication buses you'll encounter in embedded systems. The library takes care of the low-level timing so we don't have to.

Next: `#define DHTPIN 15` and `#define DHTTYPE DHT22`. These are constants that tell the library which GPIO pin the sensor is connected to and which model of sensor we're using. Then `DHT dht(DHTPIN, DHTTYPE)` creates the sensor object.

In `setup()`, we call `Serial.begin(115200)`. This initializes serial communication at 115200 baud, which is the speed at which data is sent to the Serial Monitor. Then `dht.begin()` initializes the sensor.

In `loop()`, we read temperature and humidity with `dht.readTemperature()` and `dht.readHumidity()`. Now here is the critical part: the `if (isnan(temp) || isnan(hum))` check. `isnan` means "is not a number." When the sensor fails to respond, perhaps because a wire is loose or the sensor is malfunctioning, the library returns NaN instead of a numeric value. If you don't check for this, your code proceeds with invalid data. It publishes garbage to MQTT. Your database stores garbage. Your dashboard shows garbage. Every downstream system is affected. This error check is two lines of code and it prevents an entire class of data quality problems.

Finally, `delay(2000)`. The DHT22 datasheet specifies that readings should be taken at least 2 seconds apart. If you poll faster, you get stale cached values or read errors.

Go ahead and run the simulation now. Press the green Play button.

You should see temperature and humidity values printing in the Serial Monitor every 2 seconds. Now try this: click on the DHT22 component in the simulation view. A slider appears. Drag the temperature to -40°C, which is the sensor's rated minimum. Then drag it to +80°C, the maximum. Does your code still print valid readings? It should.

Now try the failure case. Click on the green wire connecting the data pin and delete it. What do you see? The Serial Monitor should now print "ERROR: Failed to read DHT22!" every 2 seconds. The code detected the failure and reported it instead of crashing or publishing bad data. Reconnect the wire to restore normal operation.

This exercise illustrates a fundamental principle: always test the failure path. AI code generators give you the happy path, the case where everything works. But in deployed systems, sensors fail regularly. Your firmware must handle that gracefully.

---

## Exercise 2: WiFi + MQTT (25 min) — Slides 10-12

Reading a sensor locally is a good start, but the purpose of an IoT device is to transmit data. Your sensor node needs to send readings to a remote system where they can be stored, analyzed, and visualized.

The standard protocol for this in IoT is MQTT, which stands for Message Queuing Telemetry Transport. MQTT is a lightweight publish-subscribe messaging protocol designed for constrained devices and unreliable networks. It works like this: devices publish messages to "topics," which are like named channels. Other devices or applications subscribe to those topics and receive the messages. A central server called a "broker" routes messages from publishers to subscribers. We covered this in the lectures; now we implement it.

For this exercise, we need WiFi to reach the internet and an MQTT client library to speak the protocol. Look at the code on the slide.

First, the WiFi part. In Wokwi, there's a built-in virtual WiFi network. The SSID is `Wokwi-GUEST` and the password is empty. This is specific to the simulator. On your real ESP32, you'll replace these with your actual network credentials. One important security note: when you move to real hardware, do not put WiFi passwords directly in code that gets pushed to GitHub. Use a separate configuration file, or environment variables, or a secrets header that's excluded from version control.

The `connectWiFi()` function calls `WiFi.begin()` and then loops, waiting until the connection is established. This is a blocking approach, meaning the microcontroller does nothing else while waiting for WiFi. For a prototype, that's acceptable. In a production system you'd implement this as a non-blocking state machine so the device can do other tasks while reconnecting. But for your MVP, this pattern is appropriate.

Now the MQTT part. We use a library called `PubSubClient`, which is the standard Arduino MQTT client. We connect to `broker.hivemq.com` on port 1883. HiveMQ runs a free public MQTT broker that anyone can use for testing. This means your simulated ESP32 is publishing to a real broker on the internet, and you can subscribe to those messages from any MQTT client anywhere in the world.

Notice the random client ID: `"esp32-" + String(random(0xffff), HEX)`. Every MQTT client connected to a broker must have a unique identifier. If two clients connect with the same ID, the broker disconnects the first one. In a classroom where everyone might use the same example code, this would cause constant disconnections. The random suffix avoids that collision.

The `loop()` function checks whether the MQTT connection is still active on every iteration, and reconnects if needed. This reconnection logic is essential for any real deployment, because network connections drop. It's also something that AI-generated code frequently omits.

The publishing itself is straightforward: we build a JSON string with the sensor values and call `mqtt.publish()` with a topic and the payload.

Run the simulation. You should see the WiFi connection, then the MQTT connection, then a "Published:" message every 10 seconds.

Now the verification step that brings this together. Open a new browser tab and go to hivemq.com/demos/websocket-client/. This is HiveMQ's web-based MQTT client. Click "Connect" with the default settings. Then in the Subscribe section, enter the topic `eece5155/YOUR_NAME/sensors`, replacing YOUR_NAME with whatever you used in the code. Click Subscribe. You should see your JSON messages appearing in the browser in real time.

Take a moment to appreciate what just happened. Your virtual microcontroller, running in one browser tab, read a virtual sensor, connected to a real WiFi network, published a message to a real MQTT broker in the cloud, and you received that message in another browser tab. The entire IoT data path, from sensor to cloud, is working. When the physical hardware arrives, the only change is that the sensor becomes real instead of virtual. The code, the protocol, the broker, the topic structure, all stay the same.

One more test. Stop the simulation and start it again. Watch the Serial Monitor. The code should automatically reconnect to WiFi and MQTT without any manual intervention. That's the reconnection logic doing its job.

---

## Exercise 3: Structured JSON (20 min) — Slides 13-14

In Exercise 2, we published `{"temp":23.5,"hum":65.2}`. That works, but it's minimal. In a real deployment with multiple sensor nodes, you need to know which device sent each message and when. You also want consistent units and a predictable structure so that your backend systems can parse the data reliably.

We're going to add a second sensor, a photoresistor or LDR (light-dependent resistor), and restructure the JSON payload to include device identification, timestamps, and units for every measurement.

The LDR is an analog sensor. Unlike the DHT22 which gives you a digital value, the LDR produces a voltage that varies with light intensity. The ESP32 reads this voltage through its ADC, its analog-to-digital converter, which is a 12-bit converter. That means it maps the input voltage range, 0 to 3.3 volts, to a numeric range of 0 to 4095. The code converts this raw ADC value back to voltage with the formula `light * (3.3 / 4095.0)`, because voltage is a physically meaningful quantity that can be calibrated and compared across devices.

Add the LDR to your Wokwi board: click "+", search for "Photoresistor" or "LDR". Connect it to GPIO 34, which is one of the ESP32's analog-capable input pins. During the simulation, you can click on the LDR and move a slider to change the light level.

Look at the JSON structure in the code. We now have `device_id`, so you know which node sent this. We have `timestamp` using `millis()`, which gives milliseconds since the device started. And under `sensors`, each measurement has a `value` and a `unit`. This structure matters because on Wednesday, when we build Node-RED flows to process this data, and later when you store it in InfluxDB, the system needs to parse this JSON programmatically. If every student uses a different JSON schema, the processing pipeline breaks. A consistent, documented schema is what makes an IoT system reliable at scale.

Now here is your real task for the remainder of this exercise, and it extends into your homework. **Adapt this code to your own project.** Look at the sensor mapping table on slide 14. It shows which Wokwi component corresponds most closely to the sensor in your BOM. If your exact sensor doesn't exist in Wokwi, use a potentiometer as an analog proxy. A potentiometer produces a variable voltage between 0 and 3.3V, which the ADC reads as 0 to 4095. You map that range to whatever your sensor's output range is. For example, if you're building a soil moisture monitor, you might map 0-4095 to 0-100% moisture.

This is how professional embedded development works. You prototype with simulated inputs, validate all the logic and data handling, and then swap in the real transducer when hardware is available. The code structure doesn't change.

---

## Wrap-Up (5 min) — Slide 15

Before you leave, three action items.

First, save your Wokwi project. Create an account if you haven't already, and click Save. You're going to need that project link for the homework assignment and for the hardware session on Saturday.

Second, post your Wokwi project link on the Canvas discussion board. I want to see everyone's projects before Wednesday's session.

Third, verify that your MQTT messages are actually arriving at the HiveMQ web client. If they're not showing up, stay after class and we'll debug it together.

Here's the big picture for this week. Today you built the device side of the IoT pipeline: firmware that reads sensors and publishes structured data over MQTT. On Wednesday we build the automation and visualization side: Node-RED flows that subscribe to your MQTT topics, apply processing logic, and create dashboards. On Thursday we work with a real-world IoT dataset to understand the data quality issues you'll encounter with your own sensors. And on Saturday the 29th, the hardware arrives. You walk in with code that's already tested and validated. You flash it. You integrate it. You move forward.

For your portfolio and your final report: include the Wokwi project link as evidence of your development process. A sentence like "firmware was developed and tested in simulation prior to hardware deployment, reducing integration time and risk" demonstrates a mature engineering methodology. That matters.

Questions? Good. See you Wednesday. If you run into issues between now and then, post on Canvas or come to office hours tomorrow.

---

## Quick Reference: Common Issues

**"WiFi won't connect"**: The SSID must be exactly `"Wokwi-GUEST"` with an empty password string `""`. It's case sensitive. Any typo and it won't connect.

**"MQTT keeps disconnecting"**: Check that the client ID is unique. If two students use the same ID, the broker disconnects one of them on every connection attempt.

**"DHT22 returns NaN"**: Check your wiring. VCC should go to the 3.3V pin (not 5V, the ESP32 is a 3.3V device). The data pin should go to the GPIO you specified in your code. Ground to GND. In Wokwi, click each wire to verify it connects to the correct pins.

**"My sensor doesn't exist in Wokwi"**: Use a potentiometer as an analog proxy. Connect it to an analog-capable GPIO (34, 35, 36, or 39). Map the 0-4095 ADC range to your sensor's output range in the code. The data handling and MQTT publishing logic is identical regardless of the physical sensor.

**"Should I use the ArduinoJson library?"**: For the MVP, building JSON with string concatenation is perfectly adequate. If your payload becomes complex with deeply nested objects or arrays, ArduinoJson provides a structured API that reduces the chance of syntax errors. It adds a library dependency. Either approach is valid; understand the tradeoff and choose deliberately.
