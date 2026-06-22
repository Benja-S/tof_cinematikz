#pragma once
#include "SensorBase.h"
#include <Adafruit_VL53L0X.h>

/**
 * VL53L0XSensor
 * -------------
 * Wraps the Adafruit VL53L0X library in our SensorBase interface.
 * Drop-in replacement for SimulatedSensor — change one line in main.cpp.
 *
 * Wiring (ESP8266 NodeMCU):
 *   VL53L0X VIN  → 3.3V
 *   VL53L0X GND  → GND
 *   VL53L0X SDA  → D2 (GPIO4)
 *   VL53L0X SCL  → D1 (GPIO5)
 *   VL53L0X XSHUT → not connected (or GPIO for enable control)
 */
class VL53L0XSensor : public SensorBase {
public:
    bool begin() override {
        if (!_lox.begin()) {
            Serial.println("[VL53L0X] ERROR: sensor not found. Check wiring.");
            return false;
        }
        // Long-range mode: up to ~2m, slightly slower
        _lox.startRangeContinuous(50); // 50ms measurement timing budget
        Serial.println("[VL53L0X] Ready");
        return true;
    }

    float readMM() override {
        if (!_lox.isRangeComplete()) return -1.0f;
        VL53L0X_RangingMeasurementData_t measure;
        _lox.getRangingMeasurement(&measure, false);
        if (measure.RangeStatus == 4) return -1.0f; // phase failure / out of range
        return (float)measure.RangeMilliMeter;
    }

    const char* name() const override { return "VL53L0X"; }

private:
    Adafruit_VL53L0X _lox;
};
