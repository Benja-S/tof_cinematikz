#pragma once

// ─── WiFi ────────────────────────────────────────────────────────────────────
#define WIFI_SSID     "WiFi motoedge"
#define WIFI_PASSWORD "12345678"

// ─── MQTT (HiveMQ Cloud free tier) ───────────────────────────────────────────
// Sign up at https://www.hivemq.com/mqtt-cloud-broker/
// Create a free cluster → copy the host, port 8883 (TLS) or 1883 (plain)
// For ESP8266 without TLS overhead, use port 1883 with IP whitelisting
#define MQTT_HOST     "d1602d6aec814d75ab189bac6e08abfc.s1.eu.hivemq.cloud"
#define MQTT_PORT     8883
#define MQTT_USER     ""
#define MQTT_PASS     ""
#define MQTT_CLIENT_ID "esp8266-derivatives"

// Topics
#define TOPIC_MOTION  "derivatives/motion"   // JSON payload published by ESP
#define TOPIC_CONFIG  "derivatives/config"   // config commands sent from browser

// ─── Sampling ────────────────────────────────────────────────────────────────
#define SAMPLE_INTERVAL_MS   50    // 20 Hz sampling rate
#define PUBLISH_INTERVAL_MS  50    // same — publish every sample
#define BUFFER_SIZE          64    // how many samples to keep for derivative calc

// ─── Derivative calculation ───────────────────────────────────────────────────
// Order of finite difference stencil (1 = forward diff, 2 = central diff)
#define DERIVATIVE_ORDER  2

// ─── Simulation ──────────────────────────────────────────────────────────────
#define SIM_AMPLITUDE_MM   300.0f   // object swings ±300mm from center
#define SIM_CENTER_MM      500.0f   // center distance in mm
#define SIM_PERIOD_MS     3000      // period of one oscillation in ms
#define SIM_NOISE_MM         2.0f   // Gaussian noise amplitude in mm
