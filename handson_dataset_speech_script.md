# Hands-On 3: IoT Dataset Analysis — Speech Script
**Date**: Thursday, March 26, 2026  
**Duration**: 60 minutes  
**Audience**: EECE5155 students (graduate level)  
**Context**: Students have Wokwi firmware (Mon) + Node-RED flows (Wed). Hardware arrives Mon Mar 30.

---

## Opening (2 min) — Slide 1

Alright, so this is our last hands-on before the hardware shows up on Monday, which is pretty exciting. We've done firmware on Monday, we built Node-RED flows on Wednesday, and today I want to do something a bit different — I want us to actually look at what real IoT data looks like when it comes out of a real deployment, because here's the thing: it's never clean. It's always messy.

So here's how today is gonna work, and I actually think you're gonna like this approach. For each exercise, the slide is gonna give you some questions to answer, and you can use whatever AI tool you want — ChatGPT, Copilot, Claude, doesn't matter — to write the pandas code, the matplotlib code, all of that. Just let the AI handle it, because honestly that's the easy part.

The hard part, and this is where your grade really comes from, is the interpretation. After your code runs, you need to write what the results actually mean, and AI can't do that for you because AI doesn't understand the physical context of the deployment. Like, if you see a 45°C reading in a lab that's supposed to be 22°C, is that real? Did someone open an oven? Is there a server rack nearby? Or is the sensor just broken? Only you can figure that out by thinking about what's actually happening in the real world.

Your grade is gonna be 40% for getting the code to work, and 60% for what you write about the results. And just so we're clear, if you paste AI-generated interpretation, you get zero on that section, so don't do it.

Go ahead and grab the starter notebook from the repo, and also download the LaTeX template because that's where you'll write your report. Let's get started.

---

## The Intel Lab Deployment (3 min) — Slide 3

Before we dive into the actual analysis, I want to tell you a bit about where this data comes from, because the context actually matters. This is the Intel Berkeley Research Lab dataset from 2004, and it's become one of the most cited IoT datasets in the world — people are still using it for research papers today, which is pretty remarkable when you think about it.

So they deployed 54 Mica2Dot sensor nodes across a computer science lab at UC Berkeley, and these little devices measured temperature, humidity, light, and battery voltage continuously for 38 days, which ended up being about 2.3 million readings total.

Now here's what I find really interesting. These nodes had 4 kilobytes of RAM — that's it, 4 KB — and your ESP32 has 520 KB, so you have way more headroom. They transmitted at 916 MHz while you're using WiFi, so the technology is totally different. But the data quality problems? They're identical. Sensors died, readings got corrupted, timestamps drifted — it's the same stuff you're going to see when you deploy your own MVPs.

So think of it this way: imagine you just inherited someone else's deployment. You've never been to the lab, you don't know where the sensors are physically located, you just have this CSV file. That's exactly the scenario we're practicing today, and it's exactly what you'll face when you start looking at data from your own deployments after Monday.

---

## IoT Data Lifecycle (3 min) — Slide 4

Take a look at this diagram, because this is really where things go wrong in IoT systems. Each stage in the pipeline has its own characteristic failure modes that you need to be aware of.

At the collection stage, you get ADC errors and stuck readings where the sensor just freezes on one value. At transmission, you see packet loss and duplicate messages from MQTT retransmissions. At storage, there's schema mismatches and clock drift between different nodes. At the cleaning stage, you might over-filter or impute missing values incorrectly. At analysis, the classic mistake is misinterpreting an outlier as something real. And then at the action stage, you either trigger false alarms or, worse, you miss a real event that you should have caught.

And here's the key insight that I really want you to take away: errors compound as they flow downstream. A stuck ADC at the collection stage ends up looking like a real outlier at analysis, which then triggers a false alarm at the action stage. One small problem at the beginning cascades into three problems by the end.

So today we're learning to catch these issues at the analysis stage, before they reach the action stage where they can actually cause harm.

Notice how this fits into our week. On Monday we built the collection and transmission layers with Wokwi. On Wednesday we built storage and action with Node-RED. Today we're filling the gap with cleaning and analysis, and after today you'll have the complete pipeline.

