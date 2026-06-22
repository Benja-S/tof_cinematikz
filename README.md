# 📐 Cinematikz

> *Watch calculus happen in real time.*

A live physics demo that reads distance from a time-of-flight sensor and computes **velocity** and **acceleration** on the fly using numerical derivatives — then streams everything to a browser, anywhere in the world.

Move your hand. Watch the math react.

---

## What it does

An ESP8266 samples distance 20 times per second. From that single stream of numbers, it derives:

| Signal | Math | Meaning |
|---|---|---|
| **Position** x(t) | raw sensor | where the object is |
| **Velocity** v(t) | dx/dt | how fast it's moving |
| **Acceleration** a(t) | d²x/dt² | how fast *that* is changing |

All three are plotted live in the browser. The UI explains what you're seeing as it happens — when velocity hits zero, when acceleration and velocity point in opposite directions, why the acceleration curve is noisier than the others.

---

## Stack

```
[ESP8266 + VL53L0X ToF sensor]
           │
           │  MQTT over TLS
           ▼
   [HiveMQ public broker]
           │
           │  WebSocket
           ▼
  [Browser — phone or desktop]
```

**Firmware:** C++ / Arduino framework, PlatformIO  
**Transport:** MQTT over WebSocket  
**Frontend:** Vanilla JS, Chart.js — single HTML file, no build step  

---

## The math

Derivatives are approximated using **central differences** — more accurate than forward differences because the error terms cancel:

```
v(t) ≈  [ x(t + Δt) − x(t − Δt) ]  /  2Δt          error: O(Δt²)

a(t) ≈  [ x(t + Δt) − 2x(t) + x(t − Δt) ]  /  Δt²  error: O(Δt²)
```

With Δt = 50ms, halving the sample interval quarters the error. The acceleration curve is visibly noisier — that's not a bug, it's a lesson: each differentiation amplifies measurement noise. Real systems use Kalman filters for exactly this reason.

---

## Hardware

| Part | Role |
|---|---|
| ESP8266 NodeMCU | microcontroller + WiFi |
| VL53L0X | time-of-flight distance sensor (I²C) |

No sensor? A built-in sine wave simulator lets you explore the math before any hardware arrives.

---

## Project structure

```
include/
  SensorBase.h        ← abstract sensor interface
  SimulatedSensor.h   ← sine wave generator
  VL53L0XSensor.h     ← real hardware (swap in when ready)
  DataBuffer.h        ← ring buffer of recent samples
  DerivativeCalc.h    ← finite difference implementation
  MQTTPublisher.h     ← WiFi + MQTT + JSON serialisation
  MotionState.h       ← data model (x, v, a, t)
  Config.h            ← credentials and tuning

src/
  main.cpp            ← setup() / loop()

web/
  index.html          ← self-contained browser UI
```

Switching from simulated to real sensor is one line in `platformio.ini`.

---

## Live demo

🌐 **[YOUR_USERNAME.github.io/tof-cinematikz](https://your_username.github.io/tof-cinematikz)**

*(Requires an ESP running the firmware to see live data)*

---

## Built with

- [PlatformIO](https://platformio.org/)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [ArduinoJson](https://arduinojson.org/)
- [Chart.js](https://www.chartjs.org/)
- [HiveMQ](https://www.hivemq.com/)
