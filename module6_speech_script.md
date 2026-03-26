# Module 6: Data Streaming Protocols for IoT — Oral Script

> **Style:** Engaging, entertaining, rigorous engineering professor. Conversational but precise. Think: the professor who makes you laugh but also makes you *learn*.

---

## Slide 1: Title Slide

Alright everyone, welcome back. Today we're tackling Module 6: Data Streaming Protocols.

So far in this course, we've learned how to sense the physical world, how to digitize it, how to process it locally on a microcontroller, and how to shove it through a wireless radio. *Congratulations* — your data has now traveled about three feet from the sensor to the radio.

*(pause)*

Now what? How does that 20-byte soil moisture reading actually make it to a cloud dashboard in your browser, or trigger an alert on your phone? That's what today is about. The **application layer** — the last mile of the IoT pipeline.

---

## Slide 2: Learning Objectives

Five things I want you to walk out of here knowing.

**One:** Why we even need specialized streaming protocols. Can't we just use HTTP for everything? Spoiler: you *can*, but you probably *shouldn't*.

**Two:** You should be able to compare MQTT, CoAP, HTTP, AMQP, and XMPP side by side — not by memorizing a table, but by understanding *why* each one exists and what problem it was designed to solve.

**Three:** You'll be able to design a publish/subscribe architecture using MQTT 5.0 — the current version of the most important IoT protocol in the world right now.

**Four:** Protocol selection. Same method we used for wireless protocols — think in constraints, eliminate, decide.

And **five** — and this is where it gets interesting — we'll look at what's *new*. MQTT over QUIC, Sparkplug B, the Unified Namespace. This is the stuff that's happening *right now* in industry, and if you understand it, you're ahead of 90% of engineers who graduated two years ago.

Let's go.

---

## Slide 3: Outline

Here's the roadmap. We start with the fundamental question: *why* do we need data streaming protocols at all? Then we'll spend the most time on MQTT, because frankly, it's the king of IoT data streaming. We'll cover HTTP and CoAP as alternatives, briefly touch XMPP and AMQP — because you should know they exist — and then look at the emerging stuff: WebSocket, gRPC, edge architectures. We'll finish with a protocol selection guide.

Let's dive in.

---

## Slide 4: The Application-Layer Problem

*(Click slide)*

OK, so here's the setup. You've got data sitting on a sensor node at the edge of a vineyard. You need to get it to a cloud server. Simple, right?

Well, let's think about what tools we already have. FTP? That's designed to transfer *files*. Your soil reading is 20 bytes — you don't need a file transfer protocol for that. That's like renting a moving truck to deliver a letter.

Email? SMTP? I mean... technically you *could* email your sensor data. But the overhead of an email header is probably 10 times the size of your actual data. That's *insane*.

Raw TCP sockets? Sure, you can open a socket and dump bytes. But then you have no structure, no standard, no way for someone else to interoperate with your system. You've just reinvented the wheel, badly.

What IoT *actually* needs is this: small messages, sent frequently, with low overhead, some form of reliability guarantee, security, and the ability to scale to thousands of devices.

And that gives us two fundamental design patterns. Look at the diagram.

**Request/Response** — the client asks, the server answers. This is HTTP, this is CoAP. It's simple, it's well-understood, but it has a problem: the server can't *push* data to the client unless the client asks first.

**Publish/Subscribe** — the publisher sends data to a topic, and a broker routes it to anyone who's subscribed. The publisher doesn't even know who's listening. This is MQTT, this is AMQP. And this decoupling is *incredibly* powerful for IoT.

Keep this distinction in your head. It's going to drive every protocol choice we make today.

---

## Slide 5: MQTT: Message Queuing Telemetry Transport

Alright, let's talk about the big one. MQTT.

Great origin story here. 1999 — Andy Stanford-Clark at IBM and Arlen Nipper had a problem. They needed to get sensor data from *oil pipelines in the middle of nowhere* back to headquarters via a *satellite link*. Satellite bandwidth is expensive. Latency is brutal. They needed something tiny, something efficient, something that could survive crappy networks.

So they invented MQTT. And here's the beautiful irony: a protocol designed for oil pipelines in 1999 is now the backbone of the modern Internet of Things.

The name, by the way — "Message Queuing Telemetry Transport" — is a bit of a misnomer. The "MQ" came from IBM's MQ Series product line. MQTT doesn't actually *require* message queuing. Don't let the name confuse you.