---

## Real-World Failures (3 min) — Slide 5

Let me walk you through three real documented cases, and I've put the source links on the slide so you can verify these yourself if you're interested.

The first one is the Nest Thermostat incident from January 2016. What happened was they pushed firmware version 5.1.3 out to devices in December, and then a few weeks later people started waking up to freezing cold homes because their thermostats had drained their batteries overnight and shut down completely. The sensor readings were actually fine, but the algorithm was misinterpreting the battery voltage curve during the firmware transition, and the result was a lot of very unhappy customers. Nest had to publish this nine-step recovery procedure, and I can only imagine what it was like to be an engineer there that week. The lesson here is that you need to test your entire data pipeline after every firmware update, because the interaction between firmware and data interpretation can break in unexpected ways.

The second case is from Helsinki in 2021, where they deployed 25 low-cost air quality sensors across the city. What they found was that the ozone and nitrogen dioxide sensors were cross-sensitive, meaning they respond to each other's gases, not just their target gas. On top of that, humidity was changing readings by 10 to 20 percent depending on conditions. Electrochemical sensors are just inherently messy like that. Without cross-validation against reference sensors, the data would have been completely misleading, and the lesson is that you should always compare sensors against each other and against known references when you can.

The third case is actually this dataset we're using today. You've got 54 sensors running for 38 days, and 18% of the temperature readings are above 40°C in a lab that's maintained at 20 to 22°C. That's obviously not real temperature data — it's sensor error. And over 10 sensors died during the deployment from ADC errors, dead batteries, radio failures. This isn't really a failure story, it's just what real deployments look like, and that's the point.

The common thread across all three cases is that the data was there to detect the problem. The issue wasn't missing data, it was that nobody was analyzing it properly. That's the skill we're practicing today.

---

## Step 0: Download the Dataset (5 min) — Slide 6

Okay, let's get everyone set up. Open a terminal and create a working directory — the commands are on the slide.

Then download the dataset itself, which is a gzipped text file hosted at MIT CSAIL, about 35 megabytes compressed. If the server is slow or down, which does happen occasionally, there's a GitHub mirror linked on the slide that you can use instead.

Once you have the file, open up Jupyter and load it with pandas. The file format is space-separated with no headers, so you'll need to specify the column names manually: date, time, epoch, moteid, temperature, humidity, light, and voltage. The moteid column is the sensor identifier, ranging from 1 to 54.

One thing that's really important: when you combine the date and time columns into a proper timestamp, make sure to use `errors='coerce'`. The reason this matters is that real IoT data always has malformed timestamps — it's just a fact of life. If you don't use coerce, your script will crash when it hits a bad timestamp. With coerce, the bad ones become NaN values that you can handle gracefully.

Run the code and you should see about 2.3 million rows load successfully. Then run `df.isnull().sum()` to check for missing values. You'll see that temperature has some, humidity has some — this is totally normal for real data. The question you need to think about is why they're missing, because that tells you something about what happened during the deployment.

---

## Exercise 1: Data Quality Audit (20 min) — Slide 7

**Run the first cell:** Load the data. You should see 2.3 million rows, 54 sensors.

---

**Cell: Readings per sensor**

Look at the histogram. Some sensors have 80,000 readings, some have fewer than 100. Mote 5 has only 35.

**Ask:** Why does Mote 5 have so few readings? What could have happened?

*Answer: Battery died early, radio failed, or physical damage. The data tells us which sensors were reliable without visiting the lab.*

---

**Cell: Temperature distribution**

Run this. You'll see a huge spike above 40°C.

**Ask:** The lab is 20-22°C. We have 18% of readings above 40°C. Are these real or sensor errors?

*Answer: Sensor errors. ADC glitches, stuck registers, electrical noise. This is why we need to clean the data.*

---

**Cell: Mote 49 time series**

Look at this plot. The sensor stops reporting partway through.

**Ask:** When did it stop? Was it gradual or sudden? What does that tell you?

*Answer: Sudden failure around day 10-12. Probably battery depletion or radio failure, not gradual degradation.*

---

**Key insight:** From raw data alone, without visiting the lab, we can identify which sensors were reliable, which failed, when they failed, and why. This is the skill you need for your MVP data.

