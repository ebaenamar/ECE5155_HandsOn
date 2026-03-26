# Module 7: Data Storage for IoT — Oral Script

> **Style:** Engaging, entertaining, rigorous engineering professor. Conversational but precise. Think: the professor who makes you laugh but also makes you *learn*.

---

## Slide 1: Title Slide

Alright, welcome back everyone.

So last module we answered the question: *how do you get data from a sensor to a broker?* MQTT, Kafka, CoAP — we covered the pipes. The plumbing.

Today we're tackling the question nobody thinks to ask until they're three months into a project and their database is on fire.

*(pause)*

*Where does the data actually go?*

You've got 10,000 sensors sending readings every second. Congratulations — you have 10,000 *problems*. Each one is screaming data at you, and you need to decide: where does it land? How do you store a billion rows? How do you query them without waiting six hours? And how do you not go bankrupt paying for it?

That's Module 7. Let's go.

---

## Slide 2: Learning Objectives

Six things I want you to be able to do after today.

**One:** Classify data by *temperature* — not Celsius, but hot, warm, and cold. This is the single most important mental model for IoT storage, and once you get it, everything else clicks.

**Two:** Compare storage technologies side by side. Time-series databases, relational, NoSQL, object storage, data lakes. Not by memorizing names, but by understanding *what problem each one solves*.

**Three:** Justify why InfluxDB or TimescaleDB or Apache Parquet is the right choice for a *specific* scenario. The "it depends" answer is only acceptable if you know *what* it depends on.

**Four:** Design a Lambda or Kappa architecture for IoT data pipelines. These are the two dominant architectural patterns in production systems right now.

**Five:** Think about the edge-to-cloud hierarchy. Because the cloud is not always the answer. Sometimes the right answer is a SQLite database on a Raspberry Pi.

And **six** — retention and compression. Because you *cannot* keep raw sensor data forever, and the engineers who don't plan for this learn the hard way, at 3am, when their disk fills up.

Let's start with the fundamentals.

---

## Slide 3: Outline

Here's our roadmap.

We start with the core problem — why is IoT data hard to store? Then we introduce the hot/warm/cold pattern which will frame everything else. Time-series databases are the heart of this module — we'll spend real time here. Then NoSQL, object storage, data lakes. Edge storage and store-and-forward, because the cloud isn't always there. Lambda and Kappa architectures — the big-picture design patterns. Cloud platforms. And finally: compression, retention, and a selection guide.

A lot to cover. Let's move.

---

## Slide 4: The IoT Data Storage Problem

*(Click slide)*

Let me give you a number. Ready?

One factory. 10,000 sensors. Each one sends one reading per second.

That's **864 million readings per day.** From *one factory.*

Scale that to a smart city with 100,000 nodes and you're at 8.6 *billion* readings per day. That is petabytes per year. And your PostgreSQL class? That class where you learned to SELECT * FROM customers? That class did not prepare you for this.

*(pause for effect)*

The reason IoT data is fundamentally different from typical enterprise data comes down to the **four Vs**.

**Volume** — we just talked about it. Numbers that make normal database administrators nervous.

**Velocity** — it's not just a lot of data, it's data arriving *fast*. A vibration sensor on industrial equipment might run at 10 kilohertz. That's 10,000 readings per second, *per channel*. But then you also have a soil moisture sensor that only reports every 30 minutes. Same IoT system, wildly different rates.

**Variety** — some of your data is clean structured floats: temperature 23.5°C. Some is JSON blobs. Some is camera images. Some is audio. Some devices sleep for hours and send nothing, making your data inherently *sparse*.

**Veracity** — and this one people forget. Your data is *dirty*. Sensors have noise. Readings arrive out of order. QoS 1 in MQTT can deliver the same message twice. Nodes go offline and reconnect with a backlog. You need a storage system that handles all of this gracefully.

No single database handles all four Vs optimally. That's not a bug — it's the reason we have an entire ecosystem of storage technologies. And it's why today's lecture exists.

---

## Slide 5: Hot Path / Warm Path / Cold Path

This is the mental model I want you to remember for the rest of your career.

*(point to the diagram)*

When data leaves your broker — Kafka, MQTT — it doesn't go to one place. It goes to *three* places, simultaneously, along three different paths. And each path has different latency, different retention, and different cost.