Properties: pub/sub over TCP/IP. Minimum header of just 2 bytes — two! Compare that to HTTP where your headers alone can be 500 bytes. Code footprint in the kilobytes, meaning it runs on the tiniest microcontrollers.

And look at who uses it today. AWS IoT Core, Azure, Google Cloud — all three major clouds speak MQTT natively. Facebook Messenger handles *billions* of messages per day using MQTT. Tesla, BMW for connected vehicles. Siemens, Bosch for industrial IoT. The Things Network for LoRaWAN.

This is not a niche protocol. This is the **de facto standard** for device-to-cloud telemetry. If you learn one data streaming protocol from this course, make it this one.

---

## Slide 6: MQTT Publish/Subscribe Model

Let me walk you through how MQTT actually works.

Three actors. The **publisher** — that's your sensor node. It produces data and sends it to a **topic**. Think of a topic like a mailbox with a label. The **broker** — that's a server that receives messages and routes them. And the **subscribers** — those are clients that have said "hey, I want to receive messages on this topic."

Here's the critical thing: **the publisher and subscriber never talk to each other directly.** The publisher doesn't know — or care — who's listening. The subscriber doesn't know — or care — who's sending. The broker handles all the routing. This is *decoupling*, and it's enormously powerful.

Topics are hierarchical, like file paths. `vineyard/zone1/soil/moisture`. `factory/line3/pump7/vibration`. You can see the structure immediately.

And here's where it gets clever: **wildcards**. The plus sign matches one level: `vineyard/+/soil/#` matches zone1, zone2, zone3, whatever. The hash sign matches everything below: `vineyard/#` gives you *every* message from the entire vineyard. 

So imagine you add a new sensor type — say, a wind speed sensor. You publish to `vineyard/zone1/weather/wind`. The existing subscribers don't need to change *anything*. The new data just flows. Want to add a new analytics dashboard? Subscribe to `vineyard/#`. Done. Zero changes to any device in the field. *That's* why pub/sub wins for IoT.

---

## Slide 7: MQTT Protocol: Session Lifecycle

Now let's get into the mechanics. An MQTT session has four stages.

**Stage one: Connection.** The client opens a TCP connection to the broker — port 1883 for unencrypted, 8883 for TLS. It sends a CONNECT packet with its client ID, optionally a username and password, and a keep-alive interval.

Now here's a really clever feature: the **Will message**. When a client connects, it can say: "If I disconnect unexpectedly — like my battery dies or I lose network — please publish *this* message to *this* topic on my behalf." It's literally a last will and testament. So other subscribers immediately know that device went offline. Beautiful mechanism for device state management.

**Stage two: Authentication.** Without TLS, username and password travel in *clear text*. Let me be very direct about this: **never do this in production.** It's fine for your lab bench, it's unacceptable for a deployed system. With TLS you get proper certificate-based authentication.

**Stage three: Communication.** PUBLISH, SUBSCRIBE, UNSUBSCRIBE — the actual data flow. And PINGREQ/PINGRESP to keep the TCP connection alive during idle periods. If the broker doesn't hear a ping within the keep-alive window, it assumes the client is dead and triggers the Will message.

**Stage four: Termination.** A clean DISCONNECT packet. The broker knows the client left intentionally, so it does *not* publish the Will message. See the distinction? Unexpected death triggers the Will. Graceful goodbye does not.

---

## Slide 8: MQTT Quality of Service (QoS)

This is one of the most important design decisions you'll make when using MQTT. Three QoS levels.

**QoS 0: "Fire and forget."** The publisher sends the message once. The broker doesn't acknowledge it. If it gets lost — *shrug* — it's gone. This sounds reckless, but think about it: if your soil sensor sends a reading every 30 minutes, does it *really* matter if you lose one reading out of 48 in a day? Probably not. And the overhead is minimal — just a single PUBLISH packet.

**QoS 1: "At least once."** Now you get an acknowledgment. PUBLISH, PUBACK. If the publisher doesn't get the PUBACK, it retransmits. The downside? You might get *duplicates*. If the PUBACK gets lost, the publisher retransmits a message that the broker already delivered. For most sensor data, this is fine — you can deduplicate on timestamps.

