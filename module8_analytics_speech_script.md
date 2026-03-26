# Module 8: Data Analytics for IoT — Oral Lecture Script
### EECE5155: Wireless Sensor Networks and the Internet of Things
**Speaker:** Eduardo Baena | Northeastern University | Spring 2026
> **Audience note:** Students have statistics background (regression, mean/variance, correlation) but no prior ML knowledge. Every ML concept is introduced from first principles.

---

## Slide 1 — Title Slide

"Welcome back, everyone. Module 8: Data Analytics for IoT.

We've spent seven modules getting data from the physical world into a database. Sensors, digitization, protocols, storage. And now I want to ask you the uncomfortable question: *so what?* Data sitting in InfluxDB at 3 a.m. is not preventing any failures and not making anyone any money. It's just data.

Today we talk about turning raw sensor readings into **decisions**. That's the whole point of Module 8."

---

## Slide 2 — Learning Objectives

"Six things by the end of today. The four analytics types and when to use each. The complete six-stage IoT analytics pipeline. Time-series feature engineering — the part that actually determines whether a model works. Algorithm selection for IoT. Stream analytics for real-time intelligence. And data quality, because 70% missing data is not a corner case in IoT — it's Tuesday.

Notice the connection to Module 7: storage is infrastructure, analytics is intelligence. You cannot do the second without the first."

---

## Slide 3 — Outline

"Roadmap: motivation and the Four Vs, then the pipeline end-to-end, data quality, feature engineering, then our new section on ML fundamentals — because I know many of you haven't seen this before — then algorithms, stream analytics, visualization, digital twins, and the frontier: foundation models. Let's go."

---

## Slide 4 — IoT = Data = Value

"18.8 billion connected IoT devices in 2024. 40 billion by 2030. And McKinsey estimates that most organizations use **less than one percent** of their IoT data. The rest sits untouched in cold storage. Why? Because collecting data is easy — you put a sensor somewhere and it starts publishing. Analysis is hard. The McKinsey value estimate for IoT analytics by 2030: **5.5 to 12.6 trillion dollars**. The bottleneck is not the sensors. It's the analytics layer. That's what we're building today."

---

## Slide 5 — Big Data and the Four Vs

"Volume: 10,000 sensors at 1 Hz gives you 864 million readings per day. Velocity: vibration sensors stream at 50 kHz — faster than most systems can process. Variety: temperature, vibration, pressure, power meters, GPS — all different schemas, sampling rates, units. Veracity: 5 to 70% missing data in real deployments. Stuck values. Sensor drift. Out-of-order packets from QoS 1.

IoT adds a fifth V: **Value**. Raw data has no value. The analytics pipeline is the transformation."

---

## Slide 6 — Types of Analytics

"Four levels, like a staircase. Each step gives more value but requires more engineering.

**Descriptive**: what happened? Averages, trends, dashboards. Every Grafana chart you've ever seen is descriptive analytics.

**Diagnostic**: why did it happen? Correlation, root cause. 'Anomalies happen on Tuesdays after maintenance because maintenance resets a calibration parameter.'

**Predictive**: what will happen? This is where machine learning enters. 'Based on current vibration patterns, this bearing will fail in 12 days.' I'll explain exactly how that works shortly.

**Prescriptive**: what should I do? Optimization plus prediction. 'Schedule maintenance for Day 8, order the part today.' This is the highest value tier.

Most organizations are stuck at descriptive. Everything from here on is about climbing the staircase."

---

## Slide 7 — The IoT Analytics Pipeline

"Six stages. Collection: MQTT, CoAP, OPC-UA from Module 6 — sensors are dumb and unreliable. Ingestion: Kafka at 605 MB/s peak throughput. Cleansing: missing values, outliers, duplicates — the hard work starts here. Integration: join sensor streams with maintenance logs, weather data, shift schedules — context turns raw readings into meaning. Analysis: the statistics and ML layer. Delivery: dashboards, alerts, APIs, automation.

Every stage can be a failure point. A perfect model running on dirty data gives you garbage predictions."

---

## Slide 8 — Data Collection and Ingestion Challenges

"In a real deployment you have sensors from five manufacturers, three firmware versions, one miscalibrated from the factory. And here's the one that surprises engineers: when a network outage ends, all 10,000 devices reconnect simultaneously. That's called a **reconnection storm** and it can take down a perfectly healthy broker.

