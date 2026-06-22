# ESP8266 Derivatives Demo

An educational project demonstrating calculus derivatives in real time
using a VL53L0X time-of-flight sensor and an ESP8266.

```
[ESP8266 + VL53L0X]
       │
       │  MQTT over WiFi
       ▼
[HiveMQ Cloud broker]
       │
       │  MQTT over WebSocket
       ▼
[Browser — anywhere in the world]
  • Live position / velocity / acceleration charts
  • Pedagogical annotations
  • Phase portrait (x vs v)
```

---

## 1. HiveMQ Cloud setup (free, 5 min)

1. Go to https://www.hivemq.com/mqtt-cloud-broker/ → **Start Free**
2. Create a cluster (EU or US, doesn't matter)
3. Under **Access Management** → create a username + password
4. Note your **Cluster URL** (looks like `abc123.s2.eu.hivemq.cloud`)

---

## 2. Configure the firmware

Edit `include/Config.h`:

```cpp
#define WIFI_SSID     "your_network"
#define WIFI_PASSWORD "your_password"
#define MQTT_HOST     "abc123.s2.eu.hivemq.cloud"
#define MQTT_PORT     1883
#define MQTT_USER     "your_user"
#define MQTT_PASS     "your_pass"
```

---

## 3. Flash the ESP8266

```bash
# In PlatformIO terminal:
pio run --target upload

# View serial output:
pio device monitor
```

Expected serial output:
```
=== ESP8266 Derivatives Demo ===
[Sensor] Using: SimulatedSensor (sine)
[SimulatedSensor] Ready — sinusoidal motion
[WiFi] Connecting to YourNetwork....... Connected! IP: 192.168.1.42
[MQTT] Connecting to abc123.s2.eu.hivemq.cloud... connected!
[0.05s] x=500.3mm  v=628.3mm/s  a=-4935.2mm/s²  mqtt=OK
```

---

## 4. Deploy the web UI

The `web/index.html` is a single static file. Deploy it anywhere:

### Option A — GitHub Pages (recommended, free, public URL)
1. Push this repo to GitHub
2. Settings → Pages → Source: `main` branch, `/web` folder
3. Your URL: `https://yourusername.github.io/repo-name/`

### Option B — Netlify drag-and-drop
1. Go to https://app.netlify.com/drop
2. Drag the `web/` folder onto the page
3. Get a public URL instantly

### Option C — Local testing
Just open `web/index.html` in a browser. Works immediately.

---

## 5. Swap to real hardware

When your VL53L0X arrives:

1. Wire it (I²C):
   ```
   VL53L0X VIN  → 3.3V
   VL53L0X GND  → GND
   VL53L0X SDA  → D2 (GPIO4)
   VL53L0X SCL  → D1 (GPIO5)
   ```

2. In `platformio.ini`, change:
   ```ini
   build_flags = -D SENSOR_MODE=REAL
   ```

3. Re-flash. The web UI doesn't change at all.

---

## Architecture

```
include/
  Config.h          ← All credentials and tuning constants
  MotionState.h     ← Data model (x, v, a, t)
  SensorBase.h      ← Abstract sensor interface
  SimulatedSensor.h ← Sine wave generator (current)
  VL53L0XSensor.h   ← Real hardware (swap in when ready)
  DataBuffer.h      ← Ring buffer for recent samples
  DerivativeCalc.h  ← Finite difference math (documented)
  MQTTPublisher.h   ← WiFi + MQTT + JSON serialisation

src/
  main.cpp          ← setup() / loop() orchestration

web/
  index.html        ← Self-contained browser UI
```

---

## Derivative math

The firmware uses **central differences** (2nd order accuracy):

```
velocity:      v(t) ≈ [x(t+Δt) − x(t−Δt)] / (2Δt)      error: O(Δt²)

acceleration:  a(t) ≈ [x(t+Δt) − 2x(t) + x(t−Δt)] / Δt²  error: O(Δt²)
```

Compared to forward differences (`[x(t) − x(t−Δt)] / Δt`, error O(Δt)),
central differences are more accurate because the error terms cancel.

With Δt = 50ms and the simulated sine wave, you can verify numerically
that the ESP's computed derivatives match the analytical solution:

```
v_analytical(t) = A·ω·cos(ω·t)    where ω = 2π/T
a_analytical(t) = −A·ω²·sin(ω·t)
```