**QoS 2: "Exactly once."** This is the heavy machinery. Four packets: PUBLISH, PUBREC, PUBREL, PUBCOMP. A full two-phase handshake. No duplicates, no loss, guaranteed. When do you need this? Billing events. Actuator commands — you do NOT want to send "open valve" twice. Financial transactions.

Look at the cost: QoS 2 is **four times** the messages of QoS 0. On a battery-powered LoRaWAN device where every transmitted byte costs energy, that's a real cost. So the engineering question is: does the *cost* of the reliability mechanism exceed the *cost* of occasionally losing a message? For soil moisture: QoS 1. For a command to open a dam gate: QoS 2. Context matters.

---

## Slide 9: MQTT Challenges

Now, MQTT is great, but let's be honest engineers and talk about its weaknesses. Because if someone tells you a technology has no downsides, they're selling you something.

**Scalability.** The topic tree can get enormous in a large deployment. And there's no built-in federation — meaning if you need multiple brokers in different regions, you have to set up bridging yourself. Partitioning the topic namespace across broker clusters is... not trivial.

**Interoperability.** Here's a sneaky one. MQTT says "the payload is binary." OK, great. But *what format*? JSON? Protobuf? MessagePack? CBOR? MQTT doesn't care. Which means Device A from Manufacturer X sends JSON, Device B from Manufacturer Y sends Protobuf, and your application has to figure out what it's looking at. This is a *real* problem in multi-vendor deployments.

**Security.** Built-in authentication is just username and password. That's it. For real security, you need TLS. But TLS requires a TCP handshake, then a TLS handshake — that's overhead. And on a constrained MCU with 64 kB of RAM, fitting a TLS stack is a challenge.

**Transport.** TCP is reliable but heavy. Connection overhead, head-of-line blocking — one lost packet stalls everything behind it. And reconnection after a network change is slow.

The good news? MQTT 5.0 addresses several of these, and MQTT over QUIC attacks the transport problem. Let's look at those.

---

## Slide 10: MQTT 5.0 — Key New Features

MQTT 5.0 came out in 2019 and it's a significant upgrade. Seven major features. Let me hit the highlights.

**Reason Codes.** In MQTT 3.1.1, if your connection failed, you got... nothing useful. "Connection refused." Thanks, very helpful. MQTT 5.0: every acknowledgment includes a specific reason code. "Bad username or password." "Quota exceeded." "Topic name invalid." Now you can actually *debug* on a constrained device.

**Session Expiry.** Previously, sessions either died immediately on disconnect or lived forever. Now the client can say "keep my session for 3600 seconds after I disconnect." If I reconnect within that window, I get my queued messages. If not, clean it up. This is huge for battery-powered devices that go to sleep.

**Topic Aliases.** You're publishing to `factory/building3/floor2/line7/pump4/vibration/x-axis` every second. That's 55 bytes of topic name, every single message. With topic aliases, you assign it a number — say, alias 42 — and after that, every message just says "42." Massive bandwidth savings on repeated publishes.

**Shared Subscriptions.** This is the scalability answer. Say you have a million devices publishing to a topic. In MQTT 3.1.1, one subscriber has to process everything. In 5.0, multiple subscribers can share a subscription, and the broker load-balances across them. Horizontal scaling, finally.

**Request/Response.** MQTT was always fire-and-forget or pub/sub. But sometimes you need RPC — send a command, get a response. MQTT 5.0 adds a Response Topic field in the PUBLISH header, enabling clean request/response patterns over the pub/sub infrastructure.

All major brokers support this now — EMQX, Mosquitto 2.x, HiveMQ, AWS IoT Core. If you're starting a new project in 2026, there's no reason to use 3.1.1.

---

## Slide 11: MQTT Sparkplug B — Solving the Payload Problem

OK, so MQTT tells you *how* to deliver a message. But it says *nothing* about what's inside. You open the payload and it's just... bytes. Could be anything.

Sparkplug B fixes this. It's an OASIS standard, stewarded by the Eclipse Foundation, and it's become the de facto standard for industrial IoT.

Three key things Sparkplug adds:

**First, a standardized topic namespace.** Instead of everyone inventing their own topic hierarchy, Sparkplug defines it: `spBv1.0/group_name/message_type/edge_node/device`. Everyone follows the same structure. You walk into a new factory, you already know how to navigate the data.

**Second, a standardized payload format.** Protobuf-encoded. Every message has a consistent schema with timestamps, datatypes, and metric names. No more guessing if a payload is JSON or CSV or binary.

