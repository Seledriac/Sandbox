#pragma once

// Project lib
#include "vector.hpp"

class Boid {
public:
  Vector p, v, n;
  float size;

  Boid();
  Boid(Vector p, Vector v, Vector n, float size);
  
  void draw();
};
