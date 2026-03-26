# Module 9: AI/MLOps for IoT — Lecture Script
### EECE5155 | Northeastern University | Spring 2026
> Casual, conversational. Think out loud, build intuition, tie concepts back to the student's own project.

---

## Slide 1 — Title Slide

"Alright, today we close the IoT data pipeline. Modules 6, 7, 8 — data flow, storage, model building. Today's question is: you deployed your model, now what? Because that's where most teams fall apart. If you're thinking about your semester project right now — good. Will your model still work six months from now? That's what today is about."

---

## Slide 2 — Learning Objectives

"Six things by end of today. The big ones: why models break after deployment, how to push a new model wirelessly to a device, and two architectures — TinyML and Federated Learning — that make ML work on tiny hardware.

Quick context: Module 8 was building a model. This module is keeping it working. You need both to ship a real product."

---

## Slide 3 — Outline

"Here's the plan. We start with the basics — what a model is, how much data it needs, where it runs. Then we get into what goes wrong and the engineering tools to fix it. Let's go."

---

## Slide 4 — What Is a Machine Learning Model?

"Let's make sure we're all talking about the same thing. A model is a function. Numbers go in, a decision comes out. Look at the examples — vibration in, fault probability out. Temperature plus occupancy in, heating decision out. Heart rate in, anomaly flag out.

Think about your own project. What is your input? What do you want out? If you can write it as 'X in, Y out,' you've defined your model's job.

Now the important part: the model has zero understanding of the physics. It learned patterns from labeled examples. The moment the input data looks different from training, quality drops. And it won't crash — it'll keep returning numbers. It'll just be wrong."

---

## Slide 5 — How Much Data Does a Model Need?

"Real numbers: simple two-class classification, 500 to 2,000 labeled examples per class. Vibration fault detection, months of normal data plus 50 to 200 actual fault events — which are rare, so you wait or run destructive tests. Keyword detection like 'Hey Siri,' around 10,000 recordings per word.

Here's what catches most teams. Getting data is easy — sensors run 24/7. The bottleneck is labels. Every labeled example needs a human expert or a confirmed physical event. That's slow and expensive.

And watch out for the seasonal trap. If you collect everything in fall, your model drifts the moment winter hits. You need a full year to cover all conditions. For your projects: start labeling now if you're doing anomaly detection."

---

## Slide 6 — Where and When Does the Model Run?

"Two decisions for every IoT ML system: where and when.

Where: edge or cloud. Edge means on the device itself — real-time, no network needed, model must be tiny. Cloud means you send data up, get a result back — more powerful, but 50 to 200 ms of round-trip latency.

When: streaming or batch. Streaming means every sample, every few ms — for safety-critical stuff. Batch means accumulate data over an hour or a day, run once — lower power, fine for summaries.

Real examples: safety sensor on a press machine — streaming, edge, every 10 ms. If you need a cloud round-trip for that, someone gets hurt before the answer arrives. Smart meter fraud — batch, cloud, once a day. A 6-hour delay is totally fine there.

So where does your project sit? That choice shapes your whole architecture."

---

## Slide 7 — The MLOps Loop: 6 Steps

"Six steps. Steps 1 through 5 you know from Module 8: collect labeled data, clean, train in the cloud, test, deploy. That's the build phase. Step 6 is monitoring — after deployment you watch whether the model is still accurate, compare predictions to ground truth, and when accuracy drops, you loop back to step 1.

See the red dashed arrow in the diagram from Monitor back to Collect? That's the whole point of this module.

Training is heavy — cloud, hours, offline. Inference is lightweight — device, milliseconds, always-on. Most teams skip step 6. That's why 3 out of 4 deployed IoT models lose significant accuracy within 6 months."

---

## Slide 8 — Model Degradation: The Core Engineering Problem

"Concrete scenario. You deploy a fault-detection model at 91% accuracy to 500 machines. One year later it's at 67%. Same code, same hardware, no firmware changes. What happened?

Four realistic causes: motors aged and their vibration signature shifted; summer temperatures changed bearing dimensions; a firmware update changed how the ADC encodes values; and training data was only collected in winter on brand-new machines.

What makes this hard to catch: a software bug throws an exception, you see a crash log. A degrading ML model keeps running and returning numbers. It looks healthy until a real fault gets missed and a machine actually breaks.

The number: 3 out of 4 deployed IoT ML models degrade significantly within 6 months. Only 1 in 4 teams has monitoring to catch it."

---

## Slide 9 — What is MLOps?

"MLOps — Machine Learning Operations. Keeping a deployed model accurate over time.

The car analogy: training is building the car, inference is driving it, monitoring and retraining is scheduled maintenance, pushing a new model OTA is a safety recall.