**Third — and this is the killer feature — Birth and Death certificates.** When a device comes online, it publishes an NBIRTH message containing its complete metric list. "Hi, I'm pump 7, I have these 15 metrics, here are their current values." When it dies unexpectedly, the broker publishes an NDEATH — using the MQTT Will message mechanism we talked about. So the system *always* knows the state of every device. No more "is it offline or just slow?"

And this feeds into the **Unified Namespace** — UNS — which is *the* architecture buzzword in Industry 4.0 right now. The idea: one MQTT broker, one topic tree, every system in the factory publishes and subscribes to the same namespace. No more point-to-point integrations, no more OPC-UA bridges talking to Modbus gateways talking to proprietary APIs. One source of truth. Siemens, Bosch, BMW — they're all moving this direction.

---

## Slide 12: MQTT over QUIC — Beyond TCP

Here's where it gets really cutting-edge.

MQTT has always run on TCP. TCP is great for reliability, but it has some fundamental problems for IoT.

Imagine a connected vehicle — a Tesla driving on the highway. It's connected to a cell tower. It enters a tunnel. Connection drops. Comes out the other side — new cell tower, new IP address. With TCP, you have to: detect the disconnection, tear down the old connection, do a new TCP handshake, then a new TLS handshake, then reconnect the MQTT session. That's *seconds* of downtime. For a car that might be sending safety-critical telemetry, that's unacceptable.

Enter QUIC. QUIC was developed by Google, standardized by the IETF, and it's the transport protocol underneath HTTP/3. And it has properties that are almost *tailor-made* for IoT.

**Zero-RTT reconnection.** If you've connected before, you can resume instantly. Zero round trips. The first packet you send already carries data.

**Connection migration.** Your IP address changed because you switched from Wi-Fi to cellular? QUIC doesn't care. The connection *migrates* — no tear-down, no re-handshake.

**No head-of-line blocking.** In TCP, if one packet is lost, everything behind it stalls. QUIC uses independent streams within one connection, so a loss on one stream doesn't affect the others.

EMQX was the first broker to ship MQTT over QUIC natively in 2023. The IETF is working on a formal standard. This is the future for any IoT scenario involving mobile or unreliable networks.

---

## Slide 13: HTTP — The Universal Protocol

*(Shift tone slightly — more conversational)*

OK, let's talk about the elephant in the room. HTTP. Everyone's first thought is "why don't we just use HTTP for IoT?" And you know what? Sometimes you should.

HTTP is the most universally supported protocol on the planet. Every firewall knows how to handle it. Every NAT traverses it. If a web browser works on your network, HTTP will work for your devices. And the security story is excellent — OAuth2, API keys, JWT tokens, TLS. All battle-tested, well-understood.

The REST model maps beautifully to IoT operations. GET to read a sensor. POST to submit a reading. PUT to update device configuration. DELETE to remove a device. Clean, intuitive, and your web developers already know it.

**But.** And it's a big but.

HTTP headers are *huge*. Even a simple POST might have 400-500 bytes of headers for a 20-byte payload. That's a 25-to-1 overhead ratio. On a constrained network, that's criminal.

HTTP is request/response only. The server can't push data to the client without the client asking. Yes, you can do long-polling or server-sent events, but those are workarounds, not native capabilities.

And TLS on a tiny MCU is painful. You need a full TCP stack plus a TLS stack — that's 50-100 kB of RAM just for the network layer.

**My recommendation:** Use HTTP for gateway-to-cloud communication, not device-to-cloud. Your LoRaWAN gateway collects MQTT messages from sensors and forwards them to a cloud REST API? Perfect. Your Cortex-M0 talking directly to a REST endpoint? Think harder.

---

## Slide 14: HTTP Evolution — From 1.0 to HTTP/3

Quick history tour, because HTTP/3 is actually relevant to IoT now.

HTTP/1.0: one request per TCP connection. Open, request, response, close. The overhead of that connection setup for every single request is absurd.

HTTP/1.1: persistent connections. Open once, send multiple requests sequentially. Better, but still sequential — you have to wait for each response before sending the next request.

HTTP/2 in 2015: binary headers, multiplexing. Multiple requests and responses interleaved over one connection. This was a huge improvement. But still TCP underneath, which means head-of-line blocking at the transport layer.

