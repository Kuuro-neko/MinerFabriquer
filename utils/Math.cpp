#include <utils/Math.hpp>
#include <cmath>
#include <iostream>

// Do a % b. Works with a negative a. 
int betterModulo(int a, int b) {
    return a - std::floor(a / (float)b) * b;
}