For ingestion: batch is periodic and cheap; micro-batch is seconds; stream is milliseconds. You pick based on your latency requirement. And the key principle: **ingest first, validate later**. Never do synchronous schema validation at the broker at IoT scale — it becomes a bottleneck. Ingest everything, clean it downstream."

---

## Slide 9 — Data Quality

"A review of 57 IoT publications identified 8 error types. Missing data. Stuck values — sensor repeats the last reading indefinitely, looks valid to a naive system. Noise spikes — a bit flip gives you a temperature reading of 10,000 degrees. Sensor drift — calibration shifts 2% per year. Timestamp errors — clock skew between devices. Duplicates from QoS 1 at-least-once delivery. Out-of-order packets.

And the number that should concern you: **70% missing data rate** in some deployments. If you train a model on data with 70% gaps and assume those gaps are random, your model will be systematically wrong in ways that are very hard to debug."

---

## Slide 10 — Preprocessing Pipeline

"Six steps every IoT dataset goes through. **Outlier detection**: flag physically impossible values using mean ± 3σ with an adaptive window because the baseline drifts over time. **Imputation**: forward-fill for slow sensors — if temperature was 22°C a minute ago and the reading dropped out, 22°C is a better estimate than zero. **Smoothing**: Kalman filter if you have a physical model, exponential moving average if you don't. **Normalization**: z-score for Gaussian features, min-max for bounded signals. **Timestamp sync**: resample everything to a common grid. **Feature extraction**: the real engineering starts here."

---

## Slide 11 — Time-Series Feature Engineering

"The raw sensor value is almost never the right model input. You need **features** — computed summaries of a time window of data.

Example: vibration sensor at 10 kHz. You do not feed 10,000 raw numbers per second to a model. Instead, over each 1-second window, you compute: mean, standard deviation, min, max, skewness — is the distribution lopsided? — kurtosis — are there heavy tails? — and the zero-crossing rate — how often the signal crosses zero per second.

For rotating machinery, frequency-domain features are especially powerful. You take the FFT of the vibration window. Bearing faults produce energy peaks at specific, predictable frequencies determined by shaft speed and bearing geometry. This is deterministic physics — no ML needed to know *which* frequency to watch. ML then learns the boundary between healthy and faulty magnitude at that frequency.

Automated tools: tsfresh computes 787 features automatically. ROCKET applies 10,000 random convolutional filters and is state-of-the-art on time-series classification. Sometimes brute force wins."

---

## Slide 12 — What Is a Machine Learning Model? *(new intro slide)*

"Before we look at algorithms, I need to make sure we're all starting from the same place — because I know many of you haven't formally studied ML before.

You already know linear regression. You have a dataset, you fit a line y = mx + b, and you choose m and b to minimize the sum of squared errors between your predictions and the real values. That's it. That's a model.

Machine learning generalizes that idea in two directions. First, instead of just a line, you can fit *any* mathematical function — a curve, a surface, a highly non-linear boundary. Second, instead of just one or two inputs, you can have hundreds of inputs simultaneously — all your sensor readings, all your engineered features, all at once.

The key vocabulary. **Training**: the process of finding the best function parameters using historical data — it's automated curve fitting. **Inference**: taking a trained model and applying it to new data you've never seen before. That's what runs in real time on your IoT system.

And the three problem types you'll encounter in IoT. **Classification**: the output is a category — 'fault A', 'fault B', 'normal'. **Regression**: the output is a number — 'this bearing has 12 days of remaining useful life'. **Anomaly detection**: the output is a score of how unusual the data looks — 'this reading is 3 standard deviations from what the model expects as normal'.

Training is done offline, historically, once. Inference runs live, constantly, on every new sensor reading. That distinction matters for everything we'll discuss in Module 9."

---

## Slide 13 — Training, Validation, and the Bias-Variance Tradeoff *(new intro slide)*

"Once you have a trained model, how do you know it's actually good — and not just good at memorizing the data you showed it?

This is the most important practical question in ML engineering. You split your data into three parts. **Training set** — 70% — the model sees this and learns from it. **Validation set** — 15% — you use this to tune your choices, pick hyperparameters, catch problems. **Test set** — 15% — you touch this only once, at the very end, to get an honest estimate of real-world performance.