And then HTTP/3, published as RFC 9114 in 2022. This is the one that matters. HTTP/3 replaces TCP with **QUIC**. Yes, the same QUIC we just talked about for MQTT. Zero-RTT, no head-of-line blocking, built-in TLS 1.3.

Over 30% of web traffic today runs on HTTP/3 — Cloudflare, Google, Facebook. For IoT, adoption is still early because most MCU HTTP stacks only support 1.1. But gateways can bridge. And there's active research on HTTP/3 publish/subscribe that could compete with MQTT for some use cases.

Keep an eye on this space. It's moving fast.

---

## Slide 15: CoAP — Constrained Application Protocol

Now we get to my favorite underdog. CoAP.

The IETF looked at HTTP and said: "We love the REST model. GET, PUT, POST, DELETE — brilliant. URIs — brilliant. But TCP? Headers? TLS? Way too heavy for a sensor with 32 kB of RAM."

So they built CoAP. RFC 7252, 2014. The design philosophy is literally "HTTP for constrained devices."

It runs over **UDP**. No TCP connection, no three-way handshake, no connection state to maintain. A sensor can wake up, fire a UDP packet, and go back to sleep. That's it. No lingering TCP connection consuming resources.

The header? **Four bytes.** Not four hundred — four. Compare that to HTTP.

And it uses the same RESTful semantics: GET to read, PUT to write, POST to create, DELETE to remove. URIs look identical: `coap://sensor.local/temperature`. You can even proxy between CoAP and HTTP almost transparently.

Now, UDP is unreliable, so CoAP adds its own reliability layer. Two message types: **CON** — Confirmable — which gets retransmitted until you get an ACK. And **NON** — Non-confirmable — fire and forget. You choose per-message. It's like MQTT QoS, but simpler.

Look at the two response patterns on the right. If the server has the answer ready, it piggybacks it on the ACK — one round trip. If it needs time to process, it sends an empty ACK first — "I got your request, hold on" — then sends the response later in a separate CON message.

The key insight: **no broker required.** CoAP is peer-to-peer. For small deployments where you don't want to run a broker, this is a significant advantage.

---

## Slide 16: CoAP Observe and LwM2M

"But wait," I hear you say, "CoAP is request/response. What if I want the server to push updates to me, like MQTT does?"

Enter **CoAP Observe**, RFC 7641. You send a GET with the Observe option, and the server remembers you. From that point on, whenever the resource changes, the server pushes a notification to you. It's publish/subscribe without a broker — the server itself acts as the publisher.

The tradeoff compared to MQTT: simpler — no broker infrastructure — but less scalable. If you have 10,000 subscribers, the server has to maintain 10,000 observation relationships. With MQTT, the broker handles that.

Now, **LwM2M** — Lightweight M2M. This is where CoAP really shines in the real world. OMA SpecWorks defined a complete device management protocol on top of CoAP. It defines a standardized **object model** — Object 3 is "Device" with manufacturer, model, battery level. Object 3303 is "Temperature." Object 3304 is "Humidity." Over 200 registered objects.

Why does this matter? Because every NB-IoT and LTE-M cellular IoT device uses LwM2M for device management, firmware updates, and configuration. If you're deploying cellular IoT, you will encounter LwM2M. Guaranteed.

And LwM2M version 1.2, released in 2022, made a big move: it now supports **MQTT and HTTP** as transports, not just CoAP. So the boundaries between these protocols are blurring. The *application model* matters more than the *transport*.

---

## Slide 17: XMPP — Extensible Messaging and Presence Protocol

*(Slightly lighter tone)*

OK, quick stop on XMPP. Some of you might recognize this — it started life as Jabber, the open-source instant messaging protocol. Remember Google Talk? That was XMPP. Facebook Messenger used XMPP before they switched to MQTT. There's a pattern here...

XMPP is XML-based. It has a beautiful addressing scheme — JIDs that look like email addresses: `sensor42@factory.com/vibration`. It has three message types: `message` for data, `presence` for "I'm online/offline," and `iq` for queries.

There are even IoT extensions — XEP-0323 and XEP-0325 — for sensor data and control.

**The problem?** XML. XML is *verbose*. A simple temperature reading that's 2 bytes of actual data might be wrapped in 200 bytes of XML tags. And parsing XML requires real memory — 64 kB minimum for a decent XML parser. On a Cortex-M0 with 32 kB of RAM? Not happening.

