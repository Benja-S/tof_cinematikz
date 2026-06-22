#pragma once
#include "Config.h"
#include <stdint.h>

/**
 * DataBuffer
 * ----------
 * Fixed-size ring buffer holding (timestamp, distance) pairs.
 * Used by DerivativeCalc to access recent history without dynamic allocation.
 *
 * Oldest sample is automatically overwritten when the buffer is full.
 */
struct Sample {
    uint32_t timestamp_ms;
    float    position_mm;
};

class DataBuffer {
public:
    DataBuffer() : _head(0), _count(0) {}

    void push(uint32_t t, float x) {
        _data[_head] = { t, x };
        _head = (_head + 1) % BUFFER_SIZE;
        if (_count < BUFFER_SIZE) _count++;
    }

    // Access the n-th most recent sample (0 = newest, 1 = one before, ...)
    Sample newest(uint8_t n = 0) const {
        int idx = ((int)_head - 1 - (int)n + BUFFER_SIZE * 2) % BUFFER_SIZE;
        return _data[idx];
    }

    uint8_t count() const { return _count; }
    bool    full()  const { return _count == BUFFER_SIZE; }

private:
    Sample  _data[BUFFER_SIZE];
    uint8_t _head;
    uint8_t _count;
};
