#pragma once
#include "SensorBase.h"
#include "Config.h"
#include <Arduino.h>
#include <math.h>

/**
 * SimulatedSensor
 * ---------------
 * Generates a sinusoidal distance signal with optional noise,
 * mimicking an object oscillating in front of the ToF sensor.
 *
 * x(t) = center + amplitude × sin(2π × t / period)
 *
 * This is ideal for teaching derivatives because:
 *   • velocity  = dx/dt = amplitude × (2π/T) × cos(2π × t / T)  — leads by 90°
 *   • accel     = d²x/dt² = −amplitude × (2π/T)² × sin(2π × t / T) — inverted
 * Students can verify our numerical derivatives match these analytical forms.
 */
class SimulatedSensor : public SensorBase {
public:
    SimulatedSensor(
        float center_mm    = SIM_CENTER_MM,
        float amplitude_mm = SIM_AMPLITUDE_MM,
        uint32_t period_ms = SIM_PERIOD_MS,
        float noise_mm     = SIM_NOISE_MM
    )
        : _center(center_mm)
        , _amplitude(amplitude_mm)
        , _period(period_ms)
        , _noise(noise_mm)
    {}

    bool begin() override {
        _start_ms = millis();
        Serial.println("[SimulatedSensor] Ready — sinusoidal motion");
        return true;
    }

    float readMM() override {
        float t   = (millis() - _start_ms) / 1000.0f;           // seconds
        float x   = _center + _amplitude * sinf(TWO_PI * t / (_period / 1000.0f));
        float noise = _noise * ((float)random(-1000, 1000) / 1000.0f);
        return x + noise;
    }

    const char* name() const override { return "SimulatedSensor (sine)"; }

    // Analytical ground-truth for UI comparison (optional)
    float analyticalVelocity() {
        float t = (millis() - _start_ms) / 1000.0f;
        float omega = TWO_PI / (_period / 1000.0f);
        return _amplitude * omega * cosf(omega * t);
    }

    float analyticalAccel() {
        float t = (millis() - _start_ms) / 1000.0f;
        float omega = TWO_PI / (_period / 1000.0f);
        return -_amplitude * omega * omega * sinf(omega * t);
    }

private:
    float    _center;
    float    _amplitude;
    uint32_t _period;
    float    _noise;
    uint32_t _start_ms = 0;
};
