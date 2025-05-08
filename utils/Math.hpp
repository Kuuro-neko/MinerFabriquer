#pragma once

#include <vector>
#include <utility>
#include <cmath>

// Do a % b. Works with a negative a. 
int betterModulo(int a, int b);

class Interval {
public:
    float min;
    float max;

    Interval(float min, float max) : min(min), max(max) {}

    bool isInInterval(float value) {
        return value >= min && value <= max;
    }

    // Returns the position of the value in the interval [0, 1]
    float positionInInterval(float value) {
        if (value < min) return 0.0f;
        if (value > max) return 1.0f;
        return (value - min) / (max - min);
    }

    bool isInInterval(Interval other) {
        return other.min >= min && other.max <= max;
    }
};

class Spline {
public:
    std::vector<Interval> intervals;
    std::vector<std::pair<float, float>> values;

    float tooSmallSlope = 0.0f;
    float tooBigSlope = 0.0f;
    float tooSmallSlopeOffset = 0.0f;
    float tooBigSlopeOffset = 0.0f;

    Spline() = default;
    Spline(std::vector<float> intervalValues, std::vector<float> pvalues);
    Spline(std::vector<Interval> intervals, std::vector<std::pair<float, float>> values);
    
    float getValue(float x);

    void addInterval(Interval interval, std::pair<float, float> values);

    void sortIntervals();

    void updateOffLimitSlopes() {
        if (intervals.size() < 2) return;
        tooSmallSlope = (values[0].second - values[0].first) / (intervals[0].max - intervals[0].min);
        tooBigSlope = (values[values.size() - 1].second - values[values.size() - 1].first) / (intervals[intervals.size() - 1].max - intervals[intervals.size() - 1].min);
        tooSmallSlopeOffset = values[0].first - tooSmallSlope * intervals[0].min;
        tooBigSlopeOffset = values[values.size() - 1].first - tooBigSlope * intervals[intervals.size() - 1].min;
    }
};