# BOM Examples: Demonstrating Viability & Scalability

## Purpose
These examples show how to connect **PES requirements → component selection → viability proof → scalability analysis**. Use these as templates for your own project.

---

## Example 1: Smart Agriculture (Vineyard Soil Monitoring)

### Context
- **Deployment**: 50 sensor nodes across 10-acre vineyard
- **Environment**: Outdoor, New England climate, solar-powered
- **Communication**: LoRaWAN to central gateway

### PES Requirements (excerpt)
- **R1**: Measure soil moisture every 10 minutes with ±2% accuracy
- **R2**: 2 km range to gateway, outdoor deployment with obstacles (vines, terrain)
- **R3**: Solar-powered operation, 365 days/year including winter (3.5h avg sun/day)
- **R4**: Total system cost < $200/node at 50-unit deployment

### Bill of Materials (1 node)

| Component | Part # | Supplier | Unit $ | Qty | Justification |
|-----------|--------|----------|--------|-----|---------------|
| Soil moisture sensor | Capacitive v1.2 | Amazon | $6.99 | 2 | **R1**: Capacitive (not resistive) to avoid corrosion in soil. Datasheet spec: ±3% accuracy meets R1 requirement. 2 units for redundancy. |
| LoRa transceiver | RFM95W 868MHz | Adafruit | $19.95 | 1 | **R2**: LoRa 868MHz ISM band, +20dBm max TX power. Link budget calculation (see report): 2.5km range with 10dB margin. |
| MCU | STM32L072 | Mouser | $4.50 | 1 | **R3**: Ultra-low-power: 0.8µA sleep mode (datasheet Table 24). 32kB RAM sufficient for sensor data buffering. |
| Solar panel | 6V 1W monocrystalline | Amazon | $8.99 | 1 | **R3**: 1W panel + 18650 Li-ion battery. Energy budget: 3.5h sun/day × 1W = 3.5Wh/day. Consumption: 0.8Wh/day (calculated). Margin: 4.4×. |
| Battery | 18650 Li-ion 3000mAh | Amazon | $5.99 | 1 | **R3**: 3000mAh × 3.7V = 11.1Wh capacity. Supports 14 days without sun (winter contingency). |
| Enclosure | IP67 ABS 100×68×50mm | Amazon | $7.50 | 1 | Outdoor deployment requires IP67 rating. Datasheet: -40°C to +85°C operating range. |
| **Total (prototype)** | | | **$53.92** | | |
| **Total at 50 units** | | | **$45.20** | | 15% bulk discount (Mouser quote) |
| **Total at 1000 units** | | | **$32.80** | | 40% bulk discount + PCB assembly |

### Viability Proof
1. **Sensor accuracy**: Validated DHT22 against calibrated soil moisture probe. Mean error: 1.8% (within spec).
2. **Link budget**: Calculated 2.5km range. Field test: 2.1km achieved with 8dB margin.
3. **Power budget**: Measured sleep current: 1.2µA (vs 0.8µA datasheet). Still meets 365-day requirement.
4. **Environmental**: Tested enclosure at -10°C and +40°C. No condensation, seals intact.

### Scalability Analysis
- **Cost**: $53.92/node prototype → $32.80/node at 1000 units (39% reduction)
- **Supply chain**: All components available from ≥2 suppliers (Mouser, Digi-Key, Amazon)
- **Lead times**: RFM95W: 2 weeks, STM32L072: 4 weeks, others: <1 week
- **Manufacturing**: PCB design ready (KiCad files in repo). Assembly quote: $8/board at 1000 qty.
- **Cloud costs**: LoRaWAN gateway + ChirpStack: $0/month (self-hosted). InfluxDB Cloud: $50/month for 1000 nodes.
- **Data storage**: 1 node × 144 readings/day × 8 bytes = 1.15 kB/day. 1000 nodes = 1.15 MB/day = 420 MB/year. InfluxDB free tier: 10 GB sufficient.

### Portfolio Impact
This demonstrates:
- ✅ Requirements-driven component selection
- ✅ Quantitative viability proof (tested, not assumed)
- ✅ Scalability to production (cost, supply chain, manufacturing)
- ✅ Real-world constraints (power, environment, cost)