**Hot path.** This is the path for data that needs to be acted on *right now*. Milliseconds. You're feeding a stream processor — Apache Flink, Kafka Streams — and the output goes into a time-series database. InfluxDB. Why? Because when you need to render a live dashboard or trigger an alert at 50 milliseconds, you need a database that can answer a query in under 10ms. Hot path = fast, expensive, short retention. Hours to days.

**Warm path.** This is for data you'll query over the next few days or months. It goes through a message queue, gets transformed, lands in a relational or NoSQL database. PostgreSQL, MongoDB. Not as fast as hot, but much cheaper. You use this for: billing queries, recent trip analytics, anything where "give me the last 30 days" is a typical query.

**Cold path.** This is your archive. Data flows in batches — Spark, Flink batch mode — into object storage. Amazon S3. Azure Blob. Google Cloud Storage. The latency here is hours. But the *cost* is pennies per gigabyte. And the *retention* is years. This is where you keep data for ML training, regulatory compliance, audit trails.

The key insight — and put this in your notes — is that **most production IoT systems need all three paths.** A smart grid fault detection system? Hot path for the 50ms fault alerts. Warm path for the billing data you query monthly. Cold path for the 10-year regulatory retention requirement.

Design your *entire pipeline* with all three paths in mind. Not as an afterthought.

---

## Slide 6: Hot / Warm / Cold Path Characteristics

Let's make this concrete with the table.

Look at **latency**. Hot path: under 100 milliseconds. Warm path: seconds to minutes. Cold path: hours to days. The moment you need to ask "what happened 6 months ago" — you're in cold path territory, and that's fine, because cold path queries are batch queries. You run them as a job.

**Retention**. Hot: hours to days. Warm: days to months. Cold: years. You design your *data lifecycle* around these. After 7 days, raw data gets downsampled and moved from hot to warm. After 90 days, it gets archived to cold.

**Cost**. This is where it gets interesting. Hot path storage — NVMe SSDs on managed cloud databases — costs roughly $0.10 to $0.25 per gigabyte per month. Cold path — Amazon S3 — costs $0.023 per gigabyte. *Ten times cheaper.* If you keep everything in InfluxDB because "it's easier," you're paying 10x what you need to.

Two real examples that illustrate this perfectly:

**Smart Grid** — hot path catches fault conditions in under 50 milliseconds. That's not optional — if you take 2 seconds to detect a fault, you've already damaged equipment. Warm path handles billing queries for the last 3 months. Cold path archives everything for 10 years because FERC regulations require it.

**Connected Vehicle** — hot path for safety alerts and live telemetry. Warm path for trip analytics and OTA diagnostics. Cold path for crash investigation and training the next ML model for autonomous driving.

---

## Slide 7: Time-Series Databases: Why They Exist

Here's a question I get every semester.

"Professor, why can't we just use PostgreSQL? I know PostgreSQL. PostgreSQL is good."

And the answer is: you *can*. You *can* absolutely use PostgreSQL for IoT data. And at 10,000 rows, it will work great. At 10 million rows, it will start to strain. At 10 *billion* rows... you're going to have a bad time.

Let me explain *why*.

PostgreSQL uses a **B-tree index** by default. B-trees are beautiful data structures for random-access queries. But IoT data is *not* random access — it's sequential. You're always appending new readings in timestamp order. B-trees have to reorganize themselves every time you insert data, and at IoT scale, that overhead is brutal.

PostgreSQL also has **no native compression** for time-series patterns. A sensor reading temperature at 23.5°C, then 23.6°C, then 23.4°C — those values are very similar to each other. You can compress them by storing only the *delta*. General-purpose databases don't do this automatically.

No **built-in retention policies**. Want to auto-delete data older than 30 days? In PostgreSQL, you write a cron job. In InfluxDB, you configure a bucket policy in one line.

Time-series databases solve all of these problems specifically:

**LSM trees or columnar storage** — optimized for sequential appends. New data goes in fast.

**Delta-of-delta with XOR compression** — this is the Gorilla algorithm, invented by Facebook in 2015. You take the difference of differences of timestamps and values, then XOR-compress them. The result? 16 bytes of float plus timestamp compressed to an average of **1.37 bytes**. That's a 12x compression ratio, automatically, with no configuration. InfluxDB does this. Prometheus does it. VictoriaMetrics does it.

