# BOM Tutorial Session - Speech Script
**Date**: March 19, 2026  
**Duration**: 60 minutes  
**Audience**: EECE5155 students (graduate level)  
**Context**: BOM submission due today, hardware delayed until Mar 29

---

## Opening (3 min) - Slide 1

Alright everyone, let's get started. Today we're talking about your MVP Hardware Specification, which is due by end of day. I know some of you are thinking "it's just a shopping list," but I want to change that mindset right now.

This BOM is going into your portfolio. When you interview at companies like Tesla, Amazon Robotics, or any IoT startup, they're going to ask: "Show me a project where you designed something from scratch." This is that project.

**And here's the critical thing in 2026**: We're all using AI tools—ChatGPT, Claude, Copilot. You've probably already asked an AI "what sensor should I use for soil moisture?" And it gave you an answer, right? Maybe it said "use a capacitive soil moisture sensor."

But here's what the AI **doesn't know**:
- It doesn't know your deployment environment (is it a vineyard in New England or a greenhouse in Arizona?)
- It doesn't know your power constraints (solar? battery? mains power?)
- It doesn't know your budget (are you building 1 unit or 1000?)
- It doesn't know what questions to ask to understand your specific context

**Your value as an engineer in the AI era is understanding context deeply enough to design the most appropriate solution.** The AI can suggest components. But only you can evaluate if they fit your specific constraints, ask the right follow-up questions, and estimate the practical implications.

So let's talk about what this deliverable is really about.

---

## Slide 2: This Is Not Just a Shopping List (5 min)

Here's the question I want you to keep in mind throughout this session:

**"If I gave you $100,000 to deploy 1000 units of your system, would it work?"**

That's what employers and investors are asking when they look at your portfolio. Not "can you build one prototype in a lab?" but "can you design something that scales to production?"

Your BOM plus your PES is your answer to that question. Let me break down what they're looking for—and notice how each of these requires **human judgment** that AI can't provide:

1. **Can you translate requirements into hardware?** AI can suggest "use a temperature sensor." But only you can ask: "Wait, what's the deployment environment? Is it outdoor (-40°C to +85°C) or indoor (15°C to 30°C)? That changes which sensor I need." Only you know to ask: "What's the sampling rate? If I need 1 kHz, I can't use a DHT22 (max 0.5 Hz)." This context-driven component selection is human expertise.

2. **Do you understand tradeoffs?** AI might list options: "DHT22, DS18B20, or BME280 for temperature." But it doesn't know which matters more for YOUR project: cost, accuracy, interface complexity, or power consumption. You need to evaluate: "For my vineyard deployment, I need low power and outdoor rating. DHT22 works but BME280 adds humidity and pressure for only $3 more—is that worth it for my use case?" That's engineering judgment.

3. **Can you justify decisions with data?** AI can hallucinate specs. It might say "DHT22 is accurate to ±0.5°C" but not tell you that's only at 25°C, and degrades to ±2°C at temperature extremes. You need to **verify every claim** by reading the actual datasheet. This is where you prove you didn't just trust the AI—you validated it.

4. **Is your solution viable and scalable?** AI has no idea about lead times, supply chain constraints, or bulk pricing in 2026. It doesn't know that chip shortages are still affecting certain MCUs, or that shipping from China now takes 6 weeks instead of 2. Only you can call Digi-Key, check stock levels, get a quote for 1000 units, and estimate realistic costs. These practical constraints are invisible to AI.

This is what separates a class project from a portfolio piece that gets you hired. **In the AI era, your value is asking the right questions and understanding the full context.**

---

## Slide 3: Viability - Does It Actually Work? (7 min)

Let's talk about viability first. Your MVP must demonstrate technical feasibility. Here's what that means in practice:

**Hardware viability checklist:**
- Sensor meets accuracy requirements → cite the datasheet
- MCU has enough memory and processing power → calculate it
- Power budget is realistic → measure it
- Communication range is sufficient → do a link budget calculation

Let me show you a bad example versus a good example.

**BAD example:**
"I'm using a DHT22 temperature sensor and an ESP32."

Why is this bad? No justification. Why DHT22 and not DS18B20? Why ESP32 and not Arduino Uno or STM32 or Raspberry Pi? I have no idea if you just picked what you had lying around or if you actually thought about it.