Two failure modes. **Underfitting**: the model is too simple. You fit a straight line to data that's clearly curved. High error everywhere. **Overfitting**: the model is too complex. It memorizes every quirk and noise spike in the training data. It gets 99% accuracy on training data and 60% on new data. This is the more common failure in practice.

Now here's the IoT-specific trap. Time-series data is **not** independent and identically distributed. Tomorrow's vibration reading from a motor depends on today's reading. It depends on wear, temperature history, load cycles. If you randomly shuffle your data before splitting into train/test — which is what you'd do with a standard tabular dataset — your model will appear to work perfectly because it's seen readings from around the time period it's being tested on. That's called **data leakage** and it will fool you completely.

The correct approach for time-series is **walk-forward validation**: always train on the past, test on the future. Period.

And for metrics: never use accuracy alone when your dataset is imbalanced. If only 1% of your sensor readings are faults, a model that predicts 'normal' for everything gets 99% accuracy and misses every single fault. Use **F1-score**, which is the harmonic mean of precision and recall. Use precision-recall AUC. These metrics are honest."

---

## Slide 14 — Why Neural Networks? From Regression to Deep Learning *(new intro slide)*

"We established that ML is generalized curve fitting. So why do we need neural networks? Why not just use polynomial regression or decision trees for everything?

Because sensor faults are **not linearly separable** in raw feature space. The boundary between 'healthy bearing' and 'early-stage outer race fault' in a 64-dimensional feature space is a complex non-linear surface. A polynomial regression of degree 2 can approximate simple curves. A neural network can approximate *any* function — that's what the universal approximation theorem tells us.

Here's the intuition for how a neural network extends linear regression. Start with your linear model: y-hat = w1·x1 + w2·x2 + b. Now wrap the output in a non-linear function — for example, ReLU, which outputs max(0, z). That little non-linearity is what unlocks the power. Stack multiple layers of these operations, each one transforming the output of the previous layer, and you get a network that can represent extraordinarily complex mappings. Training uses gradient descent: you measure the error, compute which direction each weight should change to reduce that error, and update all weights by a tiny step in that direction. Repeat millions of times.

For IoT, we use four architectures. **Random Forest**: an ensemble of decision trees, each trained on a random subset of data and features, voting on the final answer. No gradient needed. Robust to noise. Interpretable. **LSTM** — Long Short-Term Memory: a recurrent network with an explicit memory cell that can remember patterns across long sequences of readings. Ideal when the answer depends on what happened 10 or 100 timesteps ago. **CNN** — Convolutional Network: applies sliding filters across the input, detecting local patterns like frequency peaks in an FFT. **Autoencoder**: a network trained to compress the input into a small bottleneck and then reconstruct it. Train it only on normal data — then when an anomaly arrives, it can't reconstruct it well, and the reconstruction error becomes your anomaly score.

You do not need to implement backpropagation or derive the gradient equations. You need to understand what each architecture is designed to capture and why that maps to your IoT problem."

---

## Slide 15 — ML for IoT: Algorithm Selection

"Now that we have the fundamentals, let's match algorithms to problems.

For **classification** — fault type, operating mode, quality class — start with **Random Forest**. It handles noisy sensor data well, it doesn't overfit easily, and you can ask it which features were most important. **XGBoost** is more accurate if you have clean, well-engineered features. These two win most tabular ML competitions for a reason.

For **regression** — Remaining Useful Life, energy demand, temperature forecasting — you need something that understands *sequences*, because the prediction depends on the history of readings. That's where **LSTM** comes in. It maintains a memory state across time steps, so when it predicts RUL at time T, it's incorporating everything it saw at T-1, T-2, T-10, and so on. **CNN-LSTM with Attention** is the current state of the art: the CNN extracts local patterns from each window, the LSTM models how those patterns evolve over time, and the attention mechanism learns which past time steps matter most for the current prediction.

For **anomaly detection** — the IoT-specific challenge — you almost never have labeled fault data. Faults are rare and often happen for the first time after deployment. So you train only on normal operation data. **Autoencoder**: train it to reconstruct normal readings, then flag anything with high reconstruction error. **Isolation Forest**: randomly partition the feature space — anomalies are statistically easier to isolate and land in shorter random paths.

For **forecasting**: Prophet for data with known seasonal patterns and trends. LSTM for complex learned temporal dependencies. And in 2026, foundation models for zero-shot forecasting when you have no historical data at all."