Bottom line: if you need chat-like functionality in IoT — a smart home app where you're sending notifications and commands through a conversation-like interface — XMPP can work. For sensor telemetry on constrained devices? Use MQTT. The industry has spoken.

---

## Slide 18: AMQP — Advanced Message Queuing Protocol

AMQP is the enterprise heavyweight. Born in the financial services world — JPMorgan created it in 2003 because they needed reliable, transactional messaging between trading systems.

This is a serious protocol. Binary wire format — efficient. Credit-based flow control — the *receiver* tells the sender how fast to go, preventing overload. Transactions and settlements — you can group messages into atomic units. And a layered architecture: connections contain sessions, sessions contain links, links carry transfers.

The frame types tell the story: open, begin, attach, transfer, flow, disposition, detach, end, close. It's like a well-organized bureaucracy — everything has its place, everything is tracked.

**For IoT, here's the deal:** AMQP is too heavy for edge devices. The protocol stack alone needs 100+ kB of RAM. But for backend message routing? It's excellent. RabbitMQ — one of the most popular message brokers in the world — speaks AMQP. Azure Service Bus speaks AMQP.

The architecture pattern: devices speak MQTT to an edge broker, the edge broker speaks AMQP to the cloud backend. Each protocol where it's strongest. Don't try to run AMQP on a microcontroller — use the right tool for the right layer.

---

## Slide 19: WebSocket, gRPC, and Edge Streaming

Let's look at the newer players and the bigger architectural picture.

**WebSocket** — RFC 6455, 2011. It starts as an HTTP connection, then "upgrades" to a full-duplex channel. After the handshake, the overhead is tiny — 2 to 14 bytes per frame. And here's the killer feature: it's **browser-native**. JavaScript has a built-in WebSocket API. So if you want to build a real-time IoT dashboard in a web browser, you can run MQTT *over* WebSocket. The browser connects to the MQTT broker via WebSocket, subscribes to topics, and gets live sensor data. No plugins, no hacks. This is how most IoT dashboards work in practice.

**gRPC** — Google's RPC framework, built on HTTP/2 with Protobuf serialization. Compact binary payloads, strongly typed with `.proto` schema files, and bidirectional streaming. It's *fantastic* for microservice-to-microservice communication. Your edge server talking to your cloud analytics engine? gRPC is excellent. But running it on a constrained MCU? No. It needs HTTP/2, which needs TCP, TLS... it's a gateway-and-above protocol.

And here's the **big picture** that's emerging in 2024-2025: the **edge streaming architecture**. Look at the diagram. Devices speak MQTT or CoAP to an edge broker. The edge broker does local processing — filtering, aggregation, anomaly detection. Then it forwards the *important* stuff to the cloud using gRPC or Kafka.

Speaking of **Apache Kafka** — it's not a broker, it's a distributed *event log*. Over a million messages per second, persistent, replayable. But it's designed for data centers, not for constrained devices. The pattern is: MQTT at the edge, Kafka in the cloud, with a bridge in between.

This multi-protocol architecture is *the* standard pattern now. Anyone who tells you "just use MQTT everywhere" or "just use HTTP everywhere" is oversimplifying.

---

## Slide 20: Protocol Comparison — Head-to-Head

*(Point at the table)*

Alright, let's put them all side by side. Take a moment to look at this table.

A few things I want you to notice.

**Transport:** CoAP is the only one on UDP. Everyone else is TCP. That's a fundamental architectural difference — UDP means no connection state, which is huge for battery-powered devices.

**Header size:** CoAP is 4 bytes, MQTT is 2 bytes, HTTP is *hundreds* of bytes. XMPP is XML, which means... who even knows, it depends on how much metadata you put in. But it's big.

**RAM footprint:** CoAP wins at 5-15 kB. MQTT needs 10-50 kB. HTTP needs 50-100 kB. AMQP and XMPP? 100 kB+. On a microcontroller with 64 kB of RAM total, this is the deciding factor.

**Multicast:** Only CoAP supports it. If you need to send one message to many devices simultaneously — firmware update announcement, network-wide configuration change — CoAP is your only standard option.

Don't memorize this table. *Understand* it. When someone asks you "which protocol should I use?" your answer should be a question: "What are your constraints?"

---

## Slide 21: Protocol Selection Guide — Think in Constraints

And here's the cheat sheet. Same elimination method we used for wireless protocols in Module 4.