**GOOD example:**
Look at this table on the right. For each PES requirement, there's a specific solution with a datasheet citation.

- R1 needs ±0.5°C accuracy → DHT22 datasheet page 3 says ±0.5°C. Perfect match.
- R2 needs WiFi MQTT → ESP32 has built-in WiFi and 520kB RAM. No external module needed.
- R3 needs 1-year battery life → ESP32 datasheet Table 7 shows 10µA sleep mode. I can calculate that works.
- R4 has a $50 budget → My total BOM is $42. I'm under budget.

See the difference? Every component choice answers a specific requirement with quantitative evidence. That's engineering rigor.

Now, I know some of you are thinking "but I haven't tested my sensor yet, how do I know it works?" That's fine—this is a spec, not a validation report. But you need to show that *on paper*, based on datasheets, your design should work. We'll validate experimentally when the hardware arrives on March 29.

**Important note on using AI for this**: If you ask ChatGPT "will a DHT22 work for my application?", it might say yes. But it doesn't know to ask:
- "What's your actual temperature range?" (DHT22 works -40°C to +80°C, but accuracy degrades outside 0-50°C)
- "How fast do you need readings?" (DHT22 has 2-second minimum interval)
- "What's your power budget?" (DHT22 draws 2.5mA during measurement)
- "Do you need humidity too, or just temperature?" (if just temp, DS18B20 is cheaper and more accurate)

**Your job is to ask these questions, verify the answers in datasheets, and document your reasoning.** The AI is a tool for brainstorming options. You're the engineer who validates and decides.

---

## Slide 4: Scalability - Can You Deploy 1000 Units? (8 min)

Okay, viability is about "does it work?" Scalability is about "can you deploy this at scale?"

Here are the questions investors and employers ask:

**1. Cost at scale**

Your prototype might cost $42 per unit because you're buying one sensor from Amazon. But what happens when you buy 1000 sensors from Digi-Key or Mouser? Usually the price drops 20-40%.

**Here's where AI fails you**: If you ask "what will this cost at 1000 units?", AI will guess. It has no access to current Digi-Key pricing, no knowledge of 2026 component shortages, no idea that certain chips are backordered.

**You need to do the real work**: Go to Digi-Key, look up the actual part number, check the quantity pricing table. Or email sales@digikey.com and ask for a quote. Get real numbers, not AI estimates.

Document this in your BOM. Write a note: "Prototype: $42/unit. At 1000 units: estimated $28/unit based on Digi-Key quantity pricing (see attached quote)."

Why does this matter? Because it shows you understand business **and** that you can get real-world data, not just AI guesses. A product that costs $50 to build and sells for $60 is not viable. But if it costs $28 to build at scale and sells for $60, now you have a 53% margin. That's a real business.

**2. Supply chain**

Are your components actually available? Check lead times on Digi-Key and Mouser. If a sensor has a 12-week lead time, that's a problem. You can't deploy 1000 units if you have to wait 3 months for parts.

Also, avoid single-source components. If there's only one supplier for a critical part and they go out of stock, your entire project is blocked. Always have a backup option. "Primary: Sensor X from Digi-Key. Backup: Sensor Y from Mouser with similar specs."

**3. Manufacturing**

Right now you're building on a breadboard. That's fine for a prototype. But breadboards don't scale. If you want to deploy 1000 units, you need a PCB.

You don't have to design a PCB for this class, but mention it in your final report. "Current prototype uses breadboard for rapid iteration. Production version would use a custom PCB, estimated cost $8/board at 1000 quantity from JLCPCB."

This shows you thought beyond the lab bench.

**Scalability checklist** (point to the box on the right):

Go through this checklist for your project:
- Unit cost < $50 at prototype quantity? Check.
- Projected cost at 1000 units documented? Check.
- All components available from ≥2 suppliers? Check.
- Lead times < 4 weeks? Check.
- Power budget allows >6 months battery? Check.
- Cloud costs estimated? Check.
- Data storage costs estimated? Check.

If you can check all these boxes, you have a scalable design.

---

## Slide 5: BOM Template Walkthrough (5 min)

Alright, let's look at the actual template you need to fill out. Open the file `mvp_project_spec.tex` on your laptop.

There are three critical sections:

**1. Requirements from PES**

Copy the 3 most hardware-relevant requirements from your PES. These drive your component choices.