---

## Slide 16 — Predictive Maintenance: The Flagship Application

"Predictive maintenance is where IoT analytics pays off most visibly. The goal: predict equipment failure before it happens, giving you a maintenance window instead of an unplanned outage.

The pipeline: vibration sensors at 10 to 50 kHz → MQTT → Kafka → feature extraction on sliding windows → model → RUL output. Remaining Useful Life: not 'is it failing right now?' but 'how many operating cycles until it will fail?'

The model architecture: CNN on the FFT extracts frequency-domain patterns. LSTM models how those patterns change over time as the bearing degrades. The attention layer learns to weight recent readings more heavily when degradation accelerates. On NASA's C-MAPSS benchmark — a turbofan engine dataset with 21 sensors and 4 failure scenarios — this architecture achieves an RMSE of 11 to 14 cycles. That means we predict failure within about 12 engine cycles on average. That's enough lead time to order the part and schedule the shutdown.

Real-world results from McKinsey: PdM reduces maintenance cost by 40% and unplanned downtime by up to 50%. EUROGATE, a major European port operator, achieved 92% fault prediction accuracy in production — not in a research lab."

---

## Slide 17 — Anomaly Detection for IoT

"The fundamental challenge: you're looking for something that almost never happens, in data you don't have labels for.

Anomaly rates in IoT are typically under 1% of all readings. If you train a classifier and it predicts 'normal' for every single input, it will achieve 99% accuracy. And it will miss every fault. This is why **accuracy is the wrong metric** for anomaly detection. Always use F1-score or precision-recall AUC for imbalanced IoT datasets. I will check this in your assignments.

Two approaches that work without labels. **Autoencoder**: trained only on normal data. It learns to compress normal vibration patterns into a small latent representation and reconstruct them with low error. When an anomaly arrives — bearing roughness, resonance at a new frequency — the reconstruction fails and the error spikes. You set a threshold on that error. Anything above the threshold is flagged. No labeled faults needed.

**Isolation Forest**: imagine randomly partitioning the feature space with axis-aligned cuts. Normal points are densely clustered — it takes many cuts to isolate one. Anomalous points sit alone in sparse regions — they get isolated in very few cuts. The number of cuts to isolate a point becomes your anomaly score. Elegant, fast, scales to streaming.

The state-of-the-art in 2026: DCDetector from KDD 2023 uses dual attention and contrastive learning for multivariate time-series. PatchTST treats time series as patches of tokens, like a vision transformer. Both significantly outperform older approaches on standard benchmarks."

---

## Slide 18 — Stream Analytics: Real-Time Intelligence

"Everything so far has been batch analytics — you have a dataset, you run a model. But IoT is a stream. A bearing fault doesn't wait for you to run a daily batch job.

**Apache Flink** is the answer for true sub-100 millisecond streaming. Events are processed one by one as they arrive. Flink maintains state across events — so your sliding window feature extractor is always up to date. It handles out-of-order arrivals using watermarks — because a packet sent at T=100 sometimes arrives after T=101. And it guarantees exactly-once semantics using distributed checkpointing.

**Spark Structured Streaming** is the micro-batch option: 100 milliseconds to 1 second windows. It uses the same DataFrame API as batch Spark, which means your data engineers can reuse existing code. Best for: running inference with a model trained in batch.

**Kafka Streams and ksqlDB**: no separate cluster, runs embedded inside Kafka. You write SQL-like queries over your MQTT topic stream. You can embed a trained XGBoost model as a user-defined function. Every sensor reading that arrives from MQTT gets scored before it's even stored. If the anomaly score exceeds a threshold, a downstream Kafka topic fires an alert. Sub-5 millisecond end-to-end latency from MQTT publish to alert."

---

## Slide 19 — Windowing in Stream Analytics

"All stream analytics reduce to one operation: apply a function over a window of data. The window type determines what question you're asking.

**Tumbling windows**: fixed size, no overlap. Every event belongs to exactly one window. You compute mean and standard deviation of the last 5 minutes of temperature, fire the result, start fresh. Clean, no double-counting, simple.

**Sliding windows**: fixed size, overlapping. A 5-minute window that slides every 30 seconds. You get two windows per minute. More overlap means more training samples but more correlation between consecutive windows — which matters for ML.