**Automatic downsampling**. Raw 1Hz data gets automatically rolled up to 1-minute averages, then 1-hour averages, then 1-day averages.

**Time-aware query syntax.** `GROUP BY time(1h)` is a native construct. Not a hack — a first-class feature.

This is why time-series databases exist. They are specialized tools for a specialized problem.

---

## Slide 8: InfluxDB — The IoT Time-Series Standard

InfluxDB. This is the one to know.

InfluxData built it in 2013, and by 2026 it's the number one time-series database on DB-Engines. Used by Tesla for vehicle telemetry, NASA for mission data, Cisco, Siemens. If you go work in IoT infrastructure — and statistically, some of you will — you will encounter InfluxDB.

The data model is elegant. You have a **measurement** — think of it like a table. For example, `temperature`. You have **tags** — indexed metadata. Device ID, location, sensor type. These are what you GROUP BY. You have **fields** — the actual sensor values. Float, integer, string. And a **timestamp** with nanosecond precision.

The query language has evolved over three versions. InfluxDB 1.x used InfluxQL — basically SQL for time-series. InfluxDB 2.x introduced Flux — a functional data pipeline language. It's powerful but... let's say it has a learning curve. And InfluxDB 3.x went back to native SQL, because it turns out engineers really like SQL.

**InfluxDB 3.x** — released in 2024 — is a significant architectural change. The entire engine was rewritten in *Rust*. The storage backend is Apache **Parquet on S3-compatible object storage**. This means your InfluxDB data is now sitting in open Parquet files on S3, queryable by DuckDB, Spark, or Trino directly. That's huge. It breaks the proprietary lock-in. Unlimited tag cardinality — a previous limitation that killed some use cases.

If you're starting a new IoT project today, InfluxDB 3.x is my first recommendation for time-series storage.

---

## Slide 9: TimescaleDB — Time-Series on PostgreSQL

Now, here's the scenario where InfluxDB is *not* the right answer.

Your IoT system collects sensor readings. But you *also* need to JOIN those sensor readings with an asset registry — a table of device metadata, maintenance history, installation location. That's a relational query. InfluxDB cannot do JOINs. Full stop.

Enter **TimescaleDB**.

It's a PostgreSQL *extension*. You install it on top of a regular PostgreSQL instance. From your application's perspective, it looks like PostgreSQL — you connect with the same drivers, you write the same SQL, you use the same ORMs. Under the hood, TimescaleDB partitions your time-series tables into what it calls **hypertables** — automatically chunked by time interval.

The example on the slide is something you literally cannot do in InfluxDB:

```sql
SELECT avg(temp), asset.location
FROM readings JOIN assets
WHERE time > now() - INTERVAL '1h'
```

That JOIN between a time-series table and a relational table is native TimescaleDB. Sub-second. With proper indexing.

**Compression** is 90 to 95 percent ratio — columnar storage for time-series chunks. **Continuous aggregates** pre-compute rollups on new data as it arrives, so your 1-hour average is always up to date without running a manual job.

Choose TimescaleDB when: you need time-series performance *and* relational capability, your team knows PostgreSQL and doesn't want to learn a new database, or your compliance team says "ACID transactions or we're not approving this."

---

## Slide 10: Other Time-Series Databases (2026)

Let me give you a quick tour of the landscape.

**Prometheus** — pull-based, in-memory storage, designed for infrastructure monitoring. If you want to monitor *your IoT platform* — CPU on the broker servers, memory usage, Kafka lag — you use Prometheus plus Grafana. It is *not* designed for millions of sensors. But for ops monitoring, it's the standard.

**VictoriaMetrics** — think of it as a drop-in InfluxDB replacement that's more cost-effective at scale. InfluxDB-compatible API. Better compression. Open source. If you have a huge deployment and InfluxDB's cost is becoming a problem, VictoriaMetrics is where people migrate.

**QuestDB** — this is the interesting one. Written in Java but uses SIMD instructions for columnar data processing. Benchmark numbers that look fake but are real — millions of rows per second ingestion on a single node. Great choice for edge servers where you have a beefy machine and need maximum throughput.

**TDengine** — Chinese company, purpose-built for Industrial IoT. One of the best choices for smart factory and SCADA applications. Built-in compression specifically tuned for industrial sensor patterns.