Example: "R1: Measure soil moisture every 10 minutes with ±2% accuracy."

This tells me: you need a soil moisture sensor with ±2% accuracy, and a system that can wake up every 10 minutes (so low power is important).

**2. Bill of Materials**

This is the table with Component, Part Number, Supplier, Unit Price, Quantity, Have/Need.

The most important column is the last one: **"Why this part? (link to R1-R3)"**

This is where you justify every choice. Don't leave this blank. Don't write "good sensor." Write: "R1 requires ±2% accuracy. Capacitive sensor datasheet specifies ±3% which meets requirement. Chose capacitive over resistive to avoid corrosion in soil."

**3. Datasheet citations**

At the bottom, list the URL or document name for every datasheet you referenced.

Example: "DHT22 datasheet: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf, section 3.2"

This is proof that you actually looked at the specs, not just guessed.

**Grading emphasis**: Justification weighs more than just listing components. A 3-component BOM with excellent justification will score higher than a 10-component BOM with no reasoning.

---

## Slide 6: Example - Smart Agriculture BOM (6 min)

Let me show you a complete example. This is a vineyard soil moisture monitoring system.

**Scenario**: 50 sensor nodes across a 10-acre vineyard, solar-powered, LoRaWAN communication.

**PES Requirements**:
- R1: Measure soil moisture every 10 min, ±2% accuracy
- R2: 2 km range to gateway, outdoor deployment
- R3: Solar-powered, operate 365 days/year in New England

Now look at the BOM table. Each component directly answers a requirement:

**Soil moisture sensor**: Capacitive v1.2, $6.99. Why? R1 requires moisture sensing. Capacitive avoids corrosion. Datasheet says ±3% accuracy, which meets R1. I'm buying 2 for redundancy.