---

## Example 2: Predictive Maintenance (Industrial Pump Vibration)

### Context
- **Deployment**: 20 pumps in chemical plant
- **Environment**: Indoor, 24/7 operation, WiFi available
- **Goal**: Detect bearing wear before failure (downtime cost: $50k/hour)

### PES Requirements (excerpt)
- **R1**: Measure vibration at 1 kHz sampling rate with ±0.1g accuracy
- **R2**: Detect bearing defects (90-300 Hz frequency range)
- **R3**: WiFi MQTT to plant network, publish every 10 seconds
- **R4**: Continuous operation (mains-powered, no battery)

### Bill of Materials (1 node)

| Component | Part # | Supplier | Unit $ | Qty | Justification |
|-----------|--------|----------|--------|-----|---------------|
| Accelerometer | ADXL345 (3-axis) | Adafruit | $14.95 | 1 | **R1**: ±16g range, 13-bit resolution = 0.004g/LSB (datasheet p.4). Meets ±0.1g requirement. I2C interface. |
| MCU | ESP32-WROOM-32 | Amazon | $8.99 | 1 | **R3**: Built-in WiFi, 520kB RAM for FFT buffer. MQTT library available. |
| ADC | Internal ESP32 | — | $0 | — | **R1**: ESP32 internal ADC: 12-bit, 1 kHz capable. ADXL345 is digital (I2C), no external ADC needed. |
| Power supply | 5V 2A USB | Amazon | $6.99 | 1 | **R4**: Mains-powered via USB. ESP32 + ADXL345: 240mA peak (WiFi TX). 2A sufficient. |
| Enclosure | DIN rail mount | Amazon | $9.50 | 1 | Industrial environment. DIN rail standard in control panels. |
| **Total (prototype)** | | | **$40.43** | | |
| **Total at 20 units** | | | **$38.20** | | Minimal bulk discount (small qty) |
| **Total at 1000 units** | | | **$28.50** | | 30% bulk discount |

### Viability Proof
1. **Sampling rate**: Tested ESP32 I2C read rate: 1.2 kHz achieved (exceeds 1 kHz requirement).
2. **Frequency detection**: FFT on 1024-sample window detects 90-300 Hz peaks. Validated against function generator.
3. **Accuracy**: ADXL345 calibrated against reference accelerometer. RMS error: 0.08g (within spec).
4. **WiFi reliability**: Tested in plant environment. Packet loss: 0.2% (acceptable for 10-second publish rate).

### Scalability Analysis
- **Cost**: $40.43/node → $28.50/node at 1000 units
- **Cloud costs**: MQTT broker (Mosquitto): $0/month (self-hosted on plant server). InfluxDB: $100/month for 1000 pumps.
- **Data volume**: 1 pump × 6 samples/min × 12 bytes = 72 bytes/min = 103 kB/day. 1000 pumps = 103 MB/day = 38 GB/year.
- **Maintenance**: OTA firmware updates via ESP32. No site visits for software changes.

---

## Example 3: Environmental Monitoring (Indoor Air Quality)

### Context
- **Deployment**: 100 classrooms in university campus
- **Environment**: Indoor, WiFi available, USB-powered
- **Goal**: Monitor CO2, temp, humidity for HVAC optimization

### PES Requirements (excerpt)
- **R1**: Measure CO2 (400-5000 ppm) with ±50 ppm accuracy
- **R2**: Measure temperature (15-30°C) with ±0.5°C accuracy
- **R3**: WiFi MQTT, publish every 60 seconds
- **R4**: Low-cost deployment: <$50/node at 100-unit scale

### Bill of Materials (1 node)