The selection rule, simplified: Prometheus for monitoring your stack. InfluxDB for cloud IoT. TimescaleDB for hybrid SQL workloads. QuestDB for maximum ingestion speed at the edge.

---

## Slide 11: NoSQL Storage for IoT

Time-series databases handle telemetry beautifully. But IoT systems have *other* data too.

**Document databases** — MongoDB, Couchbase. Your IoT system has device metadata. Serial numbers, firmware versions, installation dates, configuration parameters. That data has a flexible schema — different device types have different attributes. This is exactly what document databases are designed for. JSON documents, flexible schema, rich query language.

MongoDB also has **Atlas Device Sync** — an offline-first sync system for IoT. Your edge device stores data in a local MongoDB Realm database, and it syncs to Atlas in the cloud when connectivity is available. This is elegant for applications like field service tablets.

**Key-value databases** — Redis, DynamoDB. The pattern to know here is the **last-known-value cache**. In a real IoT dashboard, most of the time you don't want a time-series query — you want to know *right now*, what is the current temperature of device 1234? With Redis, you store that as a single key: `device:1234:temp = 23.5`. Every incoming MQTT message updates it. Dashboard reads in under one millisecond. No time-series query needed.

**Wide-column databases** — Apache Cassandra, ScyllaDB. These are the hyperscale options. Partition by device ID, cluster by timestamp. You get millions of writes per second, linear horizontal scaling, and multi-datacenter replication. The tradeoff: no complex aggregations. But if you're building a platform that needs to handle data from 50 million devices, Cassandra is how Netflix and Uber do it.

---

## Slide 12: Object Storage — The Cold Path Foundation

Let me talk about where your data lives when it grows up.

Amazon S3 — or Azure Blob, or Google Cloud Storage — is the backbone of IoT cold storage. Let me give you a number that will change how you think about this.

S3 Standard: **$0.023 per gigabyte per month**.

Amazon DynamoDB managed storage: **$0.25 per gigabyte per month**.

That's an **11x cost difference.** At petabyte scale, that is *millions of dollars a year.* This is why every serious IoT system eventually moves its cold data to object storage.

And the durability is remarkable — 99.999999999% — eleven nines. Amazon has architected S3 to be more durable than any database you could run yourself.

Now, the format you store matters enormously.

If you dump raw JSON to S3 — which a lot of people do because it's easy — you pay the full storage cost, and every query has to deserialize every file. Slow and expensive.

**Apache Parquet** is the answer. Columnar format, developed by Twitter and Cloudera. On typical sensor data, Parquet is 87% smaller than CSV. Parquet is natively understood by Spark, DuckDB, Trino, Athena — every major analytics engine. And InfluxDB 3.x stores *natively* in Parquet. DuckDB can query Parquet files sitting in S3 directly — no data movement, no ETL.

The partitioning strategy matters too. You organize your S3 files as: `year=2026/month=02/day=23/device_type=temperature/`. This lets query engines *prune* partitions — if I'm querying February data, the engine only opens February files. Not all 4 years of data.

---

## Slide 13: Data Lakes and Lakehouses for IoT

Quick but important distinction.

A **data lake** is what happens when you start dumping raw Parquet files into S3 without discipline. It works great for the first few months. Then you have a firmware bug and need to delete 3 million bad readings. How do you delete rows from Parquet? You can't — Parquet is immutable. You have to rewrite the entire file. Painful.

Then your schema changes — you add a new sensor field. Now some files have the old schema and some have the new schema. Your query engine starts returning null values in unexpected places. Welcome to the **data swamp**.

A **lakehouse** solves this. The idea: add a metadata layer on top of your S3 Parquet files that provides ACID transactions, schema enforcement, and time-travel.

**Apache Iceberg** is the one to know. Adopted by AWS, Apple, Netflix, LinkedIn. Iceberg maintains a table of metadata about your Parquet files — it knows which files contain which rows, which schema version they use, what timestamp range they cover.

Why does this matter for IoT specifically?

**Late-arriving data.** An edge device comes back online after a week offline and replays 7 days of readings. With raw Parquet you have to awkwardly insert them out of order. With Iceberg, it's an atomic upsert.

