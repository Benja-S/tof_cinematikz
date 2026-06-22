#pragma once
#include "Config.h"
#include "MotionState.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/**
 * MQTTPublisher
 * -------------
 * Serialises a MotionState to JSON and publishes it to the MQTT broker.
 * Uses TLS (port 8883). Works with both authenticated brokers (HiveMQ Cloud)
 * and anonymous brokers (broker.hivemq.com public).
 */
class MQTTPublisher {
public:
    MQTTPublisher() : _mqtt(_wifiClient) {}

    void begin() {
        _wifiClient.setInsecure();      // encrypt but skip cert verification
        _wifiClient.setTimeout(15000);  // 15s for TLS handshake
        _mqtt.setServer(MQTT_HOST, MQTT_PORT);
        _mqtt.setBufferSize(512);
        _mqtt.setSocketTimeout(15);
        _connect();
    }

    void loop() {
        if (!_mqtt.connected()) _connect();
        _mqtt.loop();
    }

    bool publish(const MotionState& s) {
        if (!_mqtt.connected()) return false;

        JsonDocument doc;
        doc["t"]  = s.timestamp_ms;
        doc["x"]  = roundf(s.position_mm  * 10.0f) / 10.0f;
        doc["v"]  = roundf(s.velocity_mms * 10.0f) / 10.0f;
        doc["a"]  = roundf(s.accel_mms2   * 10.0f) / 10.0f;
        doc["dt"] = roundf(s.dt_ms        * 10.0f) / 10.0f;

        char buf[128];
        size_t len = serializeJson(doc, buf);
        return _mqtt.publish(TOPIC_MOTION, buf, len);
    }

    bool connected() { return _mqtt.connected(); }

private:
    WiFiClientSecure _wifiClient;
    PubSubClient     _mqtt;
    uint32_t         _last_attempt_ms = 0;

    void _connect() {
        if (millis() - _last_attempt_ms < 15000) return;
        _last_attempt_ms = millis();

        Serial.print("[MQTT] Connecting to ");
        Serial.print(MQTT_HOST);
        Serial.print("...");

        // Use anonymous connect if credentials are empty
        bool ok = (strlen(MQTT_USER) > 0)
            ? _mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)
            : _mqtt.connect(MQTT_CLIENT_ID);

        if (ok) {
            Serial.println(" connected!");
        } else {
            Serial.print(" failed, rc=");
            Serial.println(_mqtt.state());
        }
    }
};