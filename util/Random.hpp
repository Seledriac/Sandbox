#pragma once

// Standard lib
#include <cstdlib>


namespace Random {

  inline float Val(float const iMin, float const iMax) {
    return iMin + (iMax - iMin) * (float(rand()) / float(RAND_MAX));
  }

  inline double Val(double const iMin, double const iMax) {
    return iMin + (iMax - iMin) * (double(rand()) / double(RAND_MAX));
  }

  inline int Val(int const iMin, int const iMax) {
    if (iMax <= iMin) return iMin;
    return iMin + rand() % (iMax - iMin + 1);
  }

}  // namespace Random