**Deleting bad readings.** Firmware bug corrupted your temperature values for 3 days. With Iceberg, you issue a DELETE statement. Done. The old files aren't modified — Iceberg writes new metadata pointing to a new snapshot. The old data is still there for auditing. This is **time travel** — you can query what your data looked like at any point in the past.

The 2026 landscape: AWS Glue + Iceberg is the dominant pattern on AWS. Azure uses Delta Lake. Both are supported by Spark, DuckDB, and Trino.

---

## Slide 14: Edge Storage — Why Store Locally?

I want to challenge an assumption.

The assumption is: "data should go to the cloud as fast as possible." And it feels natural — the cloud has infinite storage, managed services, dashboards, ML pipelines. Why *not* send everything immediately?

Here's why not.

**Connectivity.** A wind turbine on a ridge in Wyoming might have 4G coverage 80% of the time. The other 20%? No signal. If you have no local storage, you lose 20% of your data.

**Latency.** Closed-loop control systems — industrial robots, HVAC control, safety shutoffs — need to make decisions in *milliseconds*. A round-trip to the cloud is tens to hundreds of milliseconds. You cannot close a safety valve over the internet.

**Cost.** AWS egress is $0.09 per gigabyte. A single 10kHz vibration sensor generates about 80 kilobytes per second. Run the math: 6 gigabytes per hour, per channel. If you have 100 channels, you're looking at enormous egress bills. Versus processing locally and uploading only 1-minute summaries.

**Privacy.** GDPR and HIPAA in many cases require that personal data be processed locally before it leaves the building. This is not optional.

The edge storage technologies are now mature:

**SQLite** — embedded, zero configuration, ACID compliant. It runs on everything from Arduino Mega to industrial PLCs. There are *more* SQLite deployments than any other database in the world, by a huge margin.

**InfluxDB IOx** — the new edge-deployable version of InfluxDB. Same API as the cloud version. You write to it at the edge, replicate to the cloud. Seamless.

**QuestDB** — columnar at the edge server level. If you have a gateway with 8 cores and want serious ingestion throughput locally.

The design pattern: **store locally, process locally, upload summaries to cloud.** Raw data stays at the edge. Only anomalies, aggregates, and events go to the cloud. This is the architecture that scales.

---

## Slide 15: Store-and-Forward and Sync Patterns

Related question: what happens when the network actually goes down?

This is the **store-and-forward** pattern, and it's one of the most important reliability mechanisms in IoT systems.

The idea is simple: your edge device buffers data to local disk while the network is unavailable. When connectivity restores, it replays the buffered data to the cloud, *in order*, with original timestamps.

MQTT QoS 1 and 2 give you some of this automatically through persistent sessions. But broker buffers are typically in-memory, which means a broker restart loses them. You need *disk persistence* for serious deployments.

EMQX — which we covered in Module 6 — can use **RocksDB** as its on-disk message queue. Your edge broker buffers messages to disk, survives a restart, and replays them when the uplink comes back. This is what you want.

Now, *how* you sync matters. Four patterns:

**Full sync** — upload everything. Simple, but expensive. Fine for small deployments.

**Delta sync** — only upload what changed since the last successful sync. You track a high-water mark. More complex, but bandwidth-efficient.

**Summary sync** — instead of raw readings, upload aggregates. 1-minute averages instead of 60 individual readings. 60x bandwidth reduction. This is lossy, so you need to decide: is that acceptable for your use case?

**Event-driven sync** — only upload when something interesting happens. Normal operation: local only. Anomaly detected, threshold breached, alarm triggered: upload immediately, and include context window. Elegant for battery-constrained devices.

AWS IoT Greengrass implements all of these patterns out of the box. It's an edge runtime — it runs on your gateway, buffers MQTT messages, executes Lambda functions *offline*, and syncs to AWS IoT Core when connectivity restores.

---

## Slide 16: Lambda Architecture for IoT

We're now at the architectural level. How do you design a complete IoT data pipeline?

The **Lambda Architecture** was described by Nathan Marz in 2011, and it's still the dominant pattern for large-scale IoT data pipelines.

Look at the diagram. You have three layers.

**Batch layer** — takes *all* of your historical data, reprocesses it, and produces pre-computed views. This runs periodically — hourly, daily. The output is accurate because it uses all the data. But it's *slow* — if you're reprocessing 3 years of sensor data with Apache Spark, that job might take hours.

