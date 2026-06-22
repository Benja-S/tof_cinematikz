#pragma once
#include <stdint.h>

/**
 * MotionState
 * -----------
 * A single snapshot of the object's kinematic state at time t.
 *
 * Pedagogical note:
 *   position (x)     — raw reading from the ToF sensor, in mm
 *   velocity (v)     — first derivative of x with respect to time (dx/dt), mm/s
 *   acceleration (a) — second derivative of x with respect to time (d²x/dt²), mm/s²
 *
 * All three are computed from the same distance data; only the
 * mathematical operation applied differs.
 */
struct MotionState {
    uint32_t timestamp_ms  = 0;     // millis() at moment of reading
    float    position_mm   = 0.0f;  // x(t)   — distance to object
    float    velocity_mms  = 0.0f;  // x'(t)  — rate of change of position
    float    accel_mms2    = 0.0f;  // x''(t) — rate of change of velocity
    bool     valid         = false; // false until buffer has enough samples

    // Convenience: dt used in last derivative calculation (for display)
    float    dt_ms         = 0.0f;
};
