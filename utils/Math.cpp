#include <utils/Math.hpp>
#include <cmath>
#include <iostream>
#include "Math.hpp"
#include <algorithm>

int stringToInt(const std::string& input) {
    // Use std::hash to hash the string into a size_t value
    std::hash<std::string> hasher;
    size_t hashedValue = hasher(input);

    // Convert the hashed value to an int (may truncate on some systems)
    return static_cast<int>(hashedValue);
}

// Do a % b. Works with a negative a. 
int betterModulo(int a, int b) {
    return a - std::floor(a / (float)b) * b;
}

Spline::Spline(std::vector<float> intervalValues, std::vector<float> pvalues)
{
    if (intervalValues.size() < 2 || pvalues.size() < 2) {
        std::cerr << "Error: Spline interval values and values size must be at least 2 : "
                  << "intervalValues.size() = " << intervalValues.size()
                  << ", pvalues.size() = " << pvalues.size() << std::endl;
        exit(1);
    }
    if (intervalValues.size() != pvalues.size()) {
        std::cerr << "Error: Spline interval values and values size mismatch : "
                  << "intervalValues.size() = " << intervalValues.size()
                  << ", values.size() = " << values.size() << std::endl;
        exit(1);
    }
    for (size_t i = 0; i < intervalValues.size() - 1; ++i) {
        intervals.push_back(Interval(intervalValues[i], intervalValues[i + 1]));
        values.push_back({pvalues[i], pvalues[i + 1]});
    }
    sortIntervals();
    updateOffLimitSlopes();
}

Spline::Spline(std::vector<Interval> intervals, std::vector<std::pair<float, float>> values)
{
    if (intervals.size() != values.size()) {
        std::cerr << "Error: Spline intervals and values size mismatch : "
                  << "intervals.size() = " << intervals.size()
                  << ", values.size() = " << values.size() << std::endl;
        exit(1);
    }
    this->intervals = intervals;
    this->values = values;
    sortIntervals();
    updateOffLimitSlopes();
}

float Spline::getValue(float x)
{
    if (x < intervals[0].min) {
        return values[0].first + tooSmallSlope * (x - intervals[0].min);
    }
    if (x > intervals[intervals.size() - 1].max) {
        return values[values.size() - 1].first + tooBigSlope * (x - intervals[intervals.size() - 1].min);
    }
    for (size_t i = 0; i < intervals.size(); ++i) {
        if (intervals[i].isInInterval(x)) {
            float position = intervals[i].positionInInterval(x);
            return values[i].first * (1.0f - position) + values[i].second * position;
        }
    }
}

void Spline::addInterval(Interval interval, std::pair<float, float> values)
{
    intervals.push_back(interval);
    this->values.push_back(values);
    if (intervals.size() > 1) {
        intervals[intervals.size() - 2].max = interval.min;
    }
    sortIntervals();
    updateOffLimitSlopes();
}

void Spline::sortIntervals()
{
    std::sort(intervals.begin(), intervals.end(), [](const Interval &a, const Interval &b) {
        return a.min < b.min;
    });
    for (size_t i = 0; i < intervals.size() - 1; ++i) {
        intervals[i].max = intervals[i + 1].min;
    }
}