**Speed layer** — processes only *recent* data, in real time. Apache Flink, Kafka Streams. It produces results immediately, but only for recent data. And because it doesn't have access to full history, its results are sometimes approximate.

**Serving layer** — merges the batch view (complete, accurate, historical) with the speed view (recent, fast, approximate) and answers queries. When a user asks "show me the last 48 hours of temperature for device 1234," the serving layer stitches together the batch view for data older than a few hours, and the speed view for the most recent data.

The reason Lambda dominated for so long: it gives you the best of both worlds. Accuracy from the batch layer, speed from the real-time layer.

The downside — and it's significant — is **two codepaths**. You have to maintain Spark batch jobs *and* Flink streaming jobs that do the same computation. Every time you change your business logic, you change it in two places. That's expensive to maintain.

Which brings us to...

---

## Slide 17: Kappa Architecture

In 2014, Jay Kreps — co-creator of Apache Kafka, then at LinkedIn — wrote a blog post that was essentially: "Lambda Architecture is too complicated. Let's fix it."

The **Kappa Architecture** is the simplification: *remove the batch layer entirely.*

How? By making Kafka your single source of truth. Kafka is a *log* — an immutable, ordered, replayable sequence of messages. If you want to reprocess historical data, you don't re-run a Spark batch job — you *replay the Kafka topic from offset zero*. Apache Flink processes it just like streaming data, just faster.

One codebase. One streaming pipeline. Real-time queries *and* historical reprocessing from the same code.

Compare the table on the right:

Lambda: high complexity, full recompute for history, supports low and high latency, well-established.

Kappa: lower complexity, Kafka replay for history, low latency only, growing fast.

Which to choose? In 2026: if you're building from scratch and your team is comfortable with Kafka and Flink — go Kappa. If you have legacy systems or regulatory requirements for periodic batch jobs — Lambda is still perfectly valid.

The 2026 trend: Confluent and AWS are both pushing hard on Kappa via Kafka + Flink + Apache Iceberg. Databricks calls their version the "Lakehouse." Same idea.

---

## Slide 18: Cloud IoT Platforms in 2026

Quick survey of the cloud landscape.

**AWS IoT Core** is the dominant platform. MQTT, HTTP, AMQP support. Rules Engine routes data to 20+ AWS services. Kinesis for streaming, DynamoDB for device state, S3 for cold storage, Timestream for time-series. Massive ecosystem. High vendor lock-in. Per-message pricing.

**Azure IoT Hub** — strong in enterprise, especially anything connected to Microsoft 365, Active Directory, or Dynamics. Azure Data Explorer is genuinely excellent for time-series analytics. Tight integration with Azure Stream Analytics.

**Google Cloud IoT Core** — and here's the cautionary tale of the day.

*(pause)*

Google **shut down** IoT Core in August 2023. Announced it, gave customers 18 months. Customers who had built production systems on Google IoT Core had to migrate their entire IoT backends on a deadline. Some migrated to AWS. Some to Azure. Many went to self-hosted EMQX or HiveMQ and just moved to GCP Pub/Sub for the message bus.

This is a real risk. Vendor lock-in in IoT is *more* dangerous than in most domains because your hardware is already deployed in the field. You can't just swap the edge firmware when the cloud platform disappears.

The self-hosted option — EMQX, HiveMQ, Mosquitto on your own Kubernetes cluster — has zero lock-in. You control the stack. The tradeoff is operational burden. But increasingly, that's the choice organizations are making.

---

## Slide 19: AWS IoT Data Stack (2026)

Since AWS is the dominant choice, let's walk through the full stack.

**Ingestion:** IoT Core receives MQTT messages from your devices. The **Rules Engine** — this is where the magic happens — lets you write SQL-like rules that route messages. "IF temperature > 80 THEN send to Kinesis." "IF device_type = 'critical' THEN also write to DynamoDB." No code required.

**Processing:** Kinesis Data Analytics runs Apache Flink jobs on the incoming stream. You write SQL or Java code. AWS Lambda for serverless event-driven processing — great for triggering actions on individual events. AWS Glue for ETL jobs that move data to the cold path.

**Storage:** And here's the full picture.