The Nest example is real. In 2023 a bad model update went to 45,000 thermostats, homes were heating wrong. Google pushed a corrected model to all of them in 6 hours — no site visits. That's a working MLOps pipeline under failure conditions."

---

## Slide 10 — The MLOps Lifecycle: The Continuous Loop

"Six stages, and one key property: it loops. There's no 'done.'

Collect labeled data, prepare features, train in cloud, validate on test set, deploy via OTA, monitor for drift. When monitoring catches a problem, back to step 1.

The key point: the model on day 1 and the model on day 180 are typically different versions. MLOps tracks which version is on which device, so when something breaks you know exactly what was deployed, when, and to what hardware."

---

## Slide 11 — Why IoT Makes This Much Harder

"This table shows how much the standard ML toolchain assumes.

Cloud ML: clean data, GPU, always-on network, one server. PyTorch, TensorFlow — all designed for this.

IoT ML: sensors drop out, microcontroller RAM in kilobytes, slow unreliable network, model updates must reach 100,000 different hardware variants, sensitive data must stay on-device. And every device drifts at its own rate.

None of the standard assumptions hold. That's why TinyML, Federated Learning, OTA pipelines, and continuous monitoring all exist — each one solves a constraint standard tools ignore."

---

## Slide 12 — Concept Drift: When the World Moves and the Model Does Not

"Concept drift: the model weights are frozen at training time, but the physical world keeps changing.

Three common IoT causes: seasonal shift — summer vibration spectrum differs from winter because of thermal expansion; sensor aging — ADC gain drifts over months; process change — machine now runs at 60% rated speed after maintenance.

How you detect it: accuracy declines gradually over weeks, the model starts over-predicting 'normal,' confirmed faults start getting missed.

The fix: collect fresh labeled data under current conditions, retrain, validate, deploy via OTA. But this only works if step 6 — the monitoring — is actually running."

---

## Slide 13 — Concept Drift: A Visual Intuition

"Look at the plot. X-axis is vibration amplitude, Y-axis is the model's confidence it's seeing normal operation.

Blue curve — learned in January. At that vibration level it says 'fault.' Correct in January.

By July, thermal expansion shifts the whole distribution. That same amplitude is now normal summer operation. The model still says 'fault' — it's a false alarm. And it'll keep doing that 4 times more often than before until you retrain it on summer data.

3 out of 4 industrial IoT deployments show this within 6 months. Teams retraining every 45 days with fresh data keep accuracy stable."

---

## Slide 14 — OTA Updates

"OTA is how you push the retrained model to every device. Same idea as an OS update on your phone — a binary gets pushed wirelessly and installed. Except your package is a new inference model targeting an embedded chip with constrained flash.

The flow: cloud detects drift, retrains, signs the model binary, pushes it. Device checks the signature, installs. If the download fails, the device keeps the previous version automatically.

Critical discipline: never push to 100% of the fleet immediately. Deploy to 1% first, watch for 24 hours. If metrics look good, expand to 10%, then 100%. If something regresses, automated rollback fires.

Tesla does this with every Autopilot update — 1.5 million vehicles, per-vehicle telemetry, staged rollout. Same discipline whether you have 1,000 sensors or 1.5 million cars."

---

## Slide 15 — Where Does the Model Run?

"The spectrum: microcontroller, Raspberry Pi, edge GPU, edge server, cloud. Compute and cost go up left to right.

For IoT the interesting territory is the left side — that's where the constraints are hardest.

Why run inference on the device? Latency: sub-millisecond on-device versus 50-200 ms cloud round-trip — for safety or control loops, cloud is too slow. Bandwidth: send one score instead of the raw 10 kHz sensor stream. Privacy: sensitive data stays on-device. Resilience: keeps working without connectivity.

The tradeoff: microcontroller handles TinyML classifiers, Raspberry Pi handles medium CNNs, cloud handles anything. Design rule: push inference as close to the sensor as accuracy and power budget allow."

---

## Slide 16 — TinyML

"The gap: a standard model is ~100 MB, a sensor MCU has ~256 kB of flash. TinyML closes it with two techniques.

Quantization: convert weights from FP32 to INT8 — 4x smaller, less than 1% accuracy loss on most IoT tasks. Like converting a RAW photo to JPEG: same content, fraction of the size.

Pruning: 80-90% of trained weights are near zero anyway. Set them to zero and skip those computations. Structured pruning cuts compute, unstructured cuts size.

What you can run today: gesture recognition on an Arduino Nano 33 BLE, keyword detection on a fingernail-sized chip, vibration anomaly on a $3 MEMS module. For your projects: Edge Impulse gives you a full browser workflow — collect, train, quantize, deploy. TFLite Micro if you want lower-level control."

---

## Slide 17 — Federated Learning: Training Without Sharing Data