**Session windows**: variable length, event-triggered. The window opens when the machine starts up and closes after a configurable idle gap. Perfect for analyzing a motor's startup transient, which has different normal behavior than steady-state operation.

The Flink SQL example on the slide: a 5-minute tumbling window computing mean and standard deviation from temperature readings. Three lines of SQL. The distributed execution, fault tolerance, and exactly-once delivery are handled by Flink. You write the logic; Flink handles the infrastructure."

---

## Slide 20 — Analytics Tools Landscape

"Let me map the tools to use cases so you can make the right choice in your projects.

**Batch/exploratory**: Apache Spark for large distributed datasets. And the 2026 addition: **DuckDB**. In-process SQL engine that reads Parquet directly from S3, no cluster required. Run it from a Python notebook and query years of IoT sensor data in seconds. For exploratory analytics on cold storage, DuckDB is the right answer now.

**Stream**: Apache Flink for sub-second. Kafka Streams for in-bus, embedded processing.

**ML frameworks**: scikit-learn for classical algorithms — Random Forest, XGBoost, Isolation Forest — all with a clean Python API. PyTorch or TensorFlow for deep learning — LSTM, CNN, Autoencoder. ONNX Runtime for deploying the same trained model on any platform, from a cloud server down to a Cortex-M4 MCU.

**Feature stores**: Feast and Tecton solve a specific production problem called training-serving skew. If you compute features differently during training and during live inference — even slightly — your model's real-world performance will be worse than your test metrics predicted. A feature store computes features from the same code for both, guaranteeing consistency.

**MLflow**: when you run 200 experiments comparing different window sizes and model architectures, you need to track every hyperparameter, every metric, every trained artifact. MLflow is the difference between organized engineering and total chaos."

---

## Slide 21 — Visualization and Delivery

"All the analytics in the world is worthless if no one sees the result.

**Grafana** is the IoT dashboard standard. Open source, native InfluxDB and Prometheus support, live panels updating at sub-second rates, alerting built in. If you're running an IoT system without a Grafana dashboard, you're flying blind.

For model serving — how do you expose your trained model as a live service? **FastAPI + Docker** for a REST endpoint: your IoT gateway sends a POST request with the feature vector, gets back an anomaly score. **KServe** for Kubernetes-native autoscaling — when inference load spikes, Kubernetes automatically adds pods. **ONNX Runtime** for edge deployment: export your PyTorch model to ONNX format once, and run it on any hardware with no code changes.

The latency contract — define this before choosing your architecture. Under 10 milliseconds: edge inference, the decision never leaves the device. Under 100 milliseconds: co-located API over LAN, acceptable for real-time control. Under 1 second: cloud API, acceptable for UI responses. Over 1 second: async pipeline, fire-and-forget, result arrives later. The wrong architecture for your latency SLA is a design error that's expensive to fix later."

---

## Slide 22 — Digital Twins

"A digital twin is a living virtual model of a physical asset, continuously synchronized with real sensor data.

Four maturity levels. L1 Descriptive: a static 3D model. L2 Informative: connected to real-time IoT streams — the twin reflects what the asset is doing right now, live. L3 Predictive: an ML model runs inside the twin and forecasts 'this bearing will fail in 12 days.' L4 Living: the twin not only predicts but acts — it autonomously adjusts set points, generates work orders, reroutes production.

The 2026 state of the art is **hybrid physics-ML twins**. You use the physics model — the equations of motion, the fatigue crack propagation equations you learned in mechanics — for the failure modes you already understand analytically. You add an ML layer to catch deviations from expected behavior that the physics model never anticipated, because real machines do not behave exactly as the equations say.

Numbers from a real deployment: manufacturing press digital twin, 87% accuracy predicting failure 5 to 8 days ahead. Downtime reduced by 62%. Component life extended from 3,000 to 4,200 cycles. These are audited production results, not lab benchmarks.

The market: 13.6 billion dollars in 2024, projected to 428 billion by 2034. Azure Digital Twins, AWS IoT TwinMaker, Siemens Xcelerator are the enterprise platforms."

---

## Slide 23 — Time-Series Foundation Models (2025–2026)

"This is the most important development in IoT analytics in the last two years, and I want you to understand why it changes the game.

The traditional ML workflow requires labeled historical data. New deployment? No data yet. New failure mode? No labeled examples. That means months of data collection before you can build anything useful.