Amazon **Timestream** for time-series — auto-tiering between in-memory storage for recent data and S3 for historical data. InfluxDB-compatible write API so you can switch without changing client code. About 1/10th the cost of using DynamoDB for the same time-series workload.

**DynamoDB** for device state and last-known-value — single-digit millisecond reads, massive scale.

**S3 + Iceberg** for the cold path and data lake.

**OpenSearch** for log analytics and full-text search — if you need to search through device error messages.

If you're building a greenfield IoT system and you're an AWS shop, this is the reference architecture.

---

## Slide 20: Data Compression for IoT

I said at the beginning that compression is not optional. Let me back that up with a number.

Storing raw JSON to S3 versus Parquet with Zstandard compression: **10 to 50 times the cost.** At petabyte scale, that's millions of dollars per year. This is not academic.

Let's talk about the compression algorithms.

**Gorilla** — the algorithm Meta/Facebook designed specifically for time-series data. Delta-of-delta encoding for timestamps — the differences between differences are tiny numbers that compress beautifully. XOR for values — consecutive similar sensor readings XOR to almost all zeros. Result: 1.37 bytes per timestamp-value pair on average. Magical. Used in InfluxDB and Prometheus.

**LZ4** — general-purpose but *fast*. 400 megabytes per second decompression. If you need fast reads more than the best ratio, LZ4 is your tool.

**Snappy** — Google's compression library. Moderate ratio, very fast. Used inside Parquet files and Apache Cassandra. The default for Parquet in Spark.

**Zstandard (Zstd)** — the modern best-of-both-worlds. Excellent compression ratio *and* fast decompression. Meta open-sourced it in 2016. InfluxDB 3.x uses it. Kafka uses it. If you're choosing a compression algorithm today for IoT data, Zstd is usually the answer.

Practical order of magnitude: raw sensor data versus Gorilla-compressed time-series? 10x smaller. Versus Parquet+Zstd for cold storage? 50x smaller compared to JSON. These numbers are not theoretical — they're what you'll see in practice.

---

## Slide 21: Data Retention and Downsampling Policies

You cannot keep raw sensor data forever. That's not pessimism — it's arithmetic.

1 Hz sensor, one reading per second. 86,400 readings per sensor per day. 1,000 sensors: 86 million readings per day. After one year: 31 *billion* rows. Where does that live? How much does that cost?

The answer is the **downsampling pyramid**.

Raw 1Hz data: keep for **7 days**. After that, it's too granular for most queries anyway.

1-minute averages: keep for **90 days**. Most operational queries — "show me last month's performance" — work fine at 1-minute resolution.

1-hour averages: keep for **2 years**. Monthly reports, trend analysis.

1-day averages: keep **forever**. The long-term view. Almost no storage cost.

InfluxDB implements this with continuous queries — you configure them once and they run automatically:

```
SELECT mean(temp) INTO "1h_avg"."temp"
FROM raw GROUP BY time(1h)
```

The cost impact: going from raw 1Hz to 1-hour averages is a **3,600x storage reduction**. That's not a typo. 86,400 points per day becomes 24 points per day.

But here's where engineering judgment comes in. *Not all data can be downsampled.*

If you need ML training data — your anomaly detection model needs to see the raw waveform of the fault event, not a 1-minute average. You need to keep raw data for *at least 2 years* for a useful ML dataset.

If you're billing for electricity — a smart meter dispute resolution requires you to prove the exact kWh consumed at a specific minute. You need raw data.

Define your retention policy **before you deploy**. Retrofitting a retention policy to a system with a billion existing rows is painful in ways I cannot adequately describe in one lecture.

---

## Slide 22: IoT Storage Selection Guide

Same methodology as protocol selection. Start with constraints, eliminate, decide.

Let me walk through the table.

**High-frequency telemetry, real-time queries** — InfluxDB 3.x. No contest. Purpose-built, Gorilla compression, SQL in v3, massive adoption.

**Time-series plus relational JOINs** — TimescaleDB. The moment you need to JOIN sensor data with anything relational, you need TimescaleDB or you need to denormalize into Cassandra.

**Device state, last-known-value** — Redis. Sub-millisecond reads. Your dashboard showing the current status of 10,000 devices? Redis. Every incoming MQTT message updates the key.

**Flexible schema, device metadata** — MongoDB. Heterogeneous device types with different attributes. Document model is natural here.