Start with your **dominant constraint** and let it drive the decision.

RAM under 16 kB? **CoAP.** You don't have room for a TCP stack, let alone MQTT.

Many devices to one cloud? **MQTT 5.0.** Pub/sub decoupling and shared subscriptions handle the fan-in.

Existing web infrastructure? **HTTP.** Don't introduce new technology if you don't need it.

Cloud-to-cloud routing with transactions? **AMQP.** That's literally what it was designed for.

Real-time browser dashboard? **MQTT over WebSocket.** Best of both worlds.

Spotty or mobile network? **MQTT over QUIC.** Zero-RTT reconnect is a game-changer.

Device management and firmware OTA? **CoAP with LwM2M.** Standardized object model, every cellular IoT module supports it.

Industrial data unification? **MQTT with Sparkplug B.** Unified namespace, birth/death certificates, the whole Industry 4.0 package.

And notice the red box at the bottom: **many real systems use multiple protocols.** CoAP from device to gateway. MQTT from gateway to cloud. gRPC from cloud to analytics. Each hop uses what's optimal for *that* hop. That's good engineering.

---

## Slide 22: State of the Art — IoT Data Streaming in 2026

Let me give you the landscape as it stands right now.

**Established and production-ready:** MQTT 5.0 is the undisputed champion for device-to-cloud. All three major clouds — AWS, Azure, Google — support it natively. CoAP with LwM2M 1.2 is the standard for cellular IoT device management, especially on NB-IoT and LTE-M. HTTP REST is the universal glue for cloud-side integrations — it's not going anywhere. AMQP powers the enterprise messaging backbone — Azure Service Bus, RabbitMQ.

**Emerging and gaining traction:** MQTT over QUIC is production-ready on EMQX, with an IETF draft progressing. It's critical for connected vehicles — Tesla, BMW, the automotive industry is watching this closely. Sparkplug B and the Unified Namespace are becoming *the* Industry 4.0 architecture. HTTP/3 publish/subscribe is showing promise in research. And the Kafka-MQTT bridge is becoming the standard edge-to-cloud pipeline for high-volume analytics.

The trend to watch: **protocol convergence at the edge.** The edge gateway is becoming the Rosetta Stone of IoT. Devices speak whatever is lightest — CoAP, MQTT, even proprietary protocols. The cloud speaks whatever is most powerful — HTTP, gRPC, Kafka. And the gateway translates. The "which protocol is best?" war is over. The answer is: **all of them, at the right layer.**

---

## Slide 23: Key Takeaways

Rapid fire. Here's what I want you to remember.

**MQTT:** Pub/sub over TCP. Three QoS levels. Two-byte header. The king of IoT. MQTT 5.0 adds topic aliases, shared subscriptions, and request/response.

**CoAP:** REST over UDP. Four-byte header. Best for the most constrained devices. Observe adds pub/sub capability. LwM2M builds device management on top.

**HTTP:** Universal, but heavy. Use it for gateway-to-cloud, not device-to-cloud. HTTP/3 is interesting but MCU adoption is still early.

**What's new:** MQTT over QUIC for mobile IoT. Sparkplug B for industrial data. LwM2M 1.2 now speaks MQTT and HTTP too. Multi-protocol edge gateways are the norm.

And the meta-lesson, the one I really want you to internalize: **the protocol you choose for device-to-gateway is often different from gateway-to-cloud.** Design each hop separately, then bridge at the gateway. That's how the professionals do it.

---

## Slide 24: Questions?

Alright, that's Module 6. Data streaming — from device to cloud.

Before you go: for your projects, I want you to **justify** your protocol choice the same way we do in the exercises. What's your dominant constraint? What did you eliminate and why? Don't just say "I used MQTT because everyone uses MQTT." Tell me *why* MQTT is right for *your* specific scenario. And if CoAP is better — use CoAP. I'll be more impressed by a well-justified unconventional choice than a lazy conventional one.

Questions? Fire away.

---

*END OF SCRIPT*

**Estimated delivery time: ~50 minutes**
**Pacing notes:**
- MQTT section (slides 5–12) is the longest — ~25 minutes. This is intentional.
- HTTP/CoAP (slides 13–16) — ~10 minutes.
- XMPP/AMQP/Emerging (slides 17–19) — ~8 minutes. Keep it brisk.
- Selection/Summary (slides 20–23) — ~7 minutes.