---

## Exercise 2: Compare Sensors (20 min) — Slide 8

**Cell: Correlation matrix**

Run this. You'll see a heatmap of how well sensors agree.

**Ask:** Look at Motes 1 and 2. Correlation 0.98+. Why so high?

*Answer: They're physically next to each other. Same location = same temperature.*

**Ask:** Mote 34 is lower, around 0.94. Why?

*Answer: Physical distance. Different proximity to heat sources, air vents, or walls.*

---

**Cell: Temperature spread**

Run this. Look at the histogram.

**Ask:** Max spread is 120°C but median is <1°C. Why such a huge difference?

*Answer: Max is driven by outliers from Exercise 1 (sensor errors). Median is the real number — when sensors work, they agree within 1°C.*

---

**Key question for your PES:** Most of you wrote "accuracy ±0.5°C" in your requirements. But accuracy against what? This exercise shows you need a reference sensor to validate that claim. When we deploy Monday, what's your reference?

---

## Exercise 3: Anomaly Detection (15 min) — Slide 9

**Cell: Count anomalies by method**

Run this. You'll see two very different numbers.

**Ask:** Fixed threshold finds 6,700. Z-score finds 500. Only 10 overlap. Why such a huge difference?

*Answer: They detect fundamentally different things.*

---

**Cell: Visualization**

Run this. Look at the plot.

**Red dots:** Caught by fixed threshold (outside 15-35°C)  
**Green triangles:** Caught by z-score only (sudden jumps)

**Ask:** Why does fixed threshold catch so many more?

*Answer: It catches ALL readings outside the range, including constant high readings. Z-score only catches sudden changes.*

**Ask:** Which method would miss a sensor that's slowly drifting from 22°C to 28°C over a week?

*Answer: Fixed threshold would miss it (still in range). Z-score would catch it (rolling mean adapts slowly).*

---

**For your MVP:**
- **Fixed thresholds** → safety alerts (PES compliance)
- **Z-score** → drift detection (data quality monitoring)
- Use BOTH for complete coverage

---

## Class Discussion (5 min) — Slide 10

Let's compare what everyone found. How many sensors did people identify as dead? Did everyone get the same count? If not, why might the counts differ? Some of you probably filtered to motes 1 through 54, while others kept all the moteid values including corrupted ones, and that would give you different results.

What correlations did people get for the sensor pairs? Anyone find a pair with correlation below 0.8? That would be a red flag indicating a problematic sensor.

For anomalies, how many did each method find? Fixed threshold should be around 6,700, z-score around 500, and only about 10 overlapping. Why is there such a huge difference? That's really the key insight from today's session.

Here's a discussion question for you: a reading of 38°C appears once at 3 AM in the lab. Is it real or is it noise? How do you decide? Well, you'd check nearby sensors at the same timestamp to see if they agree. You'd check if the HVAC system has a shutdown schedule that might explain a temperature spike. You'd check if that particular sensor has a history of stuck readings. The physical context is what determines the answer, not the number itself.

---

## Thinking Like an Anomaly Detective (2 min) — Slide 11

So I want to take a step back and talk about the actual thinking process here, because this is more important than any particular method or algorithm.

When you're looking at IoT data and trying to figure out if something is wrong, you need to ask yourself four questions. First, is this physically possible? A 45°C reading in a 22°C lab is obviously not real, but 35°C outdoors in July might be completely normal. Second, do nearby sensors agree? If Mote 1 says 22°C and Mote 2 says 38°C at the exact same timestamp, one of them is wrong, and you can figure out which one by looking at their histories. Third, is this a sudden jump or a gradual drift? Sudden changes usually mean sensor glitches or real events, while gradual changes often indicate calibration drift or a slowly dying battery. Fourth, does it match a known pattern? HVAC systems cycle on and off, buildings have daily rhythms, occupancy follows weekly schedules — if something breaks that pattern, it's worth investigating.