**Cold storage, ML training data** — S3 plus Parquet plus Iceberg. 11 nines durability, 50x compression, queryable in place with DuckDB.

**Edge, intermittent connectivity** — SQLite or QuestDB. Embedded, zero configuration, no network needed.

**Multi-tenant hyperscale** — Cassandra or ScyllaDB. You're building a platform serving millions of devices for multiple customers. Write throughput in the millions per second.

The punchline: **production IoT systems use 3 to 4 of these simultaneously.** There is no silver bullet. An industrial IoT platform typically has InfluxDB for hot telemetry, Redis for device state, S3+Iceberg for cold storage, and MongoDB for device metadata. They serve different roles.

---

## Slide 23: State of the Art — IoT Data Storage in 2026

Let me give you a snapshot of where the industry is right now.

**Established and production-ready:**

InfluxDB 3.x with Parquet storage is the dominant TSDB. TimescaleDB continuous aggregates — the ability to have pre-computed rollups that update automatically — is genuinely fantastic and underused. Apache Kafka as an immutable log and the foundation of Kappa architectures is standard at scale. S3 plus Parquet is universal. AWS Timestream is mature and cost-effective for AWS shops.

**Emerging and gaining fast traction:**

**Apache Iceberg** is probably the most important trend. Raw data lakes are being replaced by Iceberg-managed lakehouses everywhere. Every major cloud provider now has native Iceberg support. This is not hype — it's a genuine shift in how cold storage is managed.

**DuckDB** — this one surprises people. It's an in-process analytical database that runs *inside your Python script or Go program* and can query Parquet files on S3 directly. No cluster needed. No ETL. You write `SELECT * FROM 's3://my-bucket/data/*.parquet'` and it just works. For ad-hoc IoT data analysis, it's a game-changer.

**RisingWave** — streaming SQL with materialized views. The materialized view updates in real time as new data arrives. Your dashboard query is always pre-computed.

**The big trend** is **storage-compute separation**. In 2020, you bought a database and it stored your data internally in its own proprietary format. In 2026, you store your data in open formats — Parquet on S3 — and you attach *any* query engine you want. DuckDB today, Spark tomorrow, Trino next week. No migration. No vendor lock-in. This is a fundamental architectural shift.

---

## Slide 24: Key Takeaways

Let me land this plane.

**Fundamentals you must know:**

The 4 Vs — Volume, Velocity, Variety, Veracity — are why IoT data is hard to store. Hot/warm/cold path is the mental model you apply to *every* IoT system you design. Time-series databases exist because general-purpose databases don't scale for sequential append-heavy workloads. Gorilla compression gives you 10x space savings automatically.

**Technologies:**

InfluxDB 3.x for cloud IoT time-series. TimescaleDB when you need SQL JOINs. Redis for device state. S3 plus Parquet plus Iceberg for cold path and ML.

**Architecture patterns:**

Lambda and Kappa are two solutions to the same problem. Lambda is mature and handles complex requirements. Kappa is simpler and is gaining ground with Kafka adoption. Edge-first and store-and-forward are not optional for serious deployments — they're the difference between a reliable system and one that loses data every time the network hiccups.

**The meta-lesson:**

Define your retention policy, your compression strategy, and your hot/warm/cold split *before* you write the first line of code. These decisions are hard to change once you have data. A billion-row database with no retention policy is a ticking clock.

You will design an IoT system. You will receive data. At some point, someone will ask you: where is all this data? How much does it cost? Can we query last year? That is the moment this module saves you.

---

## Slide 25: Questions?

Alright — that's the full picture of IoT data storage.

We went from the fundamental problem — 864 million readings per day from one factory — through the entire stack: hot/warm/cold paths, time-series databases, NoSQL, object storage, lakehouses, edge storage, Lambda and Kappa architectures, cloud platforms, compression, and retention.

The common thread through all of it: **there is no single answer.** Every production IoT system is a careful assembly of specialized tools, each optimized for its specific role.

Your job as an engineer is to know those tools, understand their tradeoffs, and match the constraint to the solution.

Any questions before we wrap up?

*(pause — take questions)*

Next module we'll tie this together with end-to-end IoT system design — bringing sensors, protocols, processing, and storage into a complete architecture. That's where everything we've covered becomes a coherent system.

See you then.
