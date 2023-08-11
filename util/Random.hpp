#pragma once

// Standard lib
#include <cstdlib>


namespace Random {

  inline float Val(float const iMin, float const iMax) {
    return iMin + (iMax - iMin) * (float(rand()) / float(RAND_MAX));
  }

  inline int Val(int const iMin, int const iMax) {
    return iMin + rand() % (iMax - iMin);
  }

}  // namespace Random