"The problem: you have 500 million keyboard users. Their typing data would make autocorrect way better. But it's private, legally protected, and you can't pull it to a server. So how do you improve the model?

Federated Learning flips the flow. Instead of bringing data to the model, you bring the model to the data. The server sends the current model to every device. Each device trains locally for a few steps, computes gradient updates — the direction the weights should shift — and sends only those back. The server aggregates all updates, improves the global model, redistributes it. Repeat.

Blue arrows down in the diagram: model going out. Red dashed arrows up: gradient updates coming back, not raw data.

Google Gboard: 500 million Android devices, training locally, gradient updates aggregated centrally. Model quality within 2% of centralized training. Keystrokes never leave your phone."

---

## Slide 18 — Federated Learning: Why It Matters for IoT

"Three IoT constraints it solves at once.

Privacy by architecture: raw data stays on-device. HIPAA and CCPA compliant by construction, not by policy.

Bandwidth: a 10 kHz vibration sensor produces megabytes per minute. A gradient update is hundreds of bytes. That's a 1,000x reduction — the difference between feasible and impossible on NB-IoT or LoRa.

Compliance: HIPAA, CCPA, trade secrets — satisfied by architecture.

Accuracy trade-off: 2-5% below centralized baseline. Acceptable for most IoT tasks.

Who uses it today: Google Gboard, Apple Siri, NVIDIA FLARE for hospital imaging, Amazon Halo. Shipping products, not prototypes."

---

## Slide 19 — Case Study 1: Predictive Maintenance

"500 CNC machines, each with a vibration sensor. Goal: predict bearing failure 5+ days ahead so maintenance can be scheduled.

Architecture: sensor feeds an inference chip on the machine. Chip runs the fault-probability model locally — edge, streaming. It sends one score per interval over MQTT to the gateway. Cloud monitors the score distribution for drift. When drift is detected, it retrains and pushes an updated model to all 500 chips via OTA.

Why edge inference? At 10 kHz, three axes, 500 machines simultaneously — that would saturate any reasonable network. The edge chip compresses a continuous vibration stream down to one number per interval.

Results: failures predicted 5-8 days out, unplanned downtime down 62%, maintenance costs down 40%, full OTA update to 500 devices in under 4 hours.

Your project might not be 500 CNC machines. But if you're doing anomaly detection on continuous sensor data, this is the architecture template."

---

## Slide 20 — Case Study 2: Energy Theft Detection

"50,000 smart meters. Goal: detect energy theft while satisfying CCPA.

The constraints force the architecture: consumption profiles reveal occupancy patterns — CCPA classifies this as personal data in California. NB-IoT link budget limits raw transmission. And 50,000 meters means 50,000 different usage patterns that a single global model won't cover well.

Solution: federated learning. Each meter trains a local anomaly model on its own 30 days of consumption history. Only weight updates go to the server, not consumption records. Server aggregates all 50,000 updates and redistributes an improved global model.

Results: 88% theft detection accuracy. Centralized baseline with full data access: 94%. 6% gap. Zero raw consumption records on any server. CCPA satisfied by architecture — not as an add-on."

---

## Slide 21 — Responsible AI

"Last topic, and one you'll run into in your career.

Training data bias: the FDA documented a pulse oximeter that was 3x less accurate on patients with darker skin — because the training cohort was underrepresented. It was already deployed in hospitals when this was found. For your project: does your training data cover the full range of conditions and users your device will actually see?

Auditability: if your deployed model makes a consequential error — missed safety event, wrong medical reading — you need to answer: which model version, trained on what data, deployed when. If you can't answer that, you can't debug it and you can't defend the design.

Minimum model card: version per device, training dataset description, known failure modes, accuracy log over time.

US context: FDA 510(k) requires model cards for AI medical devices. FTC covers consumer AI. Product liability applies. Document your systems."

---

## Slide 22 — Summary

"Six things to take away.

Models degrade — 3 in 4 IoT models drift within 6 months. Active monitoring is required.

MLOps loop — monitor, detect drift, collect fresh data, retrain, deploy OTA. Runs continuously.

OTA — staged rollout, 1% first, automated rollback. Every device tracked by version.

TinyML — FP32 to INT8 quantization plus pruning. Runs on a $3 chip.

Federated Learning — gradient updates only, raw data stays on-device. Solves privacy, bandwidth, and compliance at once.

Model documentation — version, training data, failure modes, accuracy log.

Module 8 was building the model. Module 9 is operating it at scale. Both are required."

---

## Slide 23 — Questions?

"That's the full IoT data pipeline — Modules 6 through 9. Data transport, storage, analytics, operational ML.

Before you leave: think about your project. Where does your model run? Streaming or batch? How will you know if accuracy starts dropping? You have the vocabulary and architecture patterns to answer those now.

Questions?"

---

*End of script. 23 slides | ~70 minutes*
