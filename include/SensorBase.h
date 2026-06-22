#pragma once
#include <stdint.h>

/**
 * SensorBase
 * ----------
 * Abstract interface for any distance sensor.
 * Both the real VL53L0X and the simulated sensor implement this,
 * so the rest of the codebase never needs to know which one is active.
 *
 * To swap hardware → simulated: change one line in main.cpp.
 */
class SensorBase {
public:
    virtual ~SensorBase() = default;

    // Called once in setup(). Returns true if initialisation succeeded.
    virtual bool begin() = 0;

    // Read the current distance in millimetres.
    // Returns a negative value on error.
    virtual float readMM() = 0;

    // Human-readable name for logging / UI
    virtual const char* name() const = 0;
};