| Component | Part # | Supplier | Unit $ | Qty | Justification |
|-----------|--------|----------|--------|-----|---------------|
| CO2 sensor | MH-Z19B NDIR | Amazon | $19.95 | 1 | **R1**: NDIR sensor, ±50 ppm accuracy (datasheet p.2). Range: 0-5000 ppm. UART interface. |
| Temp/Humidity | DHT22 | Amazon | $4.50 | 1 | **R2**: ±0.5°C accuracy (datasheet p.3). ±2% RH. Single-wire interface. |
| MCU | ESP8266 NodeMCU | Amazon | $6.99 | 1 | **R3**: Built-in WiFi, 80 MHz, 80kB RAM. Lower cost than ESP32 (sufficient for this use case). |
| Power supply | 5V 1A micro-USB | Amazon | $4.99 | 1 | **R4**: USB-powered from classroom outlet. ESP8266 + sensors: 180mA peak. |
| Enclosure | 3D-printed ABS | In-house | $2.50 | 1 | University has 3D printers. STL files in repo. Cost = material only. |
| **Total (prototype)** | | | **$38.93** | | |
| **Total at 100 units** | | | **$35.20** | | 10% bulk discount |
| **Total at 1000 units** | | | **$28.80** | | 26% bulk discount |

### Viability Proof
1. **CO2 accuracy**: Validated MH-Z19B against Vaisala reference sensor. Mean error: 42 ppm (within ±50 ppm spec).
2. **Temperature accuracy**: DHT22 vs calibrated thermometer. Mean error: 0.3°C (within spec).
3. **WiFi coverage**: Tested in 10 classrooms. All locations have >-70 dBm signal strength (good).
4. **Power consumption**: Measured 165mA average (vs 180mA calculated). Margin: 9%.

### Scalability Analysis
- **Cost**: $38.93/node → $28.80/node at 1000 units (26% reduction)
- **Deployment**: 100 classrooms × $35.20 = $3,520 total hardware cost
- **Cloud costs**: AWS IoT Core: $0.08/million messages. 100 nodes × 1440 msg/day = 144k msg/day = $0.35/month.
- **Data storage**: 100 nodes × 1440 readings/day × 16 bytes = 2.3 MB/day = 840 MB/year. S3: $0.02/month.
- **ROI**: HVAC optimization: estimated 15% energy savings = $12k/year. Payback: 4 months.

---

## Key Takeaways for Your BOM

### 1. Viability Checklist
- [ ] Every component answers a specific PES requirement
- [ ] Every spec is cited from a datasheet (page/table number)
- [ ] Calculations are shown (power budget, link budget, data volume)
- [ ] Experimental validation is planned (sensor accuracy, range test, etc.)

### 2. Scalability Checklist
- [ ] Unit cost at prototype qty documented
- [ ] Unit cost at 100 and 1000 units estimated (with supplier quotes if possible)
- [ ] Supply chain: ≥2 suppliers for critical components
- [ ] Lead times documented (<4 weeks preferred)
- [ ] Cloud costs estimated (MQTT broker, database, storage)
- [ ] Data volume calculated (bytes/day → GB/year)

### 3. Portfolio Narrative
Your BOM + validation report tells this story:

> "I designed a [system] for [problem]. I selected components based on [requirements], validated them experimentally, and demonstrated a working prototype. The system is viable (meets all specs) and scalable (cost drops 30% at 1000 units, supply chain secured, cloud costs <$100/month for 1000 devices)."

This is what employers want to see: **requirements → design → validation → scale**.

---

## Common Questions

**Q: What if my prototype BOM is $52 (over budget)?**  
A: Document it. Show that at 100 units it drops to $42. Explain which component drives cost and what alternatives exist.

**Q: What if I can't test my sensor against a calibrated reference?**  
A: Use a proxy. For temperature: ice water (0°C) and boiling water (100°C). For distance: tape measure. Document your method and uncertainty.

**Q: What if a component has 6-week lead time?**  
A: Note it in your BOM. Identify a backup component with shorter lead time. Explain the tradeoff (cost vs availability).

**Q: How do I estimate bulk pricing?**  
A: Check Digi-Key/Mouser quantity pricing tables. Or email sales@supplier.com with "quote request for 1000 units of [part #]". They respond in 1-2 days.

**Q: What if my project doesn't scale to 1000 units (e.g., custom installation)?**  
A: Scale differently. Show cost per installation site, or cost to add 10 more sensors to existing system. Scalability isn't always about quantity—it's about demonstrating you thought beyond the prototype.

---

**Remember**: This BOM goes in your portfolio. Make it professional. An employer reading it should think: "This person can design a real product."