There are some classic red flags you should watch for. If you see the same value repeated 100 or more times, that's almost certainly a stuck ADC. If a sensor's correlation with its neighbors drops suddenly, that sensor has probably degraded. If data goes missing at the same hour every day, there's probably a scheduled reboot or maintenance window. If values are outside the sensor's published specifications, you're probably seeing electrical noise. And if you see a bimodal distribution where there should be one peak, you might have two different operating modes that need to be analyzed separately.

The key skill here is that AI can compute all the z-scores and correlations you want, but only you can look at a pattern and say "this looks like a dying battery because I've seen this pattern before." That's engineering judgment, and that's what makes you valuable.

---

## Other Datasets (2 min) — Slide 12

So now that we've walked through all three exercises together on the Intel Lab dataset, your homework is to apply the exact same analysis to one of these other datasets on the slide.

I've got some great options for you here, including a couple that are much more recent than the 2004 Intel Lab data. First, there's the Google Smart Buildings Dataset from 2024. This is real data from three Google buildings collected over six years — temperature, humidity, occupancy, lighting levels, and energy consumption. It's available through TensorFlow Datasets, and what makes it really interesting is that it includes occupancy data, which the Intel Lab dataset doesn't have. So you can ask questions like "does the temperature spike when the room is empty?" which is actually a really useful thing to test for in building automation systems.

Second, there's the Numenta Anomaly Benchmark, or NAB. This one is special because it's the only dataset I know of that has labeled anomalies — meaning someone has already gone through and marked where the real anomalies are. So you can test your detection methods and actually know whether you caught the right things. It's got about 50 time series from AWS server metrics, traffic data, ad clicking metrics, all kinds of real-world sources. If you want to benchmark your anomaly detector objectively, this is the dataset to use.

The other ones on the slide are ASHRAE Energy for building HVAC patterns, UCI Air Quality for multivariate sensor fusion, and Kaggle Smart Home for home automation. All good options depending on what your MVP is focused on.

Your deliverable is a LaTeX report with the same three sections we did today: quality audit, sensor comparison, and anomaly detection. Use the template from the repo, fill in your interpretations, and submit on Canvas.

---

## Wrap-Up (3 min) — Slide 13

For deliverables, you're submitting a LaTeX report using the template from the repo. Fill in the results and interpretation for each exercise, replacing every "YOUR_TEXT" placeholder with your actual analysis. Include your three PNG plots — the template already has the image commands set up. Also submit your Jupyter notebook with the code. Remember, 40% for code working, 60% for interpretation quality.

Quick summary of our week: Monday was firmware, Wednesday was automation with Node-RED, and today was data analysis with AI-assisted coding and human interpretation.

Here's what I really want you to take away from today. We let AI handle the pandas and matplotlib code, and that's totally fine. In 2026, writing boilerplate data analysis code from scratch isn't the valuable skill anymore. The valuable skill is looking at your results and being able to say, "This sensor died from battery depletion, not network failure, and here's how I can tell from the data." That's what engineers actually do, and that's what the interpretation is really about.

Monday March 30, the hardware arrives. You're going to walk into that lab with tested firmware, a working Node-RED pipeline, and now the skills to analyze whatever your sensors produce.

Bring your laptops and your BOM printouts on Monday.

Any questions? Alright, see you Monday then.

---

## Quick Reference: Common Issues

**"Dataset download fails"** — Use the GitHub mirror linked on the slide, or download on your phone and transfer via USB.

**"Pandas can't parse the file"** — Make sure you're using `sep=r'\s+'` and `na_values=['']` because the file has inconsistent spacing between columns.

**"Timestamps are showing as NaT"** — Some rows have malformed date/time fields, which is expected with real messy data. Use `errors='coerce'` in your `pd.to_datetime()` call and then drop the NaN rows.

**"Plots are too slow or cluttered"** — You've got 2.3 million data points. Resample to 5-minute intervals using `df.set_index('timestamp').resample('5min').mean()` and your plots will render much faster.

**"Rolling z-score has NaN at the start"** — That's expected because the first hour of data has no rolling window to compute from. Just drop those NaN values before counting your anomalies.

**"Which dataset should I use for my final project?"** — For the final report, analyze your own sensor data that you collect after March 30. Today's exercise teaches you the methodology on a known dataset so you can apply the same approach to your own data later.
