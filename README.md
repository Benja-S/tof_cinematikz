# 📐 Cinematikz

> *Watch calculus happen in real time.*

Una demostración de física en vivo que lee distancia desde un sensor de tiempo de vuelo y calcula **velocidad** y **aceleración** en tiempo real mediante derivadas numéricas, transmitiendo todos los datos a un navegador desde cualquier lugar del mundo.

Mueve tu mano. Observa cómo las matemáticas reaccionan.

---

## Qué hace

Un ESP8266 mide la distancia 20 veces por segundo. A partir de ese único flujo de datos, obtiene:

| Señal | Matemática | Significado |
|---|---|---|
| **Posicion** x(t) | raw sensor | posición del objeto |
| **Velocidad** v(t) | dx/dt | qué tan rápido se mueve |
| **Aceleracion** a(t) | d²x/dt² | qué tan rápido cambia esa velocidad |

Las tres señales se grafican en vivo en el navegador. La interfaz explica lo que ocurre mientras sucede: cuándo la velocidad llega a cero, cuándo aceleración y velocidad apuntan en direcciones opuestas y por qué la curva de aceleración presenta más ruido que las demás.

---

## Arquitectura

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
**Transporte:** MQTT over WebSocket  
**Frontend:** Vanilla JS, Chart.js — single HTML file, no build step  

---

## Matemática

Las derivadas se aproximan usando **diferencias centrales**, un método más preciso que las diferencias hacia adelante porque los términos de error se cancelan:

```
v(t) ≈  [ x(t + Δt) − x(t − Δt) ]  /  2Δt          error: O(Δt²)

a(t) ≈  [ x(t + Δt) − 2x(t) + x(t − Δt) ]  /  Δt²  error: O(Δt²)
```

Con Δt = 50 ms, reducir a la mitad el intervalo de muestreo reduce el error a una cuarta parte. La curva de aceleración presenta más ruido visual; no es un error, es una lección: cada derivación amplifica el ruido de medición. Los sistemas reales utilizan filtros de Kalman por esta misma razón.

---

## Hardware

| Part | Role |
|---|---|
| ESP8266 NodeMCU | microcontroller + WiFi |
| VL53L0X | time-of-flight distance sensor (I²C) |

¿Sin sensor? Un simulador de onda sinusoidal integrado permite explorar la matemática antes de disponer del hardware.

---

## Estructura del proyecto

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

Cambiar del sensor simulado al sensor real requiere modificar una sola línea en `platformio.ini`.

---

## Demo en vivo

🌐 **[benja-s.github.io/tof-cinematikz](https://benja-s.github.io/tof_cinematikz)**

*(Requiere un ESP ejecutando el firmware para visualizar datos en vivo)*

---

## Construido con

- [PlatformIO](https://platformio.org/)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [ArduinoJson](https://arduinojson.org/)
- [Chart.js](https://www.chartjs.org/)
- [HiveMQ](https://www.hivemq.com/)
  
- [EFN]