Foundation models break this constraint. They are massive models pre-trained on hundreds of millions of time-series sequences from diverse domains — energy, industrial, climate, finance. What emerges is a general understanding of how time-series data behaves: trends, seasonality, sudden changes, patterns of degradation. You give the model your recent sensor readings as a context window, and it forecasts the next N steps — **zero-shot**, with no training on your specific data.

**Chronos** from Amazon: evaluated zero-shot on 42 benchmark datasets, it matches models specifically fine-tuned on each individual dataset. A general-purpose model with no task-specific training, beating purpose-built models. That's a remarkable result.

**TimesFM** from Google: in-context adaptation — you give it a handful of examples from your domain and it adapts 16 times faster than traditional fine-tuning.

**MOMENT** from CMU: open-source, achieves F1 of 0.85 on network anomaly detection in zero-shot mode.

The practical decision tree for 2026: no labeled data? Start with Chronos or TimesFM, zero-shot. Have 3 to 6 months of domain data? Fine-tune. Pure unsupervised anomaly detection? Autoencoder on normal data. The 'collect data for 12 months before doing ML' paradigm is over for forecasting tasks."

---

## Slide 24 — Distributed Analytics Challenges

"When you scale from a lab prototype to a real deployment, three new problem categories appear.

**Decentralization**: you cannot move all the data to one place — bandwidth cost alone would be prohibitive. MapReduce gives you the conceptual framework: move the computation to the data rather than moving the data to the computation. **Federated learning** extends this to ML training: each device trains locally on its own data, and only the weight updates — not the raw sensor readings — travel to the central server. The global model gets trained on data it never directly sees. For GDPR-regulated data like energy consumption patterns from smart meters, this is not optional — it's legally required.

**Security and privacy**: the fast gradient sign method — FGSM — can craft an imperceptible perturbation of your sensor input that flips a correctly classified fault to 'normal'. Your anomaly detector becomes blind to a specific attack signature. NIST AI 100-2e2025 is a full taxonomy of adversarial ML threats. Read it before deploying any safety-critical IoT ML system.

**Interpretability**: in nuclear plant monitoring, aviation, and medical devices, 'the model said so' is not an acceptable explanation. **SHAP** gives you a contribution score for every feature — 'the bearing fault prediction was driven 67% by kurtosis and 22% by the 187 Hz peak.' **LIME** builds a local linear approximation around the specific prediction. **Attention maps** show which time steps the model focused on. If you cannot explain the prediction to an operator, the operator will not trust it. And in a safety-critical system, they should not."

---

## Slide 25 — Key Takeaways

"Let me close the loop.

**Pipeline**: six stages, every one a potential failure point. Ingest first, validate later. Data quality is not glamorous but it determines whether everything downstream works.

**ML in one sentence**: training is automated curve fitting on historical data; inference is applying the fitted curve to new sensor readings in real time.

**Algorithm selection by problem**: classification → Random Forest or XGBoost. Regression/RUL → CNN-LSTM-Attention. Anomaly detection without labels → Autoencoder or Isolation Forest. Forecasting without data → Chronos or TimesFM zero-shot.

**Stream analytics**: Flink for sub-100ms, Spark Streaming for micro-batch, Kafka Streams for in-bus scoring.

**2026 frontier**: foundation models matching task-specific fine-tuned models in zero-shot. Digital twins at L4 autonomous adaptation. Federated learning for privacy-by-architecture.

The core lesson: most organizations collect data. Very few extract value from it. The difference is a rigorous end-to-end analytics pipeline, designed from day one, that handles dirty data, trains models correctly, and delivers results to the people who need them.

You now have the vocabulary, the concepts, and the tools to build one."

---

## Slide 26 — Questions?

"That's Module 8.

Next time: Module 9 — AI/MLOps for IoT. We built the analytics pipeline today. Module 9 is about what happens *after* deployment: how do you keep a model working when the world changes around it? Concept drift, OTA model delivery, federated learning in production, TinyML on microcontrollers.

The model you trained today won't be correct forever. Module 9 is how you deal with that.

See you then. And remember — F1-score, not accuracy, on your homework.

*(pause)*

I will check."

---

*End of Module 8 oral lecture script.*
*Total slides: 26 (including 3 new ML intro slides) | Estimated delivery time: 90–105 minutes*
