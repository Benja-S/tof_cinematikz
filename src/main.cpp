#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Config.h"
#include "MotionState.h"
#include "DataBuffer.h"
#include "DerivativeCalc.h"
#include "MQTTPublisher.h"

// ── Sensor selection ──────────────────────────────────────────────────────────
// Controlled by SENSOR_MODE flag in platformio.ini:
//   -D SENSOR_MODE=SIMULATED  → uses sine wave generator
//   -D SENSOR_MODE=REAL       → uses physical VL53L0X
#ifdef SENSOR_REAL
  #include "VL53L0XSensor.h"
  VL53L0XSensor sensor;
#else
  #include "SimulatedSensor.h"
  SimulatedSensor sensor;
#endif

// ── Module instances ──────────────────────────────────────────────────────────
DataBuffer    buffer;
DerivativeCalc calc;
MQTTPublisher  mqtt;

// ── Timing ────────────────────────────────────────────────────────────────────
uint32_t last_sample_ms  = 0;
uint32_t last_publish_ms = 0;

// ─────────────────────────────────────────────────────────────────────────────
void connectWiFi() {
    Serial.print("\n[WiFi] Connecting to ");
    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\n[WiFi] Connected! IP: ");
    Serial.println(WiFi.localIP());
}

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(9600);
    delay(200);

    Serial.println("\n=== ESP8266 Derivatives Demo ===");
    Serial.print("[Sensor] Using: ");
    Serial.println(sensor.name());

    if (!sensor.begin()) {
        Serial.println("[ERROR] Sensor failed to initialise. Halting.");
        while (true) delay(1000);
    }

    connectWiFi();
    mqtt.begin();

    Serial.println("[Setup] Done. Starting measurement loop.");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
    mqtt.loop(); // keepalive / reconnect

    uint32_t now = millis();

    // ── Sample the sensor ─────────────────────────────────────────────────────
    if (now - last_sample_ms >= SAMPLE_INTERVAL_MS) {
        last_sample_ms = now;

        float dist = sensor.readMM();
        if (dist < 0) return; // sensor error — skip this tick

        buffer.push(now, dist);
    }

    // ── Compute derivatives and publish ───────────────────────────────────────
    if (now - last_publish_ms >= PUBLISH_INTERVAL_MS) {
        last_publish_ms = now;

        MotionState state = calc.compute(buffer);
        if (!state.valid) return;

        bool ok = mqtt.publish(state);

        // Debug output on Serial (can be viewed in PlatformIO Monitor)
        Serial.printf("[%.2fs] x=%.1fmm  v=%.1fmm/s  a=%.1fmm/s²  mqtt=%s\n",
            state.timestamp_ms / 1000.0f,
            state.position_mm,
            state.velocity_mms,
            state.accel_mms2,
            ok ? "OK" : "FAIL"
        );
    }
}