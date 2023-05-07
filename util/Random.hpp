#pragma once

// Standard lib
#include <cstdlib>


namespace Random {

  inline float Val(float const iMin, float const iMax) {
    return iMin + (iMax - iMin) * (float(rand()) / float(RAND_MAX));
  }

}  // namespace Random
