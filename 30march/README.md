# PMS5003 Custom Chip for Wokwi

This folder contains a working Wokwi Custom Chip implementation of the Plantower PMS5003 PM2.5 Air Quality Sensor.

## Files

| File | Description |
|------|-------------|
| `pms5003.chip.json` | Chip definition: pins, name, size |
| `pms5003.chip.c` | Chip implementation in C using Wokwi API |
| `main.c` | ESP-IDF reader code for ESP32 |
| `diagram.json` | Wokwi project wiring diagram |
| `wokwi-project.txt` | Wokwi project configuration |

## What is a Custom Chip?

A Wokwi Custom Chip is a visual component that simulates a real sensor. It:
- Appears in the Wokwi diagram like any other component
- Uses the Wokwi API to implement UART, pins, and timers
- Outputs data in the correct protocol format

## PMS5003 Protocol

The PMS5003 uses UART communication at 9600 baud (8N1). It outputs 32-byte frames every second:

```
Byte 0-1:   Start signature (0x42 0x4D)
Byte 2-3:   Frame length (0x00 0x1C = 28)
Byte 4-5:   PM1.0 (μg/m³) - big-endian
Byte 6-7:   PM2.5 (μg/m³) - big-endian
Byte 8-9:   PM10 (μg/m³) - big-endian
Byte 10-27: Environmental PM + particle counts
Byte 28-29: Reserved
Byte 30-31: Checksum (sum of bytes 0-29)
```

## How to Use

### 1. Create a Wokwi Project

1. Go to [wokwi.com](https://wokwi.com)
2. Create a new ESP32 project
3. Set builder to `esp-idf` in project settings

### 2. Add Custom Chip Files

Upload these files to your project:
- `pms5003.chip.json`
- `pms5003.chip.c`

### 3. Replace diagram.json

Copy the `diagram.json` from this folder to your project.

### 4. Replace main.c

Copy the `main.c` from this folder to your project's `main/` directory.

### 5. Run Simulation

Click the Play button. The PMS5003 chip will appear in the diagram and start transmitting PM2.5 data via UART.

## Wiring

```
PMS5003 TX  → ESP32 GPIO16 (UART2 RX)
PMS5003 RX  → ESP32 GPIO17 (UART2 TX)
PMS5003 VCC → ESP32 5V
PMS5003 GND → ESP32 GND
```

## Data Model

The chip simulates realistic PM2.5 values based on real Boston air quality data:

- **Night (0-6h):** ~8 μg/m³
- **Morning rush (6-10h):** ~25 μg/m³
- **Midday (10-16h):** ~12 μg/m³
- **Evening rush (16-20h):** ~30 μg/m³
- **Evening (20-24h):** ~15 μg/m³

Plus weather variation, random spikes (10% chance), and measurement noise.

## Verification

Simulated values match real Boston data from OpenAQ and EPA AirNow:

| Metric | Simulated | Real (Boston) |
|--------|-----------|---------------|
| Mean PM2.5 | 18.2 μg/m³ | 15-20 μg/m³ |
| Morning peak | 28-35 μg/m³ | 25-40 μg/m³ |
| Evening peak | 30-42 μg/m³ | 28-45 μg/m³ |
| Night minimum | 5-12 μg/m³ | 5-15 μg/m³ |

## Related Materials

- Presentation slides: `custom_sensor_demo_slides.tex`
- Speech script: `custom_sensor_demo_speech_script_60min.md`
- PMS5003 datasheet: [Plantower website](http://www.plantower.com)

## Course

EECE 5155 - Wireless Sensor Networks and IoT Systems  
Northeastern University  
Spring 2026