**LoRa module**: RFM95W 868MHz, $19.95. Why? R2 requires 2 km range. I did a link budget calculation (which I'll include in my final report) and this module gives me 2.5 km range with 10dB margin. 868MHz is the ISM band for Europe/US.

**MCU**: STM32L072, $4.50. Why? R3 requires low power. This MCU has 0.8µA sleep mode according to datasheet Table 24. I calculated the power budget: with 10-minute wake intervals, I can run for 180 days on a 2000mAh battery.

**Solar panel**: 6V 1W, $8.99. Why? R3 requires year-round operation. In New England, winter has about 3.5 hours of sun per day. 1W panel × 3.5h = 3.5Wh/day. My system consumes 0.8Wh/day. Margin: 4.4×. That's enough even on cloudy days.

Total prototype cost: $40.43. Under budget.

**Scalability note**: At 1000 units, the RFM95W drops to $12 (Digi-Key bulk pricing), bringing total to $32/unit. That's a 26% cost reduction.

This is what a complete, justified BOM looks like.

---

## Slide 7: Common Mistakes to Avoid (6 min)

Let me show you the mistakes I see every year, so you can avoid them.

**Mistake 1: No justification**

Bad: "DHT22 temperature sensor"

Why is this bad? Doesn't explain why DHT22 versus DS18B20 versus BME280. All three measure temperature. Why did you pick DHT22?

Fix: "DHT22: ±0.5°C accuracy (datasheet page 3) meets R1 requirement. DS18B20 is also ±0.5°C but requires OneWire protocol which is more complex to implement. BME280 is ±1°C which is insufficient for R1."

See? Now I know you evaluated alternatives and made an informed choice.

**Mistake 2: No datasheet citation**

Bad: "ESP32 has enough memory"

Why is this bad? No proof. How do I know you didn't just assume?

Fix: "ESP32 datasheet Table 2: 520kB SRAM. My code uses 180kB (measured in Arduino IDE). Margin: 65%."

Now I have quantitative evidence.

**Mistake 3: Ignoring scalability**

Bad: BOM total is $48, barely under the $50 budget.

Why is this bad? No margin for production. What if one component goes up in price? What if you need to add a feature?

Fix: "Prototype: $48/unit. At 1000 units: $32/unit based on Digi-Key bulk pricing. This gives us margin for manufacturing costs and profit."

**Mistake 4: Single-source components**

Bad: "Custom sensor from Alibaba"

Why is this bad? Supply chain risk. What if that Alibaba seller disappears? What if shipping from China takes 8 weeks?

Fix: "Primary: Sensor X from Digi-Key (2-week lead time). Backup: Sensor Y from Mouser (same specs, 3-week lead time)."

**Bottom line** (point to the warning box): This BOM goes in your portfolio. An employer reading it should think: "This person can design a real product, not just a class project."

---

## Slide 8: Updated Timeline (4 min)

Okay, let's talk about what happens after you submit today.

**Today (Wednesday March 19)**: Submit your BOM by end of day on Canvas. I need this today so I can order parts over the weekend.

**Thursday through Sunday**: I'll review all your BOMs. If I see issues, I'll email you. Then I'll place orders on Amazon Prime, Digi-Key, Mouser. Most parts should arrive by early next week.

**Week of March 23, 25, 26**: We have **3 hands-on sessions, all online via Zoom**. No physical hardware yet—we're preparing for when it arrives.

- Session 1: Wokwi ESP32 simulator. You'll program a virtual ESP32 with virtual sensors. This way, when the real hardware arrives, you already have working code.
- Session 2: Node-RED + MQTT simulator. You'll build the data pipeline (MQTT → InfluxDB → Grafana) using simulated sensor data.
- Session 3: IoT dataset analysis. You'll analyze real IoT datasets to understand data quality issues before you have your own data.

**From March 29 onwards**: Hardware is available, lab is reserved. Now you start the real work:
- Assembly: wire up your sensor, MCU, communication module
- Sensor validation: test accuracy against datasheet specs
- MQTT integration: get data flowing to the cloud

**Week 14 (Wednesday April 9)**: **MVP Working Demo**. This is a checkpoint. You show me: Sensor → MQTT → Database → Dashboard, all working live.

**Week 15 (Tuesday April 15)**: Final report, code, and video due.

**Week 16 (Tuesday/Wednesday April 22-23)**: Final presentations. You pitch your solution like you're presenting to an investor.

So you have about 3 weeks from when hardware arrives to MVP demo. That's tight but doable if you use the next week (online sessions) to prepare.

---

## Slide 9: Your Portfolio Narrative (5 min)

Let me paint a picture of what you'll have at the end of this course.

**Technical artifacts** you'll create:
1. PES (Problem Engineering Spec) - defines the problem and requirements
2. BOM with justifications - shows you can select components
3. Hardware validation report - proves your sensor works as claimed
4. Working code (ESP32 or Arduino) - shows you can program embedded systems
5. Data pipeline (MQTT → Database → Dashboard) - shows you understand IoT architecture
6. Final report in LaTeX - professional documentation
7. Demo video (2-3 minutes) - shows your system working

**The story you'll tell** in job interviews:

"I designed an IoT system for [your problem]. I selected components based on [your requirements], validated them experimentally against datasheets, and demonstrated a working prototype that could scale to 1000 units at $X per unit. The system handles [edge cases like WiFi failure, sensor failure] and achieves [performance metrics like 99% uptime, <1 second latency]."

This is a **complete product development cycle** in one semester. From problem definition to working prototype to scalability analysis.

Most students graduate with projects that are just "I built something that works on my desk." You're going to have a project that shows: "I designed something that could be deployed in production."

That's the difference between getting an interview and getting hired.

---

## Slide 10: Questions & Office Hours (12 min)

Alright, questions?

[Take 2-3 questions from the class]

**Common questions I expect:**

**Q: "What if my prototype BOM is $52, over the $50 budget?"**

A: Document it. Show that at 100 units it drops to $42. Explain which component is driving the cost and what alternatives exist. Or talk to me before submitting—maybe we can find a cheaper alternative that still meets your requirements.

**Q: "What if I can't find a datasheet for a component?"**

A: Then don't use that component. If there's no datasheet, you can't verify specs, which means you can't prove viability. Find an alternative with proper documentation. This is a real-world lesson: never use components without datasheets in production.

**Q: "What if a component has a 6-week lead time?"**

A: Note it in your BOM. Identify a backup component with shorter lead time. Explain the tradeoff (e.g., "Primary choice has better accuracy but 6-week lead time. Backup has slightly lower accuracy but 1-week lead time"). Then we'll decide together which to order.

**Q: "How do I estimate bulk pricing if I've never ordered 1000 units?"**

A: Go to Digi-Key or Mouser, look up your part number, and check the quantity pricing table. It usually shows prices at 1, 10, 100, 1000 units. Or email sales@digikey.com with "quote request for 1000 units of [part number]." They'll respond in 1-2 days. **Don't trust AI estimates for this—get real quotes.**

**Q: "Can I use AI to help with my BOM?"**

A: Absolutely, but use it correctly. AI is great for:
- Brainstorming component options ("what sensors measure soil moisture?")
- Understanding concepts ("explain I2C vs SPI")
- Generating calculation scaffolding ("write Python code to calculate power budget")

But AI is **terrible** at:
- Knowing current prices, lead times, stock levels (it's trained on old data)
- Understanding your specific deployment constraints (it doesn't know to ask the right questions)
- Verifying specs (it hallucinates datasheet values)

**Your workflow should be**: Ask AI for options → Verify every claim in datasheets → Check real pricing/availability → Make informed decision → Document your reasoning. The AI is a research assistant, not the engineer. You're the engineer.

---

**Okay, for the next 20 minutes, work on your BOM. I'll circulate and help individually.**

**Key questions to ask yourself as you work:**
1. Does each component answer a specific PES requirement?
2. Can I cite a datasheet for every spec I claim?
3. Is my solution viable (does it actually work on paper)?
4. Is it scalable (can I deploy 1000 units)?
5. Would an employer reading this BOM want to hire me?

**Deadline reminder: Today, end of day on Canvas. No exceptions—I need to order parts this weekend.**

[Circulate and provide 1-on-1 help]

---

## Individual Office Hours - Sample Interactions

**Student 1: "I'm doing environmental monitoring with CO2 sensor"**

Great. What's your PES requirement for CO2 accuracy?

Student: "±50 ppm"

Okay, which sensor are you using?

Student: "MH-Z19B"

Good choice. What does the datasheet say about accuracy?

Student: "Uh... I'm not sure"

[Pull up datasheet together] Look, page 2, it says ±50 ppm. Perfect match. Write that in your justification column. Now, why MH-Z19B and not SCD30 or CCS811?

Student: "I don't know the others"

Okay, let's look. SCD30 is ±30 ppm, more accurate, but costs $58—over your budget. CCS811 is ±10%, which is ±400 ppm at 4000 ppm—way too inaccurate. So MH-Z19B is the sweet spot: meets accuracy requirement and fits budget. Write that down.

---

**Student 2: "My BOM is $55, over budget"**

Let's see. [Look at BOM] You have ESP32 for $9, DHT22 for $5, LoRa module for $20, solar panel for $12, battery for $9. What's driving the cost?

Student: "The LoRa module"

What's your range requirement?

Student: "500 meters"

Okay, you don't need LoRa for 500 meters. WiFi can do that. Drop the LoRa module, use ESP32's built-in WiFi. That saves $20. Now you're at $35.

Student: "But what if there's no WiFi at the deployment site?"

Good question. Is WiFi available at your deployment site?

Student: "Yes, it's indoor in a building"

Then use WiFi. LoRa is for when you need kilometers of range outdoors. Document this decision: "Initially considered LoRa for long range, but deployment site has WiFi coverage. Using ESP32 built-in WiFi reduces cost by $20 and simplifies design."

---

**Student 3: "I'm not sure if my MCU has enough memory"**

What MCU are you using?

Student: "Arduino Uno"

What does your code do?

Student: "Read sensor, publish to MQTT, store 100 samples in buffer"

Okay, Arduino Uno has 2kB SRAM. MQTT library uses about 1kB. Your 100-sample buffer: what's the sample size?

Student: "4 bytes per sample"

So 400 bytes for buffer. Total: 1kB + 400 bytes = 1.4kB. You have 2kB. Margin: 30%. That's tight but okay. Document this calculation in your BOM notes.

Student: "What if I need more buffer later?"

Then you'll need to upgrade to a different MCU. But for your MVP, this works. If you want more headroom, consider ESP32 (520kB SRAM) for $9. Only $3 more than Arduino Uno, and you get WiFi built-in.

---

## Closing (1 min)

Alright everyone, you have until end of day to submit. Use the examples I showed you. Justify every component. Cite every datasheet. Think about scalability.

Remember: this goes in your portfolio. Make it something you're proud to show an employer.

See you next week for our online hands-on sessions. We're going to simulate the entire pipeline before the hardware arrives, so you hit the ground running on March 29.

Good luck!
