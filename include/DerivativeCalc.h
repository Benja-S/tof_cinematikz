#pragma once
#include "DataBuffer.h"
#include "MotionState.h"

/**
 * DerivativeCalc
 * --------------
 * Computes velocity and acceleration from a DataBuffer using
 * finite difference approximations of derivatives.
 *
 * ── What is a finite difference? ───────────────────────────────────────────
 * The derivative f'(t) = lim[h→0] (f(t+h) − f(t)) / h
 * We can't take h→0 on a microcontroller, so we use small but finite h
 * (our sampling interval Δt). This gives us an approximation.
 *
 * Two stencils are available:
 *
 * Forward difference  (order 1, less accurate):
 *   f'(t) ≈ [f(t) − f(t−Δt)] / Δt
 *   Error: O(Δt) — halving Δt halves the error
 *
 * Central difference  (order 2, more accurate):
 *   f'(t) ≈ [f(t+Δt) − f(t−Δt)] / (2Δt)
 *   Error: O(Δt²) — halving Δt quarters the error
 *   Requires 2 neighbours; we use samples at t-1 and t+1 relative to t-1
 *
 * We apply central differences twice to get second derivative (acceleration).
 * ─────────────────────────────────────────────────────────────────────────────
 */
class DerivativeCalc {
public:
    /**
     * Compute the current MotionState from the last N samples in the buffer.
     * Returns a state with valid=false if there aren't enough samples yet.
     */
    MotionState compute(const DataBuffer& buf) const {
        MotionState state;

        if (buf.count() < 3) {
            // Not enough history for central differences
            state.valid = false;
            return state;
        }

        // ── Pull three consecutive samples ──────────────────────────────────
        // newest(0) = x(t)      — most recent
        // newest(1) = x(t−Δt)  — one step back
        // newest(2) = x(t−2Δt) — two steps back
        Sample s0 = buf.newest(0);
        Sample s1 = buf.newest(1);
        Sample s2 = buf.newest(2);

        state.timestamp_ms = s0.timestamp_ms;
        state.position_mm  = s0.position_mm;

        // Δt between consecutive samples (in seconds)
        float dt1 = (s0.timestamp_ms - s1.timestamp_ms) / 1000.0f; // s0–s1
        float dt2 = (s1.timestamp_ms - s2.timestamp_ms) / 1000.0f; // s1–s2

        if (dt1 < 0.001f || dt2 < 0.001f) {
            // Guard against division by near-zero (duplicate timestamps)
            state.valid = false;
            return state;
        }

        state.dt_ms = dt1 * 1000.0f;

#if DERIVATIVE_ORDER == 1
        // ── Forward (backward) difference — first order ──────────────────────
        // v(t) ≈ [x(t) − x(t−Δt)] / Δt
        // Shows the slope of the secant line between two points.
        state.velocity_mms = (s0.position_mm - s1.position_mm) / dt1;

        // a(t) ≈ [v(t) − v(t−Δt)] / Δt
        // We need a previous velocity; approximate it from s1 and s2
        float v_prev = (s1.position_mm - s2.position_mm) / dt2;
        state.accel_mms2 = (state.velocity_mms - v_prev) / dt1;

#else
        // ── Central difference — second order (default) ──────────────────────
        // Uses samples symmetrically around the midpoint s1:
        //
        //   v(t−Δt) ≈ [x(t) − x(t−2Δt)] / (2Δt)
        //
        // This estimates velocity *at s1* using s0 and s2 as neighbours.
        // More accurate than forward difference because errors cancel.
        float dt_total = (s0.timestamp_ms - s2.timestamp_ms) / 1000.0f;

        // velocity at the midpoint s1:
        state.velocity_mms = (s0.position_mm - s2.position_mm) / dt_total;

        // acceleration at s1:
        //   a ≈ [x(t) − 2x(t−Δt) + x(t−2Δt)] / Δt²
        float dt_avg = (dt1 + dt2) / 2.0f;
        state.accel_mms2 =
            (s0.position_mm - 2.0f * s1.position_mm + s2.position_mm)
            / (dt_avg * dt_avg);
#endif

        state.valid = true;
        return state;
    }
};
